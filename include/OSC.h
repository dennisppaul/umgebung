/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2023 Dennis P Paul.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscPacketListener.h"
#include "osc/OscReceivedElements.h"

const static int OSC_TRANSMIT_OUTPUT_BUFFER_SIZE = 1024;

class NetAddress {
public:
    NetAddress(std::string ip_address, int port) {
        IpEndpointName mEndpointName = IpEndpointName(ip_address.c_str(), port);
        mTransmitSocket = new UdpTransmitSocket(mEndpointName);
    }

    ~NetAddress() {
        delete mTransmitSocket;
    }

    UdpTransmitSocket *socket() {
        return mTransmitSocket;
    }

private:
    UdpTransmitSocket *mTransmitSocket = nullptr;
};

class OscMessage {
public:
    // TODO in oscP5 `OscMessage` is used for both sending and receiving while in oscpack this is handled by two different classes.
    OscMessage(std::string address_pattern, bool begin_message = true) : mAddrPattern(std::move(address_pattern)) {
        if (begin_message) {
            begin();
        }
        mTypeTag = "";
    }

    std::string addrPattern() const {
        return mAddrPattern;
    }

    std::string typetag() {
        return mTypeTag;
    }

    void set_type_tag(std::string type_tag) {
        mTypeTag = std::move(type_tag);
    }

    std::size_t size() const {
        return p.Size();
    }

    const char *data() const {
        return p.Data();
    }

    void begin() {
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(mAddrPattern.c_str());
    }

    void end() {
        // TODO compile current typetag into `mTypeTag`
        p << osc::EndMessage
          << osc::EndBundle;
    }

    void add(float value) {
        p << value;
    }

    void add(int value) {
        p << value;
    }

    void add(std::string value) {
        p << value.c_str();
    }

private:
    const std::string mAddrPattern;
    std::string mTypeTag;
    char buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p{buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE};
};

class OSCListener {
public:
    virtual void receive(const osc::ReceivedMessage &msg) = 0;

    // TODO replace `osc::ReceivedMessage` with `OscMessage`
    virtual void receive_TODO(const OscMessage &msg) {};

//    void receive_TODO(const OscMessage &msg) {
//        // TODO not properly implemented yet. will soon replace `receive(const osc::ReceivedMessage &msg)`
//        std::cout << "received address pattern: " << msg.addrPattern() << std::endl;
//        /* // from oscP5 example
//         if(theOscMessage.checkAddrPattern("/test")==true) {
//            if(theOscMessage.checkTypetag("ifs")) {
//                // parse theOscMessage and extract the values from the osc message arguments.
//                int firstValue = theOscMessage.get(0).intValue();
//                float secondValue = theOscMessage.get(1).floatValue();
//                String thirdValue = theOscMessage.get(2).stringValue();
//                print("### received an osc message /test with typetag ifs.");
//                println(" values: "+firstValue+", "+secondValue+", "+thirdValue);
//                return;
//            }
//        }
//        */
//    }

};

class OSC {
public:
    OSC(std::string transmit_address, int transmit_port, int receive_port, bool use_UDP_multicast = true) :
            fTransmitAddress(std::move(transmit_address)),
            fTransmitPort(transmit_port),
            fReceivePort(receive_port),
            fUseUDPMulticast(use_UDP_multicast) {
        mOSCThread = std::thread(&OSC::osc_thread, this);

        IpEndpointName mEndpointName = IpEndpointName(fTransmitAddress.c_str(), fTransmitPort);
        mTransmitSocket = new UdpTransmitSocket(mEndpointName);
    }

    OSC(int receive_port, bool use_UDP_multicast = true) :
            fTransmitAddress(""),
            fTransmitPort(-1),
            fReceivePort(receive_port),
            fUseUDPMulticast(use_UDP_multicast) {
        mOSCThread = std::thread(&OSC::osc_thread, this);
        mTransmitSocket = nullptr;
    }

    ~OSC() {
        mOSCThread.detach();
        if (mTransmitSocket != nullptr) {
            delete mTransmitSocket;
        }
    }

    /* callback */

    void (OSCListener::*callback_)(const osc::ReceivedMessage &);

    void (OSCListener::*callback_TODO)(const OscMessage &msg);

    OSCListener *instance_;

//    void register_callback(void (OSCListener::*callback)(const osc::ReceivedMessage &), OSCListener *instance) {
//        callback_ = callback;
//        instance_ = instance;
//    }

    void callback(OSCListener *instance) {
        callback_ = &OSCListener::receive;
        callback_TODO = &OSCListener::receive_TODO;
        instance_ = instance;
    }

    void invoke_callback(const osc::ReceivedMessage &msg) {
        // TODO transfer payload from osc::ReceivedMessage to OscMessage
        OscMessage msg_TODO(msg.AddressPattern());
        (instance_->*callback_TODO)(msg_TODO);
        (instance_->*callback_)(msg);
    }

    /* send */

    void send(OscMessage message, NetAddress address) {
        message.end();
        address.socket()->Send(message.data(), message.size());
    }

    template<typename... Args>
    void send(const std::string &addr_pattern, Args... args) {
        char buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
        osc::OutboundPacketStream p(buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE);
        p << osc::BeginBundleImmediate
          << osc::BeginMessage(addr_pattern.c_str());
        addArgsToPacketStream(p, args...);
        p << osc::EndMessage
          << osc::EndBundle;
        if (mTransmitSocket != nullptr) {
            mTransmitSocket->Send(p.Data(), p.Size());
        } else {
            std::cerr << "+++ OSC error: no transmit socket created" << std::endl;
        }
    }

//    void process_KLANG_OSC_CMD(const osc::ReceivedMessage &msg) {
//#ifdef DEBUG_OSC
//        KLANG_LOG("@sdlApp onReceive *KLANG_OSC_CMD*     : %s", KLANG_OSC_CMD);
//#endif
//        uint8_t mData[msg.ArgumentCount()];
//        uint8_t i = 0;
//        for (osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
//             arg != msg.ArgumentsEnd(); ++arg) {
//            mData[i] = (uint8_t) arg->AsInt32();
//            i++;
//        }
//        data_receive(ALL_PERIPHERALS, mData, msg.ArgumentCount());
//    }

//    void process_KLANG_OSC_DATA(const osc::ReceivedMessage &msg) {
//#ifdef DEBUG_OSC
//        KLANG_LOG("@sdlApp onReceive *KLANG_OSC_DATA* : %s", KLANG_OSC_DATA);
//#endif
//        if (msg.ArgumentCount() > 1) {
//            const uint8_t mLength = msg.ArgumentCount() - 1;
//            osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
//            const uint8_t mSender = (uint8_t) arg->AsInt32();
//            ++arg;
//            uint8_t mData[mLength];
//            uint8_t i = 0;
//            while (arg != msg.ArgumentsEnd()) {
//                mData[i] = (uint8_t) arg->AsInt32();
//                ++arg;
//                ++i;
//            }
//            data_receive(mSender, mData, mLength);
//        }
//    }

//    void process_KLANG_OSC_MIDI_IN(const osc::ReceivedMessage &msg) {
//#ifdef DEBUG_OSC
//        KLANG_LOG("@sdlApp onReceive *KLANG_OSC_MIDI_IN* : %s", KLANG_OSC_MIDI_IN);
//#endif
//        EVENT_TYPE mMidiInEvent;
//        if (msg.ArgumentCount() > 0) {
//            osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
//            mMidiInEvent = (EVENT_TYPE) arg->AsInt32();
//            if (msg.ArgumentCount() > 1) {
//                float mData[msg.ArgumentCount() - 1];
//                uint8_t i = 0;
//                for (osc::ReceivedMessage::const_iterator args = msg.ArgumentsBegin();
//                     args != msg.ArgumentsEnd(); ++args) {
//                    if (i == 0) {
//                        args++;
//                    }
//                    mData[i] = (float) args->AsInt32();
//                    i++;
//                }
//                event_receive(mMidiInEvent, mData);
//            } else {
//                event_receive(mMidiInEvent, nullptr);
//            }
//        }
//    }

private:
    const std::string fTransmitAddress;
    const int fTransmitPort;
    const int fReceivePort;
    const bool fUseUDPMulticast;
    std::thread mOSCThread;
    UdpTransmitSocket *mTransmitSocket = nullptr;

    template<typename T, typename... Rest>
    [[maybe_unused]] void addArgsToPacketStream(osc::OutboundPacketStream &p, T first, Rest... rest) {
        p << first;
        addArgsToPacketStream(p, rest...);
    }

    void addArgsToPacketStream([[maybe_unused]] osc::OutboundPacketStream &p) {}

    class MOscPacketListener : public osc::OscPacketListener {
    public:
        MOscPacketListener(OSC *parent) : mParent(parent) {}

        void process(const osc::ReceivedMessage &msg) {
            mParent->invoke_callback(msg);

//            // TODO maybe parse this into OscMessage
//            OscMessage m{std::string(msg.AddressPattern())};
//            // copy message types from osc::ReceivedMessage to OscMessage
//            osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
//            while (arg != msg.ArgumentsEnd()) {
//            }

//        if (msg.ArgumentCount() > 0) {
//            osc::ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
//            int mMidiInEvent = arg->AsInt32();
            // (msg.ArgumentCount() > 1) {
//            float mData[msg.ArgumentCount()];
//            uint8_t i = 0;
//            for (osc::ReceivedMessage::const_iterator args = msg.ArgumentsBegin();
//                 args != msg.ArgumentsEnd(); ++args) {
//                mData[i] = (float) args->AsInt32();
//                i++;
//            }
//        }
        }

    private:
        OSC *mParent;

        [[maybe_unused]] bool addr_pattern_equals(const osc::ReceivedMessage &msg, const char *pAddrPatter) {
            return (strcmp(msg.AddressPattern(), pAddrPatter) == 0);
        }

    protected:
        void ProcessMessage(const osc::ReceivedMessage &msg,
                            const IpEndpointName &remoteEndpoint) override {
            (void) remoteEndpoint;  // suppress unused parameter warning
            try {
                process(msg);
            } catch (osc::Exception &e) {
                std::cerr << "+++ OSC receive error: " << e.what() << std::endl;
            }
        }
    };

    void osc_thread() {
        try {
            if (fUseUDPMulticast) {
                MOscPacketListener mOscListener(this);
                PacketListener *listener_ = &mOscListener;
                IpEndpointName mIpEndpointName = IpEndpointName(fTransmitAddress.c_str(), fReceivePort);
                if (mIpEndpointName.IsMulticastAddress()) {
                    UdpSocket mUdpSocket;
                    mUdpSocket.SetAllowReuse(true);
                    mUdpSocket.Bind(mIpEndpointName);
                    SocketReceiveMultiplexer mux_;
                    mux_.AttachSocketListener(&mUdpSocket, listener_);
                    mux_.Run();
                } else {
                    UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, fReceivePort),
                                                &mOscListener);
                    s.Run();
                }
            } else {
                MOscPacketListener mOscListener(this);
                UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, fReceivePort), &mOscListener);
                s.Run();
            }
        } catch (std::exception &e) {
            std::cerr << "+++ OSC receive error: " << e.what() << std::endl;
        }
    }
};