#include "Umgebung.h"

using namespace umgebung;

class UmgebungApp : public PApplet {

    PVector mVector{16, 16};
    PShape  mShape;
    int     mouseMoveCounter = 0;
    int     mWidth           = 1024;
    int     mHeight          = 768;

    void arguments(std::vector<std::string> args) {
        for (std::string s: args) {
            println("> ", s);
            if (begins_with(s, "--width")) {
                mWidth = get_int_from_argument(s);
            }
            if (begins_with(s, "--height")) {
                mHeight = get_int_from_argument(s);
            }
        }
    }

    void settings() {
        size(mWidth, mHeight);
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

    void audioblock(float** input, float** output, int length) {
        for (int i = 0; i < length; i++) {
            float sample = random(-0.1, 0.1);
            for (int j = 0; j < audio_output_channels; ++j) {
                output[j][i] = sample;
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

PApplet* umgebung::instance() {
    return new UmgebungApp();
}