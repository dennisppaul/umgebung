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

#include "Umgebung.h"
#include "UmgebungSubsystems.h"
#include "PAudio.h"

#include <utility>
#include "portaudio.h"

namespace umgebung {

    static void setup_pre() {}
    static void setup_post() {}
    static void draw_pre() {}
    static void draw_post() {}
    static void event(SDL_Event* event) {}

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_AUDIO;
    }

    static PaStream* stream;

    static int audioCallback(const void* inputBuffer, void* outputBuffer,
                             unsigned long                   framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags           statusFlags,
                             void*                           userData) {
        (void) timeInfo;
        (void) statusFlags;
        (void) userData;
        // assuming the sample format is paFloat32
        auto in  = static_cast<const float*>(inputBuffer);
        auto out = static_cast<float*>(outputBuffer);
        //
        // // create 2D arrays on stack for input and output
        // std::vector<std::vector<float>> inputArray(audio_input_channels, std::vector<float>(framesPerBuffer));
        // std::vector<std::vector<float>> outputArray(audio_output_channels, std::vector<float>(framesPerBuffer));
        //
        // // pointers array to pass to processing function
        // std::vector<float*> inputPtrs(audio_input_channels);
        // std::vector<float*> outputPtrs(audio_output_channels);
        //
        // // fill input arrays from input buffer and setup pointers
        // for (unsigned int ch = 0; ch < audio_input_channels; ++ch) {
        //     inputPtrs[ch] = inputArray[ch].data();
        //     for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        //         if (audio_input_channels > 0) {
        //             inputArray[ch][i] = in[i * audio_input_channels + ch];
        //         }
        //     }
        // }
        //
        // // setup output pointers
        // for (unsigned int ch = 0; ch < audio_output_channels; ++ch) {
        //     outputPtrs[ch] = outputArray[ch].data();
        // }
        //
        // // process audio block
        // if (fApplet != nullptr) {
        //     fApplet->audioblock(inputPtrs.data(), outputPtrs.data(), static_cast<int>(framesPerBuffer));
        // }
        //
        // // write processed output to output buffer
        // for (unsigned int ch = 0; ch < audio_output_channels; ++ch) {
        //     for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        //         out[i * audio_output_channels + ch] = outputArray[ch][i];
        //     }
        // }
        return paContinue;
    }

    static void shutdown_audio() {
        PaError err;

        err = Pa_StopStream(stream);
        if (err != paNoError) {
            std::cerr << "+++ error when stopping stream: " << Pa_GetErrorText(err) << std::endl;
            return;
        }

        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            std::cerr << "+++ error when closing stream: " << Pa_GetErrorText(err) << std::endl;
            return;
        }

        Pa_Terminate();
    }

    int print_audio_devices() {
        int numDevices = Pa_GetDeviceCount();
        if (numDevices < 0) {
            std::cerr << "+++ error 'Pa_CountDevices' returned " << numDevices << std::endl;
            std::cerr << "+++ error when counting devices: " << Pa_GetErrorText(numDevices) << std::endl;
            return -1;
        }

        std::cout << "Found " << numDevices << " audio devices:" << std::endl;
        for (int i = 0; i < numDevices; i++) {
            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            std::cout << "Device " << i << ": " << deviceInfo->name << std::endl;
            std::cout << "  Max input channels: " << deviceInfo->maxInputChannels << std::endl;
            std::cout << "  Max output channels: " << deviceInfo->maxOutputChannels << std::endl;
            std::cout << "  Default sample rate: " << deviceInfo->defaultSampleRate << std::endl;

            const PaHostApiInfo* hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            if (hostInfo) {
                std::cout << "  Host API: " << hostInfo->name << std::endl;
            }
        }
        std::cout << "---" << std::endl;
        return 0;
    }

    static void init_audio(int           input_channels,
                           int           output_channels,
                           double        audio_sample_rate,
                           unsigned long audio_samples_per_frame) {
        PaError err;

        if (audio_input_device == AUDIO_DEVICE_DEFAULT && audio_output_device == AUDIO_DEVICE_DEFAULT) {
            std::cout << "Opening default audio device." << std::endl;
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
                std::cout << "Using default input device with ID: " << audio_input_device << std::endl;
            }
            if (audio_output_device < 0) {
                audio_output_device = Pa_GetDefaultOutputDevice();
                std::cout << "Using default output device with ID: " << audio_output_device << std::endl;
            }

            std::cout << "Opening audio device (input/output): (" << audio_input_device << "/" << audio_output_device << ")" << std::endl;
            const PaDeviceInfo*  deviceInfo  = Pa_GetDeviceInfo(audio_output_device);
            const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

            std::cout << "Opening stream for device with ID: " << deviceInfo->name;
            std::cout << "( Host API: " << hostApiInfo->name;
            std::cout << ", Channels (input/output): (" << input_channels << "/" << output_channels << ")";
            std::cout << " ) ... ";

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
                                audioCallback,
                                nullptr);
            std::cout << "OK." << std::endl;
        }

        if (err != paNoError) {
            std::cerr << "+++ error when opening stream: " << Pa_GetErrorText(err) << std::endl;
            return;
        }

        err = Pa_StartStream(stream);
        if (err != paNoError) {
            std::cerr << "++ error when starting stream: " << Pa_GetErrorText(err) << std::endl;
            return;
        }
    }

    static PAudio* create_audio(const AudioUnitInfo* device_info) {
        const auto audio_device = new PAudio{device_info};
        // register_audio_devices(audio_device);
        audio_device->input_buffer  = new float[audio_device->input_channels * audio_device->buffer_size]{};
        audio_device->output_buffer = new float[audio_device->output_channels * audio_device->buffer_size]{};
        return audio_device;
    }

    static bool init() {
        // NOTE not sure if we should use the AudioStream concept. currently the mental model
        //     is slightly different: a subsystem correlates to an audio driver ( e.g SDL or PortAudio )
        //     and the actual audio devices are represented by `PAudio`. so each `create_audio` call
        //     would need to open a device and create a single stream for that device. this is different
        //     from `PGraphics` and graphics susbsytem where we have a single window and multiple contexts.

        // TODO see https://github.com/libsdl-org/SDL/blob/main/examples/audio

        console("initializing PortAudio audio system");
        const PaError err = Pa_Initialize();
        if (err != paNoError) {
            error("Error message: ", Pa_GetErrorText(err));
            return false;
        }

        print_audio_devices();

        return true;
    }

    static void loop() {}
    static void shutdown() {}
    static void start(PAudio* device) {}
    static void stop(PAudio* device) {}

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
    audio->create_audio = umgebung::create_audio;
    audio->start        = umgebung::start;
    audio->stop         = umgebung::stop;
    return audio;
}