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

    void openInputPort(unsigned int port = 0) {
        midiIn->openPort(port);
        midiIn->setCallback(&MIDI::midiInputCallback, this);
        midiIn->ignoreTypes(false, false, false); // Don't ignore sysex, timing, or active sensing messages.
    }

    void openOutputPort(unsigned int port = 0) {
        midiOut->openPort(port);
    }

    void sendNoteOn(int note, int velocity, int channel = 0) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(144 + channel), static_cast<unsigned char>(note), static_cast<unsigned char>(velocity)
        };
        midiOut->sendMessage(&message);
    }

    void sendNoteOff(int note, int channel = 0) {
        std::vector<unsigned char> message = {static_cast<unsigned char>(128 + channel), static_cast<unsigned char>(note), 0};
        midiOut->sendMessage(&message);
    }

private:
    RtMidiIn* midiIn;
    RtMidiOut* midiOut;

    static void midiInputCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
        auto* handler = static_cast<MIDI *>(userData);
        handler->handleMidiInput(deltatime, *message);
    }

    void handleMidiInput(double deltatime, const std::vector<unsigned char>& message) {
        // Handle incoming messages. For example, print them or handle MIDI clock messages.
        if (message.size() > 0) {
            std::cout << "Received MIDI message: ";
            for (size_t i = 0; i < message.size(); ++i) {
                std::cout << static_cast<int>(message[i]) << " ";
            }
            std::cout << std::endl;
        }
    }
};
