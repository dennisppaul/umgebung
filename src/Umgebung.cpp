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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <portaudio.h>
#include <filesystem>

#include "Umgebung.h"

/* public */

int audio_input_device = DEFAULT_AUDIO_DEVICE;
int audio_output_device = DEFAULT_AUDIO_DEVICE;
int audio_input_channels = DEFAULT_NUMBER_OF_INPUT_CHANNELS;
int audio_output_channels = DEFAULT_NUMBER_OF_OUTPUT_CHANNELS;
int monitor = DEFAULT;
int antialiasing = DEFAULT;
bool resizable = true;
bool enable_retina_support = true;

/* private */

static PApplet *fApplet = nullptr;
static bool fAppIsRunning = true;
static const double fTargetFrameTime = 1.0 / 60.0; // @development make this adjustable
static bool fAppIsInitialized = false;
static bool fMouseIsPressed = false;

static void release_HID_callbacks(GLFWwindow *window);

static void setup_HID_callbacks(GLFWwindow *window);

static int audioCallback(const void *inputBuffer,
                         void *outputBuffer,
                         unsigned long audioFrameCount,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData) {
    auto *out = (float *) outputBuffer;
    auto *in = (const float *) inputBuffer;
    fApplet->audioblock(in, out, audioFrameCount);
    return paContinue;
}

static int print_audio_devices() {
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

static PaStream *init_audio(int input_channels, int output_channels) {
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
                                   AUDIO_SAMPLE_RATE,
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
                            AUDIO_SAMPLE_RATE,
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


static void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
    fApplet->mouseX = (float) xpos;
    fApplet->mouseY = (float) ypos;
    if (fMouseIsPressed) {
        fApplet->mouseDragged();
    } else {
        fApplet->mouseMoved();
    }
    fApplet->pmouseX = fApplet->mouseX;
    fApplet->pmouseY = fApplet->mouseY;
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    fApplet->mouseButton = button;
    if (action == GLFW_PRESS) {
        fMouseIsPressed = true;
        fApplet->mousePressed();
    } else if (action == GLFW_RELEASE) {
        fMouseIsPressed = false;
        fApplet->mouseReleased();
    }
}

static void key_callback(GLFWwindow *window, int _key, int scancode, int action, int mods) {
    // @TODO key is not set properly
    if (action == GLFW_PRESS) {
        fApplet->key = _key;
        if (fApplet->key == GLFW_KEY_ESCAPE) {
            fAppIsRunning = false;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else {
            fApplet->keyPressed();
        }
    }
    if (action == GLFW_RELEASE) {
        fApplet->key = _key;
        fApplet->keyReleased();
    }
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
//    fApplet->key = static_cast<char>(codepoint);
//    std::cout << "Character entered: " << static_cast<char>(codepoint) << " (Unicode: " << codepoint << ")" << std::endl;
}

static void set_default_graphics_state() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    fApplet->width = width;
    fApplet->height = height;

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    fApplet->framebuffer_width = framebufferWidth;
    fApplet->framebuffer_height = framebufferHeight;
}

static void error_callback(int error, const char *description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

static int _wndPos[2];
static int _wndSize[2];

static void SetFullScreen(GLFWwindow *_wnd, GLFWmonitor *_monitor, bool fullscreen) {
    // see https://stackoverflow.com/questions/47402766/switching-between-windowed-and-full-screen-in-opengl-glfw-3-2
    if (fullscreen) {
        // backup window position and window size
        glfwGetWindowPos(_wnd, &_wndPos[0], &_wndPos[1]);
        glfwGetWindowSize(_wnd, &_wndSize[0], &_wndSize[1]);

        // get resolution of monitor
        const GLFWvidmode *mode = glfwGetVideoMode(_monitor);

        // switch to full screen
        glfwSetWindowMonitor(_wnd, _monitor, 0, 0, mode->width, mode->height, 0);
    } else {
        // restore last window size and position
        glfwSetWindowMonitor(_wnd, nullptr, _wndPos[0], _wndPos[1], _wndSize[0], _wndSize[1], 0);
    }
}

static GLFWwindow *init_graphics(int width, int height, const char *title) {
    glfwSetErrorCallback(error_callback);

    fApplet->width = width;
    fApplet->height = height;

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
            std::cout << "+++ monitor " << i << ": " << mode->width << "x" << mode->height << " (" << mode->refreshRate
                      << "Hz)"
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

    if (enable_retina_support) {
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    }

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::cout << "+++ OpenGL version: " << major << "." << minor << "." << revision << std::endl;

//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_FOCUSED, true);
    glfwWindowHint(GLFW_DECORATED, true);
    glfwWindowHint(GLFW_RESIZABLE, resizable);

    GLFWwindow *window = glfwCreateWindow(fApplet->width, fApplet->height, title, desiredMonitor, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "+++ error: could not create window" << std::endl;
        return nullptr;
    }

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    if (fApplet->width != framebufferWidth || fApplet->height != framebufferHeight) {
        std::cout << "+++ retina display detected" << std::endl;
    }
    fApplet->framebuffer_width = framebufferWidth;
    fApplet->framebuffer_height = framebufferHeight;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    /* setup callbacks */
    setup_HID_callbacks(window);

    set_default_graphics_state();

    /* initialize GLEW */
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return nullptr;
    }

    return window;
}

/* implement scroll_callback */

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    // TODO add `void mouseWheel(MouseEvent event) { ... }`
    std::cout << "Scroll: " << xoffset << ", " << yoffset << std::endl;
}

static void setup_HID_callbacks(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, character_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

static void release_HID_callbacks(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetKeyCallback(window, nullptr);
    glfwSetCharCallback(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
}

static void shutdown(PaStream *stream, GLFWwindow *window) {
    /* terminate PortAudio */
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    /* terminate GLFW */

    /* release callbacks */
    release_HID_callbacks(window);

    glfwDestroyWindow(window);
    glfwTerminate();
}

static void handle_setup(GLFWwindow *window) {
    glfwSwapInterval(1); // Enable vsync (1 means on, 0 means off)
    fAppIsInitialized = true;
    fApplet->setup();
}

static void handle_draw(GLFWwindow *window) {
    /* timer begin  */
    static std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now(), endTime;

    /* draw */
    fApplet->pre_draw();
    fApplet->draw();
    fApplet->post_draw();

    /* swap front and back buffers */
    glfwSwapBuffers(window);

    /* poll events */
    glfwPollEvents();

    /* timer end */
    endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
            endTime - startTime);
    double frameTime = frameDuration.count();
    fApplet->frameRate = (float) (1.0 / frameTime);
    fApplet->frameCount++;
    startTime = std::chrono::high_resolution_clock::now();
}

static void print_sketchpath() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current working directory: " << currentPath << std::endl;
}

static int run_application() {
    print_sketchpath();

    fApplet = instance();
    if (fApplet == nullptr) {
        std::cerr << "+++ error: no instance created make sure to include\n"
                  << "\n"
                  << "    PApplet *instance() {\n"
                  << "        return new ApplicationInstance()\n"
                  << "    }\n"
                  << "\n"
                  << "+++ in application file,"
                  << std::endl;
        return -1;
    }

    fApplet->settings();

    GLFWwindow *window = init_graphics(fApplet->width, fApplet->height, UMGEBUNG_WINDOW_TITLE); // @development
    if (window == nullptr) {
        return -1;
    }

    PaStream *stream = init_audio(audio_input_channels, audio_output_channels);
    if (stream == nullptr) {
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

    fApplet->finish();
    shutdown(stream, window);

    return 0;
}

void exit() {
    fAppIsRunning = false;
}

int main() {
    return run_application();
}
