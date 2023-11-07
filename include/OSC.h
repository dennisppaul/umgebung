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

class OSCListener {
public:
    virtual void receive(const osc::ReceivedMessage &msg) = 0;
};

class OscMessage {
public:
    OscMessage(std::string address_pattern) : mAddrPattern(std::move(address_pattern)) {}

    std::string addrPattern() const {
        return mAddrPattern;
    }

    std::string typetag() {
        return "";
    }

    std::size_t size() const {
        return p.Size();
    }

    const char *data() const {
        return p.Data();
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
    std::string mAddrPattern;
    char buffer[OSC_TRANSMIT_OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p{buffer, OSC_TRANSMIT_OUTPUT_BUFFER_SIZE};
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

    ~OSC() {
        mOSCThread.detach();
    }

    /* callback */

    void (OSCListener::*callback_)(const osc::ReceivedMessage &);

    OSCListener *instance_;

//    void register_callback(void (OSCListener::*callback)(const osc::ReceivedMessage &), OSCListener *instance) {
//        callback_ = callback;
//        instance_ = instance;
//    }

    void callback(OSCListener *instance) {
        callback_ = &OSCListener::receive;
        instance_ = instance;
    }

    void invoke_callback(const osc::ReceivedMessage &msg) {
        (instance_->*callback_)(msg);
    }

    /* send */

    void send(OscMessage &message) {
        mTransmitSocket->Send(message.data(), message.size());
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
        mTransmitSocket->Send(p.Data(), p.Size());
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
                std::cerr << "+++ OSC error: " << e.what() << std::endl;
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
            std::cerr << "+++ OSC error: " << e.what() << std::endl;
        }
    }
};