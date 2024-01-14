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

#include <iostream>

#if !defined(DISABLE_GRAPHICS) || !defined(DISABLE_AUDIO)

#include <SDL2/SDL.h>

#endif

#ifndef DISABLE_GRAPHICS

#include <GL/glew.h>

#define APP_WINDOW SDL_Window

#endif

#ifndef DISABLE_AUDIO

#include <chrono>
#include <thread>

#endif

#include "Umgebung.h"

namespace umgebung {
    /* public */

    int  audio_input_device    = DEFAULT_AUDIO_DEVICE;
    int  audio_output_device   = DEFAULT_AUDIO_DEVICE;
    int  audio_input_channels  = DEFAULT_NUMBER_OF_INPUT_CHANNELS;
    int  audio_output_channels = DEFAULT_NUMBER_OF_OUTPUT_CHANNELS;
    int  monitor               = DEFAULT;
    int  antialiasing          = DEFAULT;
    bool resizable             = true;
    bool enable_retina_support = true;
    bool headless              = false;
    bool no_audio              = false;

    /* private */

    static PApplet *        fApplet           = nullptr;
    static bool             fAppIsRunning     = true;
    static constexpr double fTargetFrameTime  = 1.0 / 60.0; // @development make this adjustable
    static bool             fAppIsInitialized = false;
    static bool             fMouseIsPressed   = false;

    static SDL_GLContext     glContext           = nullptr;
    static SDL_AudioDeviceID audio_output_stream = 0;
    static SDL_AudioDeviceID audio_input_stream  = 0;

#ifndef DISABLE_AUDIO

    float *input_buffer    = nullptr;
    bool audio_input_ready = false;

    void audioOutputCallback(void *userdata, Uint8 *stream, int len) {
        const int samples    = len / sizeof(float); // Number of samples to fill
        float *output_buffer = reinterpret_cast<float *>(stream); // (assuming AUDIO_F32 format)
        if (input_buffer == nullptr && audio_input_channels > 0) {
            for (int i = 0; i < samples; ++i) {
                output_buffer[i] = 0;
            }
            return;
        }
        int mIterationGuard = DEFAULT_AUDIO_SAMPLE_RATE / DEFAULT_FRAMES_PER_BUFFER;
        while (!audio_input_ready && mIterationGuard-- > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (mIterationGuard <= 1) {
            return;
        }
        fApplet->audioblock(input_buffer, output_buffer, samples / audio_output_channels);
    }

    void audioInputCallback(void *userdata, Uint8 *stream, int len) {
        audio_input_ready     = false;
        const float *samples  = reinterpret_cast<float *>(stream); // Assuming AUDIO_F32 format
        const int sampleCount = len / sizeof(float);

        if (input_buffer == nullptr) {
            return;
        }

        for (int i = 0; i < sampleCount; ++i) {
            input_buffer[i] = samples[i];
        }
        audio_input_ready = true;
    }

    static int print_audio_devices() {
        std::cout << "Available audio devices:\n";

        const int numInputDevices = SDL_GetNumAudioDevices(SDL_TRUE);
        for (int i = 0; i < numInputDevices; i++) {
            const char *deviceName = SDL_GetAudioDeviceName(i, SDL_TRUE);
            std::cout << "- Input Device  : " << i << " : " << deviceName << std::endl;
        }

        const int numOutputDevices = SDL_GetNumAudioDevices(SDL_FALSE);
        for (int i = 0; i < numOutputDevices; i++) {
            const char *deviceName = SDL_GetAudioDeviceName(i, SDL_FALSE);
            std::cout << "- Output Device : " << i << " : " << deviceName << std::endl;
        }

        return 0;
    }

    static void init_audio(int input_channels, int output_channels) {
        if (audio_input_device != DEFAULT_AUDIO_DEVICE || audio_output_device != DEFAULT_AUDIO_DEVICE) {
            print_audio_devices();
        }
        if (output_channels > 0) {
            SDL_AudioSpec audio_output_spec, audio_output_obtained_spec;
            SDL_zero(audio_output_spec);
            audio_output_spec.freq     = DEFAULT_AUDIO_SAMPLE_RATE; // @TODO make this adjustable
            audio_output_spec.format   = AUDIO_F32; // @TODO make this adjustable
            audio_output_spec.channels = output_channels;
            audio_output_spec.samples  = DEFAULT_FRAMES_PER_BUFFER; // @TODO make this adjustable
            audio_output_spec.callback = audioOutputCallback;
            audio_output_stream        = SDL_OpenAudioDevice(
                audio_output_device == DEFAULT_AUDIO_DEVICE ? nullptr : SDL_GetAudioDeviceName(audio_output_device, 0),
                0,
                &audio_output_spec,
                &audio_output_obtained_spec,
                SDL_AUDIO_ALLOW_FORMAT_CHANGE);
            if (audio_output_stream == 0) {
                std::cerr << "error: failed to open audio output: " << SDL_GetError() << std::endl;
                return;
            }
            SDL_PauseAudioDevice(audio_output_stream, 0);
        }

        if (input_channels > 0) {
            SDL_AudioSpec audio_input_spec, audio_input_obtained_spec;
            SDL_zero(audio_input_spec);
            audio_input_spec.freq     = DEFAULT_AUDIO_SAMPLE_RATE; // @TODO make this adjustable
            audio_input_spec.format   = AUDIO_F32; // @TODO make this adjustable
            audio_input_spec.channels = input_channels;
            audio_input_spec.samples  = DEFAULT_FRAMES_PER_BUFFER; // @TODO make this adjustable
            audio_input_spec.callback = audioInputCallback;
            audio_input_stream        = SDL_OpenAudioDevice(
                audio_input_device == DEFAULT_AUDIO_DEVICE ? nullptr : SDL_GetAudioDeviceName(audio_input_device, 1),
                1,
                &audio_input_spec,
                &audio_input_obtained_spec,
                SDL_AUDIO_ALLOW_FORMAT_CHANGE);
            if (audio_input_stream == 0) {
                std::cerr << "error: failed to open audio input: " << SDL_GetError() << std::endl;
                return;
            }

            input_buffer = new float[DEFAULT_FRAMES_PER_BUFFER * input_channels];
            SDL_PauseAudioDevice(audio_input_stream, 0);
        }

        //            std::cout << "+++ using default audio input and output devices" << std::endl;
        //            err = Pa_OpenDefaultStream(&stream,
        //                                       input_channels,
        //                                       output_channels,
        //                                       paFloat32,
        //                                       DEFAULT_AUDIO_SAMPLE_RATE,
        //                                       DEFAULT_FRAMES_PER_BUFFER,
        //                                       audioCallback,
        //                                       nullptr);
        //        } else {
        //            print_audio_devices();

        //            PaStreamParameters inputParameters, outputParameters;
        //            inputParameters.device                     = audio_input_device;
        //            inputParameters.channelCount               = input_channels;
        //            inputParameters.sampleFormat               = paFloat32;
        //            inputParameters.suggestedLatency           = Pa_GetDeviceInfo(audio_input_device)->defaultLowInputLatency;
        //            inputParameters.hostApiSpecificStreamInfo  = nullptr;
        //            outputParameters.device                    = audio_output_device;
        //            outputParameters.channelCount              = output_channels;
        //            outputParameters.sampleFormat              = paFloat32;
        //            outputParameters.suggestedLatency          = Pa_GetDeviceInfo(audio_output_device)->defaultLowOutputLatency;
        //            outputParameters.hostApiSpecificStreamInfo = nullptr;
        //            err = Pa_OpenStream(&stream,
        //                                &inputParameters,
        //                                &outputParameters,
        //                                DEFAULT_AUDIO_SAMPLE_RATE,
        //                                DEFAULT_FRAMES_PER_BUFFER,
        //                                paClipOff,
        //                                audioCallback,
        //                                nullptr);
        //        }
        //
        //        if (err != paNoError) {
        //            std::cerr << "PortAudio error (@open_stream): " << Pa_GetErrorText(err) << std::endl;
        //            return nullptr;
        //        }
        //
        //        err = Pa_StartStream(stream);
        //        if (err != paNoError) {
        //            std::cerr << "PortAudio error (@start_stream): " << Pa_GetErrorText(err) << std::endl;
        //            return nullptr;
        //        }
        //        return stream;
        //        }
    }

#endif // DISABLE_AUDIO

#ifndef DISABLE_GRAPHICS

    static void set_default_graphics_state() {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    //    static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    //        fApplet->width  = width;
    //        fApplet->height = height;
    //
    //        int framebufferWidth, framebufferHeight;
    //        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    //        fApplet->framebuffer_width  = framebufferWidth;
    //        fApplet->framebuffer_height = framebufferHeight;
    //    }
    //
    //    static void error_callback(int error, const char *description) {
    //        std::cerr << "GLFW Error: " << description << std::endl;
    //    }
    //
    //    static int wndPos[2];
    //    static int wndSize[2];
    //
    //    static void SetFullScreen(GLFWwindow *_wnd, GLFWmonitor *_monitor, bool fullscreen) {
    //        // see https://stackoverflow.com/questions/47402766/switching-between-windowed-and-full-screen-in-opengl-glfw-3-2
    //        if (fullscreen) {
    //            // backup window position and window size
    //            glfwGetWindowPos(_wnd, &wndPos[0], &wndPos[1]);
    //            glfwGetWindowSize(_wnd, &wndSize[0], &wndSize[1]);
    //
    //            // get resolution of monitor
    //            const GLFWvidmode *mode = glfwGetVideoMode(_monitor);
    //
    //            // switch to full screen
    //            glfwSetWindowMonitor(_wnd, _monitor, 0, 0, mode->width, mode->height, 0);
    //        } else {
    //            // restore last window size and position
    //            glfwSetWindowMonitor(_wnd, nullptr, wndPos[0], wndPos[1], wndSize[0], wndSize[1], 0);
    //        }
    //    }

    static APP_WINDOW *init_graphics(int width, int height, const char *title) {
        fApplet->width  = width;
        fApplet->height = height;

        //        /* monitors */
        //        GLFWmonitor *desiredMonitor = nullptr;
        //        if (monitor != DEFAULT) {
        //            int         monitorCount;
        //            GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
        //            for (int    i          = 0; i < monitorCount; ++i) {
        //                const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
        //                std::cout << "+++ monitor " << i << ": " << mode->width << "x" << mode->height << " (" << mode->refreshRate
        //                          << "Hz)"
        //                          << std::endl;
        //            }
        //            if (monitor < monitorCount) {
        //                desiredMonitor = monitors[monitor];
        //            }
        //        }

        /* anti aliasing */
        if (antialiasing > 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
        }

        int mWindowFlags = SDL_WINDOW_OPENGL;
        if (enable_retina_support) {
            mWindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
        }

        //        int major, minor, revision;
        //        glfwGetVersion(&major, &minor, &revision);
        //        std::cout << "+++ OpenGL version: " << major << "." << minor << "." << revision << std::endl;

        //#if USE_CURRENT_OPENGL
        //        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        //        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
        //        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //#endif // USE_CURRENT_OPENGL
        //        glfwWindowHint(GLFW_FOCUSED, true);
        //        glfwWindowHint(GLFW_DECORATED, true);
        //        glfwWindowHint(GLFW_RESIZABLE, resizable);

        APP_WINDOW *window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            fApplet->width,
            fApplet->height,
            SDL_WINDOW_OPENGL
        );

        if (!window) {
            std::cerr << "+++ error: could not create window: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        glContext = SDL_GL_CreateContext(window);
        if (glContext == nullptr) {
            std::cerr << "+++ error: could not create OpenGL context: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            return nullptr;
        }

        int framebufferWidth, framebufferHeight;
        SDL_GL_GetDrawableSize(window, &framebufferWidth, &framebufferHeight);
        if (fApplet->width != framebufferWidth || fApplet->height != framebufferHeight) {
            std::cout << "+++ retina display detected" << std::endl;
        }
        fApplet->framebuffer_width  = framebufferWidth;
        fApplet->framebuffer_height = framebufferHeight;

        set_default_graphics_state();

        /* initialize GLEW */
        glewExperimental            = GL_TRUE;
        const GLenum glewInitResult = glewInit();
        if (GLEW_OK != glewInitResult) {
            std::cerr << "ERROR: " << glewGetErrorString(glewInitResult) << std::endl;
            return nullptr;
        }

        return window;
    }

    //    /* implement scroll_callback */
    //
    //    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    //        // TODO add `void mouseWheel(MouseEvent event) { ... }`
    //        std::cout << "Scroll: " << xoffset << ", " << yoffset << std::endl;
    //    }


#endif // DISABLE_GRAPHICS

#ifndef DISABLE_AUDIO

    static void shutdown() {
        if (!audio_output_stream) {
            SDL_CloseAudioDevice(audio_output_stream);
        }
        if (!audio_input_stream) {
            SDL_CloseAudioDevice(audio_input_stream);
        }
    }

#endif // DISABLE_AUDIO

#ifndef DISABLE_GRAPHICS

    static void shutdown(APP_WINDOW *window) {
        if (!headless) {
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
        }
    }

    static void handle_setup(APP_WINDOW *window) {
        if (!headless) {
#ifndef DISABLE_GRAPHICS
            if (window != nullptr) {
                SDL_GL_SetSwapInterval(1); // Enable vsync (1 means on, 0 means off)
            }
#endif // DISABLE_GRAPHICS
        }
        fAppIsInitialized = true;
        fApplet->init();
        if (!headless) {
            fApplet->pre_draw();
            set_default_graphics_state();
        }
        fApplet->setup();
        if (!headless) {
            fApplet->post_draw();
        }
    }

    static void handle_draw(APP_WINDOW *window) {
        /* timer begin  */
        static std::chrono::high_resolution_clock::time_point
                startTime = std::chrono::high_resolution_clock::now(), endTime;

        if (headless) {
            fApplet->draw();
        } else {
            /* draw */
            fApplet->pre_draw();
            fApplet->draw();
            fApplet->post_draw();

            /* swap front and back buffers */
            SDL_GL_SwapWindow(window);
        }

        /* timer end */
        endTime                                           = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double> >(
            endTime - startTime);
        const double frameTime = frameDuration.count();
        fApplet->frameRate     = static_cast<float>(1.0 / frameTime);
        fApplet->frameCount++;
        startTime = std::chrono::high_resolution_clock::now();
    }

    static void handle_event(const SDL_Event &event) {
        switch (event.type) {
            case SDL_QUIT:
                fAppIsRunning = false;
                break;
            case SDL_KEYDOWN:
                fApplet->key = event.key.keysym.sym;
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    fAppIsRunning = false;
                } else {
                    fApplet->keyPressed();
                }
                break;
            case SDL_KEYUP:
                fApplet->key = event.key.keysym.sym;
                fApplet->keyReleased();
                break;
            case SDL_MOUSEBUTTONDOWN:
                fApplet->mouseButton = event.button.button;
                fMouseIsPressed = true;
                fApplet->mousePressed();
                break;
            case SDL_MOUSEBUTTONUP:
                fMouseIsPressed = false;
                fApplet->mouseReleased();
                break;
            case SDL_MOUSEMOTION:
                fApplet->mouseX = static_cast<float>(event.motion.x);
                fApplet->mouseY = static_cast<float>(event.motion.y);
                if (fMouseIsPressed) {
                    fApplet->mouseDragged();
                } else {
                    fApplet->mouseMoved();
                }
                fApplet->pmouseX = fApplet->mouseX;
                fApplet->pmouseY = fApplet->mouseY;
                break;
            case SDL_DROPFILE: {
                char *dropped_filedir = event.drop.file;
                fApplet->dropped(dropped_filedir);
                SDL_free(dropped_filedir);
                break;
            }
            default: break;
        }
    }

#else // DISABLE_GRAPHICS

    static void handle_draw() {
        /* timer begin  */
        static std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now(), endTime;

        fApplet->draw();

        /* timer end */
        endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
                endTime - startTime);
        double frameTime = frameDuration.count();
        fApplet->frameRate = (float) (1.0 / frameTime);
        fApplet->frameCount++;
        startTime = std::chrono::high_resolution_clock::now();
    }

#endif // DISABLE_GRAPHICS

    static int run_application() {
        std::cout << "+++ current working directory: " << sketchPath() << std::endl;

#if !defined(DISABLE_GRAPHICS) && !defined(DISABLE_AUDIO)
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            std::cerr << "error: unable to initialize SDL: " << SDL_GetError() << std::endl;
            return 1;
        }
#elif !defined(DISABLE_GRAPHICS)
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "error: unable to initialize SDL: " << SDL_GetError() << std::endl;
            return 1;
        }
#elif !defined(DISABLE_AUDIO)
            if (SDL_Init(SDL_INIT_AUDIO) != 0) {
                std::cerr << "error: unable to initialize SDL: " << SDL_GetError() << std::endl;
                return 1;
            }
#endif

        fApplet = instance();
        if (fApplet == nullptr) {
            std::cerr << "+++ error: no instance created make sure to include\n"
                    << "\n"
                    << "    PApplet *umgebung::instance() {\n"
                    << "        return new ApplicationInstance()\n"
                    << "    }\n"
                    << "\n"
                    << "+++ in application file,"
                    << std::endl;
            return -1;
        }

        fApplet->settings();

#ifndef DISABLE_GRAPHICS

        APP_WINDOW *window;
        if (headless) {
            window = nullptr;
            std::cout << "+++ running headless application" << std::endl;
        } else {
            window = init_graphics(fApplet->width, fApplet->height, UMGEBUNG_WINDOW_TITLE); // @development
            if (window == nullptr) {
                return -1;
            }
        }

#endif // DISABLE_GRAPHICS

#ifndef DISABLE_AUDIO
        if (no_audio) {
            std::cout << "+++ running application with no audio" << std::endl;
        } else {
            init_audio(audio_input_channels, audio_output_channels);
            if (!audio_output_stream && audio_output_channels > 0) {
                std::cerr << "+++ error: no audio output stream" << std::endl;
                return -1;
            }
            if (!audio_input_stream && audio_input_channels > 0) {
                std::cerr << "+++ error: no audio input stream" << std::endl;
                return -1;
            }
        }
#endif // DISABLE_AUDIO

#ifndef DISABLE_GRAPHICS
        handle_setup(window);

        /* loop */
        std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
        while (fAppIsRunning) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                handle_event(e);
            }
            std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double> >(
                currentTime - lastFrameTime);
            double frameTime = frameDuration.count();
            if (frameTime >= fTargetFrameTime) {
                handle_draw(window);
                lastFrameTime = currentTime;
            }
        }

        fApplet->finish();
        shutdown(window);
#else
        fAppIsInitialized = true;
        fApplet->setup();

        /* loop */
        std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
        while (fAppIsRunning) {
            std::chrono::high_resolution_clock::time_point currentTime   = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double>                  frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
                    currentTime - lastFrameTime);
            double                                         frameTime     = frameDuration.count();
            if (frameTime >= fTargetFrameTime) {
                handle_draw();
                lastFrameTime = currentTime;
            }
        }

        fApplet->finish();
#endif // DISABLE_GRAPHICS

#ifndef DISABLE_AUDIO
        shutdown();
#endif // DISABLE_AUDIO

#if defined(DISABLE_GRAPHICS) || defined(DISABLE_AUDIO)
        SDL_Quit();
#endif

        return 0;
    }

    void exit() {
        fAppIsRunning = false;
    }
} // namespace umgebung

int main(int argc, char *argv[]) {
    // `int main() {` this signature fails on windows
    return umgebung::run_application();
}
