#include "Umgebung.h"

PFont *mFont;
PImage *mImage;
PVector mVector;
int mouseMoveCounter = 0;

void settings() {
    size(1024, 768);
    audio_devices(DEFAULT_AUDIO_DEVICE, DEFAULT_AUDIO_DEVICE);
    antialiasing = DEFAULT;
}

void setup() {
    // note that font + image are not included
    mFont = loadFont("../RobotoMono-Regular.ttf", 48);
    mImage = loadImage("../image.png");
    textFont(mFont);

    println("width : ", width);
    println("height: ", height);
}

void draw() {
    background(1, 1, 1);
    background(1);

    /* rectangle */
    const float padding = width / 16.0;
    const float grid = width / 16.0;
    const float spacing = grid + width / 32.0;

    stroke(1, 0, 0);
    noFill();
    rect(padding, padding, grid, grid);

    noStroke();
    fill(0, 1, 0);
    rect(padding + spacing, padding, grid, grid);

    stroke(0.75);
    fill(0, 0, 1);
    rect(padding + 2 * spacing, padding, grid, grid);

    /* line */
    stroke(0);
    line(padding + 3 * spacing, padding, padding + 3 * spacing + grid, padding + grid);
    line(padding + 3 * spacing, padding + grid, padding + 3 * spacing + grid, padding);

    /* text + nf + push/popMatrix */
    fill(0);
    noStroke();
    textSize(48);
    text("23", padding + 4 * spacing, padding + grid);

    pushMatrix();
    translate(mouseX, mouseY);
    rotate(PI * 0.25);
    textSize(11);
    text(to_string((int) mouseX, ", ", (int) mouseY, " > ", nf(mouseMoveCounter, 4)), 0, 0);
    popMatrix();

    /* image */
    fill(1);
    image(mImage, padding, padding + spacing, grid, grid);
    image(mImage, padding + spacing, padding + spacing);

    /* noise + point */
    pushMatrix();
    translate(padding, padding + 2 * spacing);
    for (int i = 0; i < grid * grid; ++i) {
        float x = i % (int) grid;
        float y = i / grid;
        float grey = noise(x / (float) grid, y / (float) grid);
        stroke(grey);
        point(x, y, 1);
    }
    popMatrix();

    fill(1, 0, 0);
    beginShape();
    vertex(padding, padding + 3 * spacing);
    vertex(padding, padding + 3 * spacing + grid);
    vertex(padding + grid, padding + 3 * spacing + grid);
    vertex(padding + grid, padding + 3 * spacing);
    endShape();
}

void audioblock(const float *input, float *output, unsigned long length) {
    // NOTE length is the number of samples per channel
    // TODO change to `void audioblock(float** input_signal, float** output_signal) {}`
    static float phase = 0.0;
    float frequency = 220.0 + sin(frameCount * 0.1) * 110.0;
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
    println("key           : ", (char) key);
}

void mouseMoved() {
    mouseMoveCounter++;
}

void mousePressed() {
    println("mouse button  : ", mouseButton);
    exit();
}

void finish() {
    println("application shutting down");
}