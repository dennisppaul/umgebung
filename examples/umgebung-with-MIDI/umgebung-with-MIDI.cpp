#include "Umgebung.h"
#include "midi/MIDI.h"

#include <thread>
#include <chrono>

using namespace umgebung;

class UmgebungExampleAppWithOSC : public PApplet {
    void settings() {
        size(1024, 768);
        no_audio = true;
    }

    void setup() {
        MIDI midiHandler;
        midiHandler.openInputPort(); // Open first available input port
        midiHandler.openOutputPort(); // Open first available output port

        midiHandler.sendNoteOn(60, 112); // Send a middle C note on
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second
        midiHandler.sendNoteOff(60); // Send a middle C note off

        background(0);
    }

    void draw() {
        background(0);
    }

    void keyPressed() {
        if (key == 'Q') {
            exit();
        }
    }
};

PApplet* umgebung::instance() {
    return new UmgebungExampleAppWithOSC();
}
