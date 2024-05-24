#include "Umgebung.h"
#include "osc/OSC.h"

using namespace umgebung;

class UmgebungExampleAppWithOSC : public PApplet, OSCListener {

    OSC  mOSC{"127.0.0.1", 7001, 7000};
    bool message_event   = false;
    int  message_counter = 0;

    void receive(const OscMessage& msg) {
        if (msg.typetag() == "ifs") {
            println("received address pattern: ",
                    msg.addrPattern(),
                    " : ",
                    msg.typetag(),
                    "(",
                    msg.get(0).intValue(),
                    ", ",
                    msg.get(1).floatValue(),
                    ", ",
                    msg.get(2).stringValue(),
                    ")");
            message_event = true;
        } else {
            println("could not parse OSC message: ", msg.typetag());
        }
    }

    void settings() {
        size(1024, 768);
        no_audio = true;
    }

    void setup() {
        mOSC.callback(this);
        background(0);

        println("sizeof(int): ", sizeof(int));
    }

    void draw() {
        if (message_event) {
            message_event = false;
            fill(1);
            const int   num_rects = 20;
            const float size_rect = width / num_rects;
            float       x         = (message_counter % num_rects) * size_rect;
            float       y         = (message_counter / num_rects) * size_rect;
            rect(x, y, size_rect, size_rect);
            message_counter++;
            if (message_counter > num_rects * num_rects) {
                message_counter = 0;
                background(0);
            }
        }
    }

    void keyPressed() {
        if (key == 'q') {
            exit();
        }
        if (key == 's') {
            std::cout << "send OSC message" << std::endl;
            mOSC.send("/test_send_1", 23, "hello", 42);

            OscMessage msg("/test_send_2");
            msg.add(mouseY);
            mOSC.send(msg, NetAddress("localhost", 8000));

            message_event = true;
        }
    }
};

PApplet* umgebung::instance() {
    return new UmgebungExampleAppWithOSC();
}