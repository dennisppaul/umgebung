/*
* Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2025 Dennis P Paul.
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

#ifdef ENABLE_PORTAUDIO

#include <iostream>
#include <portaudio.h>
#include <chrono>
#include <thread>

#include "Umgebung.h"
#include "UmgebungSubsystems.h"
#include "UmgebungFunctionsAdditional.h"
#include "PAudio.h"

namespace umgebung {

    class PAudioPortAudio {
    public:
        PAudio* audio;

        // #define SAMPLE_RATE       44100
        // #define FRAMES_PER_BUFFER 512
        // #define NUM_CHANNELS      2 // Stereo input/output

        PaStream* stream;

        explicit PAudioPortAudio(PAudio* audio) : audio(audio),
                                                  update_interval((audio->buffer_size * 1000) / audio->sample_rate) {
            this->audio = audio;
            if (!init()) {
                error("PAudioPortAudio: could not intialize");
            }
            if (audio->input_channels > 0) {
                audio->input_buffer = new float[audio->buffer_size * audio->input_channels]{0};
            } else {
                audio->input_buffer = nullptr; // TODO maybe do this in sdl subsytem as well
            }
            if (audio->output_channels > 0) {
                audio->output_buffer = new float[audio->buffer_size * audio->output_channels]{0};
            } else {
                audio->output_buffer = nullptr; // TODO maybe do this in sdl subsytem as well
            }
        }

        void start() {
            isPaused = false;
            if (Pa_IsStreamStopped(stream)) {
                Pa_StartStream(stream);
            }
        }

        void stop() {
            isPaused = true;
            if (Pa_IsStreamActive(stream)) {
                Pa_StopStream(stream);
            }
        }

        void loop() {
            if (audio == nullptr) {
                return;
            }

            if (isPaused) {
                return;
            }

            const auto now = std::chrono::high_resolution_clock::now();
            if (now - last_audio_update >= update_interval) {
                // TODO PROBLEM if either of the devices is not present i.e channel count is 0
                //      the application crashes or is not updated. need to move audioEvent().
                // check if input is available
                const long availableInputFrames = Pa_GetStreamReadAvailable(stream);
                if (availableInputFrames >= audio->buffer_size) {
                    const PaError err = Pa_ReadStream(stream, audio->input_buffer, audio->buffer_size);
                    if (err != paNoError) {
                        error("Error reading from stream: ", Pa_GetErrorText(err));
                        return;
                    }
                }

                // check if output buffer has space
                const long availableOutputFrames = Pa_GetStreamWriteAvailable(stream);

                // call audioevent resepcting non-present audio devices and available frames
                if ((availableInputFrames >= audio->buffer_size || audio->input_channels == 0) &&
                    (availableOutputFrames >= audio->buffer_size || audio->output_channels == 0)) {
                    // TODO request audio block from PAudio
                    if (a != nullptr && audio == umgebung::a) {
                        audioEvent();
                    }
                    audioEvent(*audio);
                }

                if (availableOutputFrames >= audio->buffer_size) {
                    const PaError err = Pa_WriteStream(stream, audio->output_buffer, audio->buffer_size);
                    if (err != paNoError) {
                        error("Error writing to stream: ", Pa_GetErrorText(err), "");
                        return;
                    }
                }

                last_audio_update = now;
            }
        }

        void shutdown() const {
            Pa_StopStream(stream);
            Pa_CloseStream(stream);
            delete[] audio->input_buffer;
            delete[] audio->output_buffer;
            audio->input_buffer  = nullptr;
            audio->output_buffer = nullptr;
        }

    private:
        bool                                                        isPaused = false;
        std::chrono::milliseconds                                   update_interval;
        std::chrono::time_point<std::chrono::high_resolution_clock> last_audio_update;

        bool init() {
            warning("currently only default devices are supported");
            int audio_input_device  = AUDIO_DEVICE_DEFAULT;
            int audio_output_device = AUDIO_DEVICE_DEFAULT;

            if (audio_input_device == AUDIO_DEVICE_DEFAULT) {
                audio_input_device = Pa_GetDefaultInputDevice();
                console("Using default input device with ID : ", audio_input_device);
            }
            if (audio_output_device == AUDIO_DEVICE_DEFAULT) {
                audio_output_device = Pa_GetDefaultOutputDevice();
                console("Using default output device with ID: ", audio_output_device);
            }

            console("Opening audio device (input/output): (",
                    audio_input_device, "/",
                    audio_output_device, ")");

            const PaDeviceInfo*  deviceInfo  = Pa_GetDeviceInfo(audio_output_device);
            const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

            console("Opening stream for device with ID: ", deviceInfo->name,
                    "( Host API: ", hostApiInfo->name,
                    ", Channels (input/output): (", input_channels,
                    "/", output_channels, ")",
                    " ) ... ");

            /* input */

            PaStreamParameters inputParams;
            if (audio->input_channels > 0) {
                inputParams.device = Pa_GetDefaultInputDevice();
                if (inputParams.device == paNoDevice) {
                    error("No default input device found.");
                    return false;
                }
                inputParams.channelCount              = audio->input_channels;
                inputParams.sampleFormat              = paFloat32;
                inputParams.suggestedLatency          = Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
                inputParams.hostApiSpecificStreamInfo = nullptr;
            }

            /* output */

            PaStreamParameters outputParams;
            if (audio->output_channels > 0) {
                outputParams.device = Pa_GetDefaultOutputDevice();
                if (outputParams.device == paNoDevice) {
                    error("No default output device found.");
                    return false;
                }
                outputParams.channelCount              = audio->output_channels;
                outputParams.sampleFormat              = paFloat32;
                outputParams.suggestedLatency          = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
                outputParams.hostApiSpecificStreamInfo = nullptr;
            }

            // --- Open Duplex Stream (input + output) ---

            const PaError err = Pa_OpenStream(
                &stream,
                audio->input_channels > 0 ? &inputParams : nullptr,
                audio->output_channels > 0 ? &outputParams : nullptr,
                audio->sample_rate,
                audio->buffer_size,
                paClipOff, // No clipping
                nullptr,   // No callback (blocking mode)
                nullptr    // No user data
            );

            if (err != paNoError) {
                error("audio->input_channels : ", audio->input_channels);
                error("audio->output_channels: ", audio->output_channels);
                error("Failed to open stream: ", Pa_GetErrorText(err), "");
                return false;
            }

            Pa_StartStream(stream);
            last_audio_update = std::chrono::high_resolution_clock::now();

            return true;
        }
    };

    // TODO move some of this code to PAudioPortAudio
    /*
  static void init_audio(int           input_channels,
                           int           output_channels,
                           double        audio_sample_rate,
                           unsigned long audio_samples_per_frame) {
        PaError err;

        if (audio_input_device == AUDIO_DEVICE_DEFAULT && audio_output_device == AUDIO_DEVICE_DEFAULT) {
            console("Opening default audio device.");
            err = Pa_OpenDefaultStream(&stream,
                                       input_channels,
                                       output_channels,
                                       paFloat32,
                                       audio_sample_rate,
                                       audio_samples_per_frame,
                                       audioCallback,
                                       nullptr);
        } else {
            print_audio_devices();

            if (audio_input_device < 0) {
                audio_input_device = Pa_GetDefaultInputDevice();
                console("Using default input device with ID: ",audio_input_device);
            }
            if (audio_output_device < 0) {
                audio_output_device = Pa_GetDefaultOutputDevice();
                console("Using default output device with ID: ",audio_output_device);
            }

            console("Opening audio device (input/output): (",audio_input_device, "/",audio_output_device, ")");
            const PaDeviceInfo*  deviceInfo  = Pa_GetDeviceInfo(audio_output_device);
            const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

            console("Opening stream for device with ID: ",deviceInfo->name;
            console("( Host API: ",hostApiInfo->name;
            console(", Channels (input/output): (",input_channels, "/",output_channels, ")";
            console(" ) ... ";

            PaStreamParameters inputParameters;
            inputParameters.device                    = audio_input_device;
            inputParameters.channelCount              = input_channels;
            inputParameters.sampleFormat              = paFloat32;
            inputParameters.suggestedLatency          = Pa_GetDeviceInfo(audio_input_device)->defaultLowInputLatency;
            inputParameters.hostApiSpecificStreamInfo = nullptr;

            PaStreamParameters outputParameters;
            outputParameters.device                    = audio_output_device;
            outputParameters.channelCount              = output_channels;
            outputParameters.sampleFormat              = paFloat32;
            outputParameters.suggestedLatency          = Pa_GetDeviceInfo(audio_output_device)->defaultLowOutputLatency;
            outputParameters.hostApiSpecificStreamInfo = nullptr;

            err = Pa_OpenStream(&stream,
                                &inputParameters,
                                &outputParameters,
                                audio_sample_rate,
                                audio_samples_per_frame,
                                paClipOff,
                                nullptr,
                                //                                audioCallback,
                                nullptr);
            console("OK.");
        }

        if (err != paNoError) {
            error("+++ error when opening stream: ",Pa_GetErrorText(err));
            return;
        }

        err = Pa_StartStream(stream);
        if (err != paNoError) {
            error("++ error when starting stream: ",Pa_GetErrorText(err));
            return;
        }
    }
 */

    static std::vector<PAudioPortAudio*> audio_devices;

    static void setup_post() {}
    static void draw_pre() {}
    static void draw_post() {}
    static void event(SDL_Event* event) {}

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_AUDIO;
    }

    // static PaStream* stream;
    // static int audioCallback(const void* inputBuffer, void* outputBuffer,
    //                          unsigned long                   framesPerBuffer,
    //                          const PaStreamCallbackTimeInfo* timeInfo,
    //                          PaStreamCallbackFlags           statusFlags,
    //                          void*                           userData) {
    //     (void) timeInfo;
    //     (void) statusFlags;
    //     (void) userData;
    //     // assuming the sample format is paFloat32
    //     auto in  = static_cast<const float*>(inputBuffer);
    //     auto out = static_cast<float*>(outputBuffer);
    //     //
    //     // // create 2D arrays on stack for input and output
    //     // std::vector<std::vector<float>> inputArray(audio_input_channels, std::vector<float>(framesPerBuffer));
    //     // std::vector<std::vector<float>> outputArray(audio_output_channels, std::vector<float>(framesPerBuffer));
    //     //
    //     // // pointers array to pass to processing function
    //     // std::vector<float*> inputPtrs(audio_input_channels);
    //     // std::vector<float*> outputPtrs(audio_output_channels);
    //     //
    //     // // fill input arrays from input buffer and setup pointers
    //     // for (unsigned int ch = 0; ch < audio_input_channels; ++ch) {
    //     //     inputPtrs[ch] = inputArray[ch].data();
    //     //     for (unsigned long i = 0; i < framesPerBuffer; ++i) {
    //     //         if (audio_input_channels > 0) {
    //     //             inputArray[ch][i] = in[i * audio_input_channels + ch];
    //     //         }
    //     //     }
    //     // }
    //     //
    //     // // setup output pointers
    //     // for (unsigned int ch = 0; ch < audio_output_channels; ++ch) {
    //     //     outputPtrs[ch] = outputArray[ch].data();
    //     // }
    //     //
    //     // // process audio block
    //     // if (fApplet != nullptr) {
    //     //     fApplet->audioblock(inputPtrs.data(), outputPtrs.data(), static_cast<int>(framesPerBuffer));
    //     // }
    //     //
    //     // // write processed output to output buffer
    //     // for (unsigned int ch = 0; ch < audio_output_channels; ++ch) {
    //     //     for (unsigned long i = 0; i < framesPerBuffer; ++i) {
    //     //         out[i * audio_output_channels + ch] = outputArray[ch][i];
    //     //     }
    //     // }
    //     return paContinue;
    // }
    // static void shutdown_audio() {
    //     PaError err;
    //
    //     err = Pa_StopStream(stream);
    //     if (err != paNoError) {
    //         error("+++ error when stopping stream: ",Pa_GetErrorText(err));
    //         return;
    //     }
    //
    //     err = Pa_CloseStream(stream);
    //     if (err != paNoError) {
    //         error("+++ error when closing stream: ",Pa_GetErrorText(err));
    //         return;
    //     }
    //
    //     Pa_Terminate();
    // }

    int print_audio_devices() {
        const int numDevices = Pa_GetDeviceCount();
        if (numDevices < 0) {
            error("+++ error 'Pa_CountDevices' returned ", numDevices);
            error("+++ error when counting devices: ", Pa_GetErrorText(numDevices));
            return -1;
        }

        console("Found ", numDevices, " audio devices:");
        for (int i = 0; i < numDevices; i++) {
            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            console("Device ", i, ": ", deviceInfo->name,
                    "  Max input channels: ", deviceInfo->maxInputChannels,
                    "  Max output channels: ", deviceInfo->maxOutputChannels,
                    "  Default sample rate: ", deviceInfo->defaultSampleRate);

            const PaHostApiInfo* hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            if (hostInfo) {
                console("  Host API: ", hostInfo->name);
            }
        }
        console("---");
        return 0;
    }

    static bool init() {
        console("initializing PortAudio audio system");
        const PaError err = Pa_Initialize();
        if (err != paNoError) {
            error("Error message: ", Pa_GetErrorText(err));
            return false;
        }

        print_audio_devices();

        return true;
    }

    static void loop() {
        for (const auto device: audio_devices) {
            if (device != nullptr) {
                device->loop();
            }
        }
    }

    static void shutdown() {
        for (const auto device: audio_devices) {
            if (device != nullptr) {
                device->shutdown();
                // TODO again … who cleans up the buffers in PAudio i.e `ad->audio`? align with SDL
                delete device;
            }
        }
        Pa_Terminate();
    }

    static PAudioPortAudio* find_device(const PAudio* device) {
        for (const auto d: audio_devices) {
            if (d->audio == device) {
                return d;
            }
        }
        return nullptr;
    }

    static void start(PAudio* device) {
        PAudioPortAudio* _audio = find_device(device);
        if (_audio != nullptr) {
            _audio->start();
        }
    }

    static void stop(PAudio* device) {
        PAudioPortAudio* _audio = find_device(device);
        if (_audio != nullptr) {
            _audio->stop();
        }
    }

    static void setup_pre() {
        for (const auto _device: audio_devices) {
            if (_device != nullptr) {
                _device->start();
            }
        }
    }

    static PAudio* create_audio(const AudioUnitInfo* device_info) {
        const auto _device = new PAudio{device_info};
        _device->unique_id = audio_unique_device_id++;
        const auto _audio  = new PAudioPortAudio{_device};
        _audio->stop();
        audio_devices.push_back(_audio);
        // TODO newing the arrays happens in class … need to check and align with SDL implementation
        // audio_device->input_buffer  = new float[audio_device->input_channels * audio_device->buffer_size]{};
        // audio_device->output_buffer = new float[audio_device->output_channels * audio_device->buffer_size]{};
        return _device;
    }
} // namespace umgebung

umgebung::SubsystemAudio* umgebung_subsystem_audio_create_portaudio() { // TODO maybe rename all subsytems to `umgebung_subsystem_audio_portaudio_create`
    auto* audio         = new umgebung::SubsystemAudio{};
    audio->set_flags    = umgebung::set_flags;
    audio->init         = umgebung::init;
    audio->setup_pre    = umgebung::setup_pre;
    audio->setup_post   = umgebung::setup_post;
    audio->loop         = umgebung::loop;
    audio->draw_pre     = umgebung::draw_pre;
    audio->draw_post    = umgebung::draw_post;
    audio->shutdown     = umgebung::shutdown;
    audio->event        = umgebung::event;
    audio->start        = umgebung::start;
    audio->stop         = umgebung::stop;
    audio->create_audio = umgebung::create_audio;
    return audio;
}

#endif // ENABLE_PORTAUDIO