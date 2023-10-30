#include <GLFW/glfw3.h>

#include <iostream>
#include <portaudio.h>

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 2048

void setup() {
}

void draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glEnd();
}

void audioblock(float *input, float *output, unsigned long length) {
    // NOTE length is the number of samples per channel
    const float TWO_PI = 6.283185307179586;
    static float phase = 0.0;
    float frequency = 440.0;
    float amplitude = 0.5;

    for (int i = 0; i < length; i++) {
        float sample = amplitude * sinf(phase);
        phase += (TWO_PI * frequency) / SAMPLE_RATE;

        if (phase >= TWO_PI) {
            phase -= TWO_PI;
        }

        output[i * 2 + 0] = sample;
        output[i * 2 + 1] = sample;
    }
}

/* ------------------------------------------------------------------------------------------- */

bool fAppIsRunning = true;

int audioCallback(const void *inputBuffer,
                  void *outputBuffer,
                  unsigned long frameCount,
                  const PaStreamCallbackTimeInfo *timeInfo,
                  PaStreamCallbackFlags statusFlags,
                  void *userData) {
    auto *out = (float *) outputBuffer;
    audioblock(nullptr, out, frameCount);
    return paContinue;
}

PaStream *init_audio() {
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error (@init): " << Pa_GetErrorText(err) << std::endl;
        return nullptr;
    }

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, audioCallback, NULL);
    if (err != paNoError) {
        std::cerr << "PortAudio error (@stream): " << Pa_GetErrorText(err) << std::endl;
        return nullptr;
    }

    Pa_StartStream(stream);
    return stream;
}


void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
    std::cout << "mouse: " << xpos << ", " << ypos << std::endl;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    std::cout << "mouse: " << button << ", " << action << ", " << mods << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "key  : " << key << std::endl;
        if (key == GLFW_KEY_ESCAPE) {
            fAppIsRunning = false;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

GLFWwindow *init_graphics(int width, int height, const char *title) {
    if (!glfwInit()) {
        return nullptr;
    }

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
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

int main() {
    PaStream *stream = init_audio();
    if (stream == nullptr) {
        return -1;
    }

    GLFWwindow *window = init_graphics(800, 600, "Umgebung");
    if (window == nullptr) {
        return -1;
    }

    setup();
    while (fAppIsRunning && !glfwWindowShouldClose(window)) {
        draw();
        // swap the front and back buffers
        glfwSwapBuffers(window);
        // poll for and process events
        glfwPollEvents();
    }

    shutdown(stream, window);

    return 0;
}
