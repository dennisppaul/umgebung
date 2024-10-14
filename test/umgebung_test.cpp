#include "Umgebung.h"

using namespace umgebung;

class UmgebungApp : public PApplet {

    PShape mShape;
    int    mWidth  = 1024;
    int    mHeight = 768;

    void arguments(std::vector<std::string> args) override {
        for (std::string& s: args) {
            println("> ", s);
            if (begins_with(s, "--width")) {
                mWidth = get_int_from_argument(s);
            }
            if (begins_with(s, "--height")) {
                mHeight = get_int_from_argument(s);
            }
        }
    }

    void settings() override {
        size(mWidth, mHeight);
        audio_input_device    = DEFAULT_AUDIO_DEVICE;
        audio_output_device   = DEFAULT_AUDIO_DEVICE;
        audio_input_channels  = DEFAULT_NUMBER_OF_INPUT_CHANNELS;
        audio_output_channels = DEFAULT_NUMBER_OF_OUTPUT_CHANNELS;
        monitor               = 0;
        fullscreen            = false;
        borderless            = true;
        antialiasing          = 8;
        resizable             = false;
        always_on_top         = true;
        enable_retina_support = true;
        headless              = true;
        no_audio              = true;
    }

    void setup() override {
        println("width : ", width);
        println("height: ", height);
        if (headless) {
            println("headless mode");
            exit();
        }
    }

    void draw() override {
        background(1.0f);

        stroke(0.0f);
        noFill();
        rect(10, 10, width / 2 - 20, height / 2 - 20);

        noStroke();
        fill(random(0, 0.2));
        rect(20, 20, width / 2 - 40, height / 2 - 40);

        if (frameCount > 60) {
            println("exiting gracefully");
            exit();
        }
    }

    void audioblock(float** input, float** output, int length) override {
        for (int i = 0; i < length; i++) {
            float sample = random(-0.1, 0.1);
            for (int j = 0; j < audio_output_channels; ++j) {
                output[j][i] = sample;
            }
        }
    }

    void keyPressed() override {
        if (key == 'q') {
            exit();
        }
        println((char) key, " pressed");
    }
};

PApplet* umgebung::instance() {
    return new UmgebungApp();
}