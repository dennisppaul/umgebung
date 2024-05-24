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

#include <iostream>
#include <RtMidi.h>
#include <vector>

class MIDIListener {
public:
    virtual void message(const std::vector<unsigned char>& message) {};

    virtual void note_off(int channel, int note) {};

    virtual void note_on(int channel, int note, int velocity) {};

    virtual void control_change(int channel, int control, int value) {};

    virtual void program_change(int channel, int program) {};

    //    virtual void aftertouch(int channel, int pressure) {};

    virtual void pitch_bend(int channel, int value) {};

    //    virtual void poly_aftertouch(int channel, int note, int pressure) {};

    virtual void sys_ex(const std::vector<unsigned char>& message) {};
};

class MIDI {
public:
    MIDI() {
        midiIn  = new RtMidiIn();
        midiOut = new RtMidiOut();
    }

    ~MIDI() {
        delete midiIn;
        delete midiOut;
    }

    void print_available_ports() {
        // Listing MIDI Input Ports
        unsigned int nInPorts = midiIn->getPortCount();
        std::cout << "+++ MIDI input ports" << std::endl;
        for (unsigned int i = 0; i < nInPorts; i++) {
            try {
                std::string portName = midiIn->getPortName(i);
                std::cout << "+++\t" << i << ":\t" << portName << std::endl;
            } catch (RtMidiError& error) {
                error.printMessage();
            }
        }
        std::cout << "+++ " << std::endl;

        // Listing MIDI Output Ports
        unsigned int nOutPorts = midiOut->getPortCount();
        std::cout << "+++ MIDI output ports" << std::endl;
        for (unsigned int i = 0; i < nOutPorts; i++) {
            try {
                std::string portName = midiOut->getPortName(i);
                std::cout << "+++\t" << i << ":\t" << portName << std::endl;
            } catch (RtMidiError& error) {
                error.printMessage();
            }
        }
    }

    void open_input_port(unsigned int port = 0) {
        if (port >= midiIn->getPortCount()) {
            std::cerr << "+++ MIDI error: port " << port << " not available" << std::endl;
            return;
        }
        midiIn->openPort(port);
        midiIn->setCallback(&MIDI::midiInputCallback, this);
        midiIn->ignoreTypes(false, false, false); // Don't ignore sysex, timing, or active sensing messages.
    }

    void open_output_port(unsigned int port = 0) {
        if (port >= midiOut->getPortCount()) {
            std::cerr << "+++ MIDI error: port " << port << " not available" << std::endl;
            return;
        }
        midiOut->openPort(port);
    }

    void open_input_port(const std::string& portName) {
        unsigned int portNumber = find_port(portName, true);
        open_input_port(portNumber);
    }

    void open_output_port(const std::string& portName) {
        unsigned int portNumber = find_port(portName, false);
        open_output_port(portNumber);
    }

    unsigned int find_port(const std::string& portName, bool isInput) {
        RtMidi*      midi   = isInput ? static_cast<RtMidi*>(midiIn) : static_cast<RtMidi*>(midiOut);
        unsigned int nPorts = midi->getPortCount();

        for (unsigned int i = 0; i < nPorts; ++i) {
            try {
                std::string name = midi->getPortName(i);
                if (name == portName) {
                    return i;
                }
            } catch (RtMidiError& error) {
                error.printMessage();
            }
        }

        std::cerr << "+++ port '" << portName << "' not found. using default port." << std::endl;
        return 0;
    }

    void note_off(int channel, int note) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(128 + channel),
            static_cast<unsigned char>(note),
            0};
        midiOut->sendMessage(&message);
    }

    void note_on(int channel, int note, int velocity) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(144 + channel),
            static_cast<unsigned char>(note),
            static_cast<unsigned char>(velocity)};
        midiOut->sendMessage(&message);
    }

    void control_change(int channel, int control, int value) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(176 + channel),
            static_cast<unsigned char>(control),
            static_cast<unsigned char>(value)};
        midiOut->sendMessage(&message);
    }

    void program_change(int channel, int program) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(192 + channel),
            static_cast<unsigned char>(program)};
        midiOut->sendMessage(&message);
    }

    void pitch_bend(int channel, int value) {
        // Pitch bend value is a 14-bit value (0 to 16383).
        // It is split into two 7-bit values: MSB and LSB.
        int lsb = value & 0x7F;
        int msb = (value >> 7) & 0x7F;

        std::vector<unsigned char> message = {
            static_cast<unsigned char>(224 + channel),
            static_cast<unsigned char>(lsb),
            static_cast<unsigned char>(msb)};
        midiOut->sendMessage(&message);
    }

    void channel_pressure(int channel, int pressure) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(208 + channel),
            static_cast<unsigned char>(pressure)};
        midiOut->sendMessage(&message);
    }

    void sysEx(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> message;
        message.push_back(0xF0); // Start of SysEx
        message.insert(message.end(), data.begin(), data.end());
        message.push_back(0xF7); // End of SysEx
        midiOut->sendMessage(&message);
    }

    void callback(MIDIListener* instance) {
        callback_message = &MIDIListener::message;
        //        callback_note_on         = &MIDIListener::note_on;
        //        callback_note_off        = &MIDIListener::note_off;
        //        callback_control_change  = &MIDIListener::control_change;
        //        callback_program_change  = &MIDIListener::program_change;
        //        callback_aftertouch      = &MIDIListener::aftertouch;
        //        callback_pitch_bend      = &MIDIListener::pitch_bend;
        //        callback_poly_aftertouch = &MIDIListener::poly_aftertouch;
        fInstance = instance;
    }

private:
    RtMidiIn*     midiIn;
    RtMidiOut*    midiOut;
    MIDIListener* fInstance;

    void (MIDIListener::*callback_message)(const std::vector<unsigned char>& message);

    //    void (MIDIListener::*callback_note_on)(int, int, int);
    //    void (MIDIListener::*callback_note_off)(int, int);
    //    void (MIDIListener::*callback_control_change)(int, int, int);
    //    void (MIDIListener::*callback_program_change)(int, int);
    //    void (MIDIListener::*callback_aftertouch)(int, int);
    //    void (MIDIListener::*callback_pitch_bend)(int, int);
    //    void (MIDIListener::*callback_poly_aftertouch)(int, int, int);

    void invoke_callback(const std::vector<unsigned char>& message) {
        if (fInstance == nullptr) {
            std::cerr << "+++ MIDI error: no callback instance" << std::endl;
            return;
        }

        if (message.size() < 2) {
            std::cerr << "+++ MIDI error: malformed message ( < 2 )" << std::endl;
            return;
        }

        int status  = message[0];
        int channel = status & 0x0F;
        int command = status & 0xF0;

        switch (command) {
            case 0x80: // Note Off
                if (message.size() >= 2) {
                    (fInstance->*(&MIDIListener::note_off))(channel,
                                                            static_cast<int>(message[1]));
                }
                break;
            case 0x90: // Note On
                if (message.size() >= 3) {
                    (fInstance->*(&MIDIListener::note_on))(channel,
                                                           static_cast<int>(message[1]),
                                                           static_cast<int>(message[2]));
                }
                break;
            case 0xB0: // Control Change
                if (message.size() >= 3) {
                    (fInstance->*(&MIDIListener::control_change))(channel,
                                                                  static_cast<int>(message[1]),
                                                                  static_cast<int>(message[2]));
                }
                break;
            case 0xC0: // Program Change
                if (message.size() >= 2) {
                    (fInstance->*(&MIDIListener::program_change))(channel,
                                                                  static_cast<int>(message[1]));
                }
                break;
            case 0xE0: // Pitch Bend
                if (message.size() >= 3) {
                    const int value = (static_cast<int>(message[2]) << 7) | static_cast<int>(message[1]);
                    (fInstance->*(&MIDIListener::pitch_bend))(channel,
                                                              value);
                }
                break;
            default:
                // TODO remocve this eventually
                std::cout << "unknown command: " << command << std::endl;
        }

        if (message[0] == 0xF0) { // SysEx
            (fInstance->*(&MIDIListener::sys_ex))(message);
            return;
        }

        /* also send native message … for debugging */
        (fInstance->*callback_message)(message);
    }

    static void midiInputCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
        auto* handler = static_cast<MIDI*>(userData);
        handler->invoke_callback(*message);
    }
};
