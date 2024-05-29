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

    static PApplet*         fApplet           = nullptr;
    static bool             fAppIsRunning     = true;
    static constexpr double fTargetFrameTime  = 1.0 / 60.0; // @development make this adjustable
    static bool             fAppIsInitialized = false;
    static bool             fMouseIsPressed   = false;

    static SDL_AudioDeviceID audio_output_stream = 0;
    static SDL_AudioDeviceID audio_input_stream  = 0;

#ifndef DISABLE_AUDIO

    float* input_buffer          = nullptr;
    bool   audio_input_ready     = false;
    bool   audio_was_initialized = false;

#define FIXED_MEMORY_ALLOCATION
#ifdef FIXED_MEMORY_ALLOCATION

#define MAX_CHANNELS 4                       // TODO make this configurable
#define MAX_FRAMES DEFAULT_FRAMES_PER_BUFFER // TODO make this configurable

    void audioOutputCallback(void* userdata, Uint8* stream, int len) {
        const int samples       = len / sizeof(float);              // Number of samples to fill
        float*    output_buffer = reinterpret_cast<float*>(stream); // Assuming AUDIO_F32 format

        if (input_buffer == nullptr && audio_input_channels > 0) {
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }
        if (!fAppIsInitialized) {
            /* wait with callback until after `setup()` */
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }
        int mIterationGuard = DEFAULT_AUDIO_SAMPLE_RATE / DEFAULT_FRAMES_PER_BUFFER;
        while (!audio_input_ready && mIterationGuard-- > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (mIterationGuard <= 1) {
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }

        const int frames = (audio_output_channels == 0) ? (samples / audio_input_channels) : (samples / audio_output_channels);

        // Ensure that frames and channels do not exceed maximum allowed values
        if (audio_input_channels > MAX_CHANNELS || audio_output_channels > MAX_CHANNELS || frames > MAX_FRAMES) {
            std::cerr << "Error: Exceeded maximum channel or frame count." << std::endl;
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }

        // Statically allocated arrays for deinterleaving and interleaving
        float input_channels[MAX_CHANNELS][MAX_FRAMES];
        float output_channels[MAX_CHANNELS][MAX_FRAMES];

        // Deinterleave input buffer into separate channels
        if (input_buffer != nullptr) {
            for (int frame = 0; frame < frames; ++frame) {
                for (int ch = 0; ch < audio_input_channels; ++ch) {
                    input_channels[ch][frame] = input_buffer[frame * audio_input_channels + ch];
                }
            }
        }

        // Process the deinterleaved audio
        float* input_ptrs[MAX_CHANNELS];
        float* output_ptrs[MAX_CHANNELS];
        for (int ch = 0; ch < audio_input_channels; ++ch) {
            input_ptrs[ch] = input_channels[ch];
        }
        for (int ch = 0; ch < audio_output_channels; ++ch) {
            output_ptrs[ch] = output_channels[ch];
        }

        fApplet->audioblock(input_ptrs, output_ptrs, frames);

        // Interleave the processed output channels into the output buffer
        for (int frame = 0; frame < frames; ++frame) {
            for (int ch = 0; ch < audio_output_channels; ++ch) {
                output_buffer[frame * audio_output_channels + ch] = output_channels[ch][frame];
            }
        }

        // Apply fade-in if audio was not initialized before
        if (!audio_was_initialized) {
            audio_was_initialized = true;
            for (int i = 0; i < samples; ++i) {
                output_buffer[i] *= static_cast<float>(i) / samples;
            }
        }
    }

#else
    void audioOutputCallback(void* userdata, Uint8* stream, int len) {
        const int samples       = len / sizeof(float);              // Number of samples to fill
        float*    output_buffer = reinterpret_cast<float*>(stream); // (assuming AUDIO_F32 format)
        if (input_buffer == nullptr && audio_input_channels > 0) {
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }
        if (!fAppIsInitialized) {
            /* wait with callback until after `setup()` */
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }
        int mIterationGuard = DEFAULT_AUDIO_SAMPLE_RATE / DEFAULT_FRAMES_PER_BUFFER;
        while (!audio_input_ready && mIterationGuard-- > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (mIterationGuard <= 1) {
            std::fill(output_buffer, output_buffer + samples, 0);
            return;
        }

        const int frames = (audio_output_channels == 0) ? (samples / audio_input_channels) : (samples / audio_output_channels);

        // Dynamically allocate memory for deinterleaved channels
        float** input_channels  = new float*[audio_input_channels];
        float** output_channels = new float*[audio_output_channels];

        for (int ch = 0; ch < audio_input_channels; ++ch) {
            input_channels[ch] = new float[frames];
        }

        for (int ch = 0; ch < audio_output_channels; ++ch) {
            output_channels[ch] = new float[frames];
        }

        // Deinterleave input buffer into separate channels
        if (input_buffer != nullptr) {
            for (int frame = 0; frame < frames; ++frame) {
                for (int ch = 0; ch < audio_input_channels; ++ch) {
                    input_channels[ch][frame] = input_buffer[frame * audio_input_channels + ch];
                }
            }
        }

        // Process the deinterleaved audio
        fApplet->audioblock(input_channels, output_channels, frames);

        // Interleave the processed output channels into the output buffer
        for (int frame = 0; frame < frames; ++frame) {
            for (int ch = 0; ch < audio_output_channels; ++ch) {
                output_buffer[frame * audio_output_channels + ch] = output_channels[ch][frame];
            }
        }

        // Apply fade-in if audio was not initialized before
        if (!audio_was_initialized) {
            audio_was_initialized = true;
            for (int i = 0; i < samples; ++i) {
                output_buffer[i] *= static_cast<float>(i) / samples;
            }
        }

        // Deallocate memory
        for (int ch = 0; ch < audio_input_channels; ++ch) {
            delete[] input_channels[ch];
        }
        for (int ch = 0; ch < audio_output_channels; ++ch) {
            delete[] output_channels[ch];
        }
        delete[] input_channels;
        delete[] output_channels;
    }
#endif

    void audioInputCallback(void* userdata, Uint8* stream, int len) {
        audio_input_ready        = false;
        const float* samples     = reinterpret_cast<float*>(stream); // Assuming AUDIO_F32 format
        const int    sampleCount = len / sizeof(float);

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
            const char* deviceName = SDL_GetAudioDeviceName(i, SDL_TRUE);
            std::cout << "- Input Device  : " << i << " : " << deviceName << std::endl;
        }

        const int numOutputDevices = SDL_GetNumAudioDevices(SDL_FALSE);
        for (int i = 0; i < numOutputDevices; i++) {
            const char* deviceName = SDL_GetAudioDeviceName(i, SDL_FALSE);
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
            audio_output_spec.format   = AUDIO_F32;                 // @TODO make this adjustable
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
            audio_input_spec.format   = AUDIO_F32;                 // @TODO make this adjustable
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

    static void shutdown() {
        if (!audio_output_stream) {
            SDL_CloseAudioDevice(audio_output_stream);
        }
        if (!audio_input_stream) {
            SDL_CloseAudioDevice(audio_input_stream);
        }
    }

#endif // DISABLE_AUDIO

    static int run_application(std::vector<std::string> args) {
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

        set_graphics_context(fApplet);
        fApplet->arguments(args);
        fApplet->settings();

#ifndef DISABLE_GRAPHICS

        APP_WINDOW* window;
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
        fAppIsInitialized = true;

        /* loop */
        std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
        while (fAppIsRunning) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                handle_event(e, fAppIsRunning, fMouseIsPressed);
            }
            std::chrono::high_resolution_clock::time_point currentTime   = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double>                  frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
                currentTime - lastFrameTime);
            double frameTime = frameDuration.count();
            if (frameTime >= fTargetFrameTime) {
                handle_draw(window);
                lastFrameTime = currentTime;
            }
        }

        fApplet->finish();
        handle_shutdown(window);
#else
        fAppIsInitialized = true;
        fApplet->setup();

        /* loop */
        std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
        while (fAppIsRunning) {
            std::chrono::high_resolution_clock::time_point currentTime   = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double>                  frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
                currentTime - lastFrameTime);
            double frameTime = frameDuration.count();
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

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }
    }
    return umgebung::run_application(args);
}
