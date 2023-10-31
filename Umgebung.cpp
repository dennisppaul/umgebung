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

#include <GLFW/glfw3.h>

#include <iostream>
#include <portaudio.h>

#include "Umgebung.h"

/* public */

int width = DEFAULT_WINDOW_WIDTH;
int height = DEFAULT_WINDOW_HEIGHT;
float mouseX = 0;
float mouseY = 0;
float pmouseX = 0;
float pmouseY = 0;
int mouseButton = 0;
int key = 0;
int frameCount = 0;
float frameRate = 0;
int audio_input_device = DEFAULT_AUDIO_DEVICE;
int audio_output_device = DEFAULT_AUDIO_DEVICE;
int monitor = DEFAULT;
int antialiasing = DEFAULT;

/* private */

static bool fAppIsRunning = true;
static const double fTargetFrameTime = 1.0 / 60.0; // @development
static bool fAppIsInitialized = false;
static bool fMouseIsPressed = false;

int audioCallback(const void *inputBuffer,
                  void *outputBuffer,
                  unsigned long audioFrameCount,
                  const PaStreamCallbackTimeInfo *timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData) {
    auto *out = (float *) outputBuffer;
    auto *in = (const float *) inputBuffer;

    audioblock(in, out, audioFrameCount);
    return paContinue;
}

int print_audio_devices() {
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(numDevices));
        return numDevices;
    }

    printf("Available audio devices:\n");

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        printf("- Device %d: %s\n", i, deviceInfo->name);
        printf("  - Max Input Channels .... : %d\n", deviceInfo->maxInputChannels);
        printf("  - Max Output Channels ... : %d\n", deviceInfo->maxOutputChannels);
        printf("  - Default Sample Rate ... : %8.2f\n", deviceInfo->defaultSampleRate);
    }

    return paNoError;
}

PaStream *init_audio(int input_channels, int output_channels) {
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error (@init): " << Pa_GetErrorText(err) << std::endl;
        return nullptr;
    }

    PaStream *stream;
    if (audio_input_device == DEFAULT_AUDIO_DEVICE || audio_output_device == DEFAULT_AUDIO_DEVICE) {
        std::cout << "+++ using default audio input and output devices" << std::endl;
        err = Pa_OpenDefaultStream(&stream,
                                   input_channels,
                                   output_channels,
                                   paFloat32,
                                   SAMPLE_RATE,
                                   FRAMES_PER_BUFFER,
                                   audioCallback,
                                   nullptr);
    } else {
        print_audio_devices();

        PaStreamParameters inputParameters, outputParameters;
        inputParameters.device = audio_input_device;
        inputParameters.channelCount = input_channels;
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(audio_input_device)->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = nullptr;
        outputParameters.device = audio_output_device;
        outputParameters.channelCount = output_channels;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(audio_output_device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = nullptr;
        err = Pa_OpenStream(&stream,
                            &inputParameters,
                            &outputParameters,
                            SAMPLE_RATE,
                            FRAMES_PER_BUFFER,
                            paClipOff,
                            audioCallback,
                            nullptr);
    }

    if (err != paNoError) {
        std::cerr << "PortAudio error (@open_stream): " << Pa_GetErrorText(err) << std::endl;
        return nullptr;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error (@start_stream): " << Pa_GetErrorText(err) << std::endl;
        return nullptr;
    }
    return stream;
}


void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
    mouseX = (float) xpos;
    mouseY = (float) ypos;
    if (fMouseIsPressed) {
        mouseDragged();
    } else {
        mouseMoved();
    }
    pmouseX = mouseX;
    pmouseY = mouseY;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    mouseButton = button;
    if (action == GLFW_PRESS) {
        fMouseIsPressed = true;
        mousePressed();
    } else if (action == GLFW_RELEASE) {
        fMouseIsPressed = false;
        mouseReleased();
    }
}

void key_callback(GLFWwindow *window, int _key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        key = _key;
        if (key == GLFW_KEY_ESCAPE) {
            fAppIsRunning = false;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else {
            keyPressed();
        }
    }
    if (action == GLFW_RELEASE) {
        key = _key;
        keyReleased();
    }
}

GLFWwindow *init_graphics(int _width, int _height, const char *title) {
    width = _width;
    height = _height;

    if (!glfwInit()) {
        return nullptr;
    }

    /* monitors */
    GLFWmonitor *desiredMonitor = nullptr;
    if (monitor != DEFAULT) {
        int monitorCount;
        GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
        for (int i = 0; i < monitorCount; ++i) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
            std::cout << "+++ monitor " << i << ": " << mode->width << "x" << mode->height << " (" << mode->refreshRate << "Hz)"
                      << std::endl;
        }
        if (monitor < monitorCount) {
            desiredMonitor = monitors[monitor];
        }
    }

    /* anti aliasing */
    if (antialiasing > 0) {
        glfwWindowHint(GLFW_SAMPLES, antialiasing);
    }

    GLFWwindow *window = glfwCreateWindow(width, height, title, desiredMonitor, nullptr);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    return window;
}

void shutdown(PaStream *stream, GLFWwindow *window) {
    std::cout << "+++ shutting down" << std::endl;

    /* terminate PortAudio */
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    /* terminate GLFW */
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetKeyCallback(window, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void handle_setup(GLFWwindow *window) {
    glfwSwapInterval(1); // Enable vsync (1 means on, 0 means off)
    fAppIsInitialized = true;
    setup();
}

void handle_draw(GLFWwindow *window) {
    /* timer */
    static std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now(), endTime;

    // set up your projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(1, -1, 1);
    glTranslatef(0, (float) -height, 0);

    glViewport(0, 0, width, height); // Set the viewport dimensions to match the screen resolution

    draw();

    // swap the front and back buffers
    glfwSwapBuffers(window);
    // poll for and process events
    glfwPollEvents();

    /* timer */
    endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    double frameTime = frameDuration.count();
    frameRate = (float) (1.0 / frameTime);
    frameCount++;
    startTime = std::chrono::high_resolution_clock::now();
}

int run_application() {
    settings();

    PaStream *stream = init_audio(NUMBER_OF_INPUT_CHANNELS, NUMBER_OF_OUTPUT_CHANNELS);
    if (stream == nullptr) {
        return -1;
    }

    GLFWwindow *window = init_graphics(width, height, DEFAULT_WINDOW_TITLE); // @development
    if (window == nullptr) {
        return -1;
    }

    handle_setup(window);
    std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();

    while (fAppIsRunning && !glfwWindowShouldClose(window)) {
        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
                currentTime - lastFrameTime);
        double frameTime = frameDuration.count();
        if (frameTime >= fTargetFrameTime) {
            handle_draw(window);
            lastFrameTime = currentTime;
        }
    }

    shutdown(stream, window);

    return 0;
}

int main() {
    return run_application();
}
