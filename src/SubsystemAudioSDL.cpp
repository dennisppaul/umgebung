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

namespace umgebung {
    struct PAudioSDL {
        PAudio*          audio_device;
        SDL_AudioStream* stream;
    };

    static void setup_pre() {}
    static void draw_pre() {}
    static void draw_post() {}

    static std::vector<PAudioSDL*> _audio_devices;
    static SDL_AudioStream*        stream = nullptr;

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_AUDIO;
    }

    static int current_sine_sample = 0;

    static void SDLCALL request_audio_samples(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount) {
        if (a != nullptr) {
            audioEvent();
        }

        if (userdata != nullptr) {
            const uint8_t c = static_cast<uint8_t*>(userdata)[0];
            console("message: ", c);
        }

        for (const auto audio_device: _audio_devices) {
            if (audio_device != nullptr && audio_device->audio_device != nullptr) {
                audioEvent(*audio_device->audio_device);
            }
        }

        /* total_amount is how much data the audio stream is eating right now, additional_amount is how much more it needs
        than what it currently has queued (which might be zero!). You can supply any amount of data here; it will take what
        it needs and use the extra later. If you don't give it enough, it will take everything and then feed silence to the
        hardware for the rest. Ideally, though, we always give it what it needs and no extra, so we aren't buffering more
        than necessary. */
        additional_amount /= sizeof(float); /* convert from bytes to samples */
        while (additional_amount > 0) {
            float     samples[128]; /* this will feed 128 samples each iteration until we have enough. */
            const int total = SDL_min(additional_amount, SDL_arraysize(samples));
            int       i;

            /* generate a 440Hz pure tone */
            for (i = 0; i < total; i++) {
                constexpr int freq  = 220;
                const float   phase = current_sine_sample * freq / 48000.0f;
                samples[i]          = SDL_sinf(phase * 2 * SDL_PI_F);
                current_sine_sample++;
            }

            /* wrapping around to avoid floating-point errors */
            current_sine_sample %= 48000;

            /* feed the new data to the stream. It will queue at the end, and trickle out as the hardware needs more data. */
            SDL_PutAudioStreamData(astream, samples, total * sizeof(float));
            additional_amount -= total; /* subtract what we've just fed the stream. */
        }
    }

    int print_audio_devices() {
        console("------------------------------------------");
        console("available audio drivers:");
        const int _num_audio_drivers = SDL_GetNumAudioDrivers();
        for (int i = 0; i < _num_audio_drivers; ++i) {
            console(SDL_GetAudioDriver(i));
        }
        console("------------------------------------------");
        console("INPUT (recording) audio devices:");
        {
            int                      _num_recording_devices;
            const SDL_AudioDeviceID* audio_input_device_ids = SDL_GetAudioRecordingDevices(&_num_recording_devices);
            if (audio_input_device_ids != nullptr) {
                for (int i = 0; i < _num_recording_devices; i++) {
                    const int   id    = audio_input_device_ids[i];
                    const char* _name = SDL_GetAudioDeviceName(id);
                    console(i, "\tID: ", audio_input_device_ids[i], "\tname: ", _name);
                    SDL_AudioSpec spec;
                    if (SDL_GetAudioDeviceFormat(id, &spec, nullptr)) {
                        console("\tch: ", spec.channels, ", ", spec.freq, "Hz, ", spec.format);
                    }
                }
            }
        }

        console("OUTPUT (playback) audio devices:");
        {
            int                      _num_playback_devices;
            const SDL_AudioDeviceID* audio_output_device_ids = SDL_GetAudioPlaybackDevices(&_num_playback_devices);
            if (audio_output_device_ids != nullptr) {
                for (int i = 0; i < _num_playback_devices; i++) {
                    const int   id    = audio_output_device_ids[i];
                    const char* _name = SDL_GetAudioDeviceName(id);
                    console(i, "\tID: ", audio_output_device_ids[i], "\tname: ", _name);
                    SDL_AudioSpec spec;
                    if (SDL_GetAudioDeviceFormat(id, &spec, nullptr)) {
                        console("\tch: ", spec.channels, ", ", spec.freq, "Hz, ", spec.format);
                    }
                }
            }
        }
        console("------------------------------------------");

        // const int numInputDevices = SDL_GetNumAudioDrivers();
        // std::cout << "numInputDevices:" << numInputDevices << std::endl;
        // for (int i = 0; i < numInputDevices; i++) {
        //     const char*   audio_driver_name = SDL_GetAudioDriver(i);
        //     const char*   deviceName        = SDL_GetAudioDeviceName(i);
        //     SDL_AudioSpec spec;
        //     int           sample_frames = 0;
        //     SDL_GetAudioDeviceFormat(i, &spec, nullptr);
        //     // std::cout << "- Input Device  : " << i << " : " << deviceName;
        //     // std::cout << " ( ";
        //     // std::cout << "channels : " << static_cast<int>(spec.channels);
        //     // std::cout << " + frequency : " << spec.freq;
        //     // std::cout << " )";
        //     std::cout << i;
        //     std::cout << "\t";
        //     if (deviceName != nullptr) {
        //         std::cout << deviceName;
        //     }
        //     std::cout << std::endl;
        // }

        // const int numOutputDevices = SDL_GetNumAudioDrivers();
        // for (int i = 0; i < numOutputDevices; i++) {
        //     const char*   deviceName = SDL_GetAudioDeviceName(i);
        //     SDL_AudioSpec spec;
        //     SDL_GetAudioDeviceFormat(i, &spec, nullptr);
        //     std::cout << "- Output Device : " << i << " : " << deviceName;
        //     std::cout << " ( ";
        //     std::cout << "channels : " << static_cast<int>(spec.channels);
        //     std::cout << " + frequency : " << spec.freq;
        //     std::cout << " )";
        //     std::cout << std::endl;
        // }

        return 0;
    }

    const char* message = "hello";
    static bool init() {
        // NOTE not sure if we should use the AudioStream concept. currently the mental model
        //     is slightly different: a subsystem correlates to an audio driver ( e.g SDL or PortAudio )
        //     and the actual audio devices are represented by `PAudio`. so each `create_audio` call
        //     would need to open a device and create a single stream for that device. this is different
        //     from `PGraphics` and graphics susbsytem where we have a single window and multiple contexts.

        // TODO see https://github.com/libsdl-org/SDL/blob/main/examples/audio

        console("init audio system");
        print_audio_devices();

        // TODO so this needs to be moved to PAudio which also need to store the stream
        SDL_AudioSpec spec;
        spec.channels = 2;
        spec.format   = SDL_AUDIO_F32;
        spec.freq     = 48000;
        // TODO make it an option to run via callback ( on own thread? ) or use `loop`

        stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, request_audio_samples, &message);
        if (!stream) {
            SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        return true;
    }

    static void setup_post() {
        SDL_ResumeAudioStreamDevice(stream);
    }

    static void loop() {
        // TODO acquire new audio samples from registered audio devices?
        //      see https://github.com/libsdl-org/SDL/blob/main/examples/audio/01-simple-playback/simple-playback.c
        // if (SDL_GetAudioStreamQueued(stream) < minimum_audio) {
        //     SDL_PutAudioStreamData(stream, samples, sizeof (samples));
        // }
    }

    static void shutdown() {
        for (const auto audio_device: _audio_devices) {
            if (audio_device != nullptr) {
                delete audio_device;
            }
        }
        _audio_devices.clear();
    }

    static void event(SDL_Event* event) {}

    static void register_audio_devices(PAudio* device) {
        const auto _device    = new PAudioSDL();
        _device->audio_device = device;

        // SDL_OpenAudioDevice();
        //
        // SDL_AudioSpec spec;
        // spec.channels = device.output_channels;
        // spec.format   = SDL_AUDIO_F32;
        // spec.freq     = 48000;
        // // TODO make it an option to run via callback ( on own thread? ) or use `loop`
        // stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, request_audio_samples, NULL);
        // if (!stream) {
        //     SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        // }

        _device->stream = nullptr;
        _audio_devices.push_back(_device);
    }

    static PAudio* create_audio(AudioDeviceInfo* device_info) {
        // TODO register audio devices here
        // TODO use `device_info`
        const auto audio_device = new PAudio{device_info};
        register_audio_devices(audio_device);
        return audio_device;
    }

    // TODO where does this live? also as a callback?
    void start() {
        SDL_ResumeAudioStreamDevice(stream);
    }

    // TODO where does this live? also as a callback?
    void stop() {
        SDL_PauseAudioStreamDevice(stream);
    }

} // namespace umgebung

umgebung::SubsystemAudio* umgebung_subsystem_audio_create_sdl() {
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
    return audio;
}