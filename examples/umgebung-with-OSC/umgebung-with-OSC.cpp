#include "Umgebung.h"
#include "osc/OSC.h"

class UmgebungExampleAppWithOSC : public PApplet, OSCListener {

    OSC mOSC{"127.0.0.1", 8000, 8001};

    void receive(const OscMessage &msg) {
        if (msg.typetag() == "ifs") {
            println("received address pattern: ",
                    println(msg.addrPattern(),
                    " : ",
                    msg.typetag(),
                    "(",
                    msg.get(0).intValue(),
                    ", ",
                    msg.get(1).floatValue(),
                    ", ",
                    msg.get(2).stringValue(),
                    ")"
            );
        } else {
            println("could not parse OSC message: ", msg.typetag());
        }
    }

    void settings() {
        size(1024, 768);
        headless = false;
        no_audio = true;
    }

    void setup() {
        mOSC.callback(this);
    }

    void draw() {
        background(0);
    }

    void audioblock(const float *input, float *output, unsigned long length) {}

    void keyPressed() {
        if (key == 'Q') {
            exit();
        }
        if (key == 'S') {
            std::cout << "send OSC message" << std::endl;
            mOSC.send("/test_send_1", 23, "hello", 42);

            OscMessage msg("/test_send_2");
            msg.add(mouseY);
            mOSC.send(msg, NetAddress("localhost", 8000));
        }
    }
};

PApplet *instance() {
    return new UmgebungExampleAppWithOSC();
}