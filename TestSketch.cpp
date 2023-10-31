#include <iostream>
#include "Umgebung.h"

void settings() {
    size(2048, 1280);
    audio_devices(DEFAULT_AUDIO_DEVICE, DEFAULT_AUDIO_DEVICE);
    antialiasing = 32;
}

void setup() {
}

void draw() {
    background(0, 0, 0);
    float padding = random(50, 70);
    noStroke();
    stroke(1, 0, 1);
    fill(1, 1, 1);
    rect(padding, padding, width - 2 * padding, height - 2 * padding);
    stroke(0.5, 0.5, 0.5);
    noFill();
    line(0, 0, mouseX, mouseY);
}

void audioblock(const float *input, float *output, unsigned long length) {
    // NOTE length is the number of samples per channel
    // TODO change to `void audioblock(float** input_signal, float** output_signal) {}`
    const float TWO_PI = 6.283185307179586;
    static float phase = 0.0;
    float frequency = 440.0;
    float amplitude = 0.5;

    for (int i = 0; i < length; i++) {
        float sample = amplitude * sin(phase);
        phase += (TWO_PI * frequency) / SAMPLE_RATE;

        if (phase >= TWO_PI) {
            phase -= TWO_PI;
        }

        float mInput = 0;
        for (int j = 0; j < NUMBER_OF_INPUT_CHANNELS; ++j) {
            mInput += input[i * NUMBER_OF_INPUT_CHANNELS + j];
        }
        for (int j = 0; j < NUMBER_OF_OUTPUT_CHANNELS; ++j) {
            output[i * NUMBER_OF_OUTPUT_CHANNELS + j] = sample + mInput * 0.5f;
        }
    }
}

void keyPressed() {
    println("key  : ", (char) key);
}

void mouseMoved() {
    println("mouse move    : ", mouseX, ", ", mouseY);
}

void mousePressed() {
    println("mouse button  : ", mouseButton);
}