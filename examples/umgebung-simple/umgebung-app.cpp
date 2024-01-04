#include "Umgebung.h"

using namespace umgebung;

class UmgebungApp : public PApplet {

    PVector mVector{16, 16};
    PShape  mShape;
    int     mouseMoveCounter = 0;

    void settings() {
        size(1024, 768);
        antialiasing          = 8;
        enable_retina_support = true;
        headless              = false;
        no_audio              = false;
        monitor               = DEFAULT;
    }

    void setup() {
        println("width : ", width);
        println("height: ", height);
    }

    void draw() {
        background(1);

        stroke(0);
        noFill();
        rect(10, 10, width / 2 - 20, height / 2 - 20);

        noStroke();
        fill(random(0, 0.2));
        rect(20, 20, width / 2 - 40, height / 2 - 40);
    }

    void audioblock(const float *input, float *output, int length) {
        for (int i = 0; i < length; i++) {
            float    sample = random(-0.1, 0.1);
            for (int j      = 0; j < audio_output_channels; ++j) {
                output[i * audio_output_channels + j] = sample;
            }
        }
    }

    void keyPressed() {
        if (key == 'q') {
            exit();
        }
        println((char) key, " pressed");
    }
};

PApplet *umgebung::instance() {
    return new UmgebungApp();
}