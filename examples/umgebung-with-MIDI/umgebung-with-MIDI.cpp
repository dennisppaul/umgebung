#include "Umgebung.h"
#include "midi/MIDI.h"

using namespace umgebung;

class UmgebungExampleAppWithMIDI : public PApplet, MIDIListener {
    MIDI midiHandler;

    void settings() {
        size(1024, 768);
        no_audio = true;
    }

    void setup() {
        midiHandler.print_available_ports();
        midiHandler.open_input_port("IAC Driver Bus 1");
        midiHandler.open_output_port(0);
        midiHandler.callback(this);

        background(0);
    }

    void draw() {
        background(0);
    }

    void mousePressed() {
        println("send note_on");
        midiHandler.note_on(0, 60, 112);
    }

    void mouseReleased() {
        println("send note_off");
        midiHandler.note_off(0, 60);
    }

    void note_on(int, int, int) {
        println("received note_on");
    }

    void note_off(int, int) {
        println("received note_off");
    }

    void keyPressed() {
        if (key == 'q') {
            exit();
        }
    }

    void receive_native(const std::vector<unsigned char>& message) {
        if (message.size() > 0) {
            std::cout << "Received MIDI message: ";
            for (size_t i = 0; i < message.size(); ++i) {
                std::cout << static_cast<int>(message[i]) << " ";
            }
            std::cout << std::endl;
        }
    };
};

PApplet* umgebung::instance() {
    return new UmgebungExampleAppWithMIDI();
}
