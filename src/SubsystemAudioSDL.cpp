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
        SDL_AudioStream* sdl_stream;
    };

    static void setup_pre() {}
    static void draw_pre() {}
    static void draw_post() {}
    static void event(SDL_Event* event) {}

    static std::vector<PAudioSDL*> _audio_devices;

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_AUDIO;
    }

    static int current_sine_sample = 0;

    // TODO
    static void SDLCALL request_audio_samples(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount) {
        if (a != nullptr) {
            audioEvent();
        }

        if (userdata != nullptr) {
            const uint8_t c = static_cast<uint8_t*>(userdata)[0];
            console("message: ", c);
        }

        for (const auto device: _audio_devices) {
            if (device != nullptr &&
                device->audio_device != nullptr &&
                device->sdl_stream != nullptr &&
                device->sdl_stream == astream) {
                // console("request data from: ", a->audio_device->name);
                audioEvent(*device->audio_device);
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

            /* generate a 440Hz pure tone */
            for (int i = 0; i < total; i++) {
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

    void find_audio_input_devices(std::vector<AudioDeviceInfo>& devices) {
        int                      _num_recording_devices;
        const SDL_AudioDeviceID* _audio_device_ids = SDL_GetAudioRecordingDevices(&_num_recording_devices);
        if (_audio_device_ids != nullptr) {
            for (int i = 0; i < _num_recording_devices; i++) {
                const int   id    = _audio_device_ids[i];
                const char* _name = SDL_GetAudioDeviceName(id);
                if (_name == nullptr) {
                    console(i, "\tID: ", _audio_device_ids[i], "\tname: ", _name);
                    warning("failed to acquire audio device name for device:", _name);
                }
                SDL_AudioSpec spec;
                if (!SDL_GetAudioDeviceFormat(id, &spec, nullptr)) {
                    warning("failed to acquire audio device info for device:", _name);
                    continue;
                }
                // console("SDL_GetAudioFormatName: ", SDL_GetAudioFormatName(spec.format));
                AudioDeviceInfo _device;
                _device.id              = _audio_device_ids[i];
                _device.input_buffer    = nullptr;
                _device.input_channels  = spec.channels;
                _device.output_buffer   = nullptr;
                _device.output_channels = 0;
                _device.buffer_size     = BUFFER_SIZE_UNDEFINED;
                _device.sample_rate     = spec.freq;
                _device.name            = _name;
                devices.push_back(_device);
            }
        }
    }

    void find_audio_output_devices(std::vector<AudioDeviceInfo>& devices) {
        int                      _num_playback_devices;
        const SDL_AudioDeviceID* _audio_device_ids = SDL_GetAudioPlaybackDevices(&_num_playback_devices);
        if (_audio_device_ids != nullptr) {
            for (int i = 0; i < _num_playback_devices; i++) {
                const int   id    = _audio_device_ids[i];
                const char* _name = SDL_GetAudioDeviceName(id);
                if (_name == nullptr) {
                    console(i, "\tID: ", _audio_device_ids[i], "\tname: ", _name);
                    warning("failed to acquire audio device name for device:", _name);
                }
                SDL_AudioSpec spec;
                if (!SDL_GetAudioDeviceFormat(id, &spec, nullptr)) {
                    warning("failed to acquire audio device info for device:", _name);
                    continue;
                }
                AudioDeviceInfo _device;
                _device.id              = _audio_device_ids[i];
                _device.input_buffer    = nullptr;
                _device.input_channels  = spec.channels;
                _device.output_buffer   = nullptr;
                _device.output_channels = 0;
                _device.buffer_size     = BUFFER_SIZE_UNDEFINED;
                _device.sample_rate     = spec.freq;
                _device.name            = _name;
                devices.push_back(_device);
            }
        }
    }

    static constexpr int column_width = 80;
    static constexpr int format_width = 40;

    static void separator_headline() {
        console(separator(true, column_width));
    }

    static void separator_subheadline() {
        console(separator(false, column_width));
    }

    static void print_device_info(const AudioDeviceInfo& device) {
        console(format_label("- [" + to_string(device.id) + "]" + (device.id > 9 ? " " : "") + device.name, format_width),
                "in: ", device.input_channels, ", ",
                "out: ", device.output_channels, ", ",
                device.sample_rate, " Hz");
    }

    std::vector<AudioDeviceInfo> get_audio_info() {
        separator_headline();
        console("AUDIO INFO");

        separator_subheadline();
        console("AUDIO DEVICE DRIVERS");
        separator_subheadline();
        const int _num_audio_drivers = SDL_GetNumAudioDrivers();
        for (int i = 0; i < _num_audio_drivers; ++i) {
            console("- [", i, "]\t", SDL_GetAudioDriver(i));
        }
        console("( current audio driver: '", SDL_GetCurrentAudioDriver(), "' )");

        separator_subheadline();
        console("AUDIO INPUT DEVICES");
        separator_subheadline();
        std::vector<AudioDeviceInfo> _devices_found;
        find_audio_input_devices(_devices_found);
        for (auto d: _devices_found) {
            print_device_info(d);
        }

        separator_subheadline();
        console("AUDIO OUTPUT DEVICES");
        _devices_found.clear();
        find_audio_output_devices(_devices_found);
        for (auto d: _devices_found) {
            print_device_info(d);
        }
        separator_headline();

        return _devices_found;
    }

    static bool init() {
        // NOTE not sure if we should use the AudioStream concept. currently the mental model
        //     is slightly different: a subsystem correlates to an audio driver ( e.g SDL or PortAudio )
        //     and the actual audio devices are represented by `PAudio`. so each `create_audio` call
        //     would need to open a device and create a single stream for that device. this is different
        //     from `PGraphics` and graphics susbsytem where we have a single window and multiple contexts.

        // TODO see https://github.com/libsdl-org/SDL/blob/main/examples/audio

        separator_headline();
        console("initializing audio system");
        separator_headline();
        get_audio_info();

        return true;
    }

    static SDL_AudioStream* get_stream_from_paudio(const PAudio* device) {
        for (int i = 0; i < _audio_devices.size(); i++) {
            if (_audio_devices[i]->audio_device == device) {
                return _audio_devices[i]->sdl_stream;
            }
        }
        return nullptr;
    }

    void start(PAudio* device) {
        if (device == nullptr) {
            return;
        }

        SDL_AudioStream* _stream = get_stream_from_paudio(device);
        if (_stream == nullptr) {
            error("could not find audio stream for device: ", device->name);
            return;
        }

        /* bind audio stream to device */
        const SDL_AudioDeviceID _bound_id = SDL_GetAudioStreamDevice(_stream);
        if (_bound_id != device->id) {
            SDL_BindAudioStream(device->id, _stream);
            SDL_AudioSpec src_spec;
            SDL_AudioSpec dst_spec;
            if (SDL_GetAudioStreamFormat(_stream, &src_spec, &dst_spec)) {
                console("audio stream info:");
                console("src: ", src_spec.channels, ", ", src_spec.freq, ", ", SDL_GetAudioFormatName(src_spec.format));
                console("dst: ", dst_spec.channels, ", ", dst_spec.freq, ", ", SDL_GetAudioFormatName(dst_spec.format));
            }
            console("binding audio stream to device: ", device->name, ":", device->id);
        }

        SDL_ResumeAudioStreamDevice(_stream);
    }

    void stop(PAudio* device) {
        if (device == nullptr) {
            return;
        }

        SDL_AudioStream* _stream = get_stream_from_paudio(a);

        if (_stream == nullptr) {
            error("could not find audio stream for device: ", device->name);
            return;
        }

        SDL_PauseAudioStreamDevice(_stream);
    }

    static void setup_post() {
        // TODO decide if audio should be started here or later by client
        // start(a);
    }

    static void loop() {
        for (const auto _device: _audio_devices) {
            if (_device != nullptr &&
                _device->audio_device != nullptr &&
                _device->sdl_stream != nullptr) {
                constexpr int minimum_audio = (48000 * sizeof(float)) / 2; /* 8000 float samples per second. Half of that. */
                if (SDL_AudioDevicePaused(_device->audio_device->id)) {
                    continue;
                }
                if (SDL_AudioStreamDevicePaused(_device->sdl_stream)) {
                    continue;
                }
                if (SDL_GetAudioStreamQueued(_device->sdl_stream) < minimum_audio) {
                    audioEvent(*_device->audio_device);

                    if (_device->audio_device == a) {
                        audioEvent();
                    }

                    // /* this will feed 512 samples each frame until we get to our maximum. */
                    // static float samples[512];
                    // int          i;
                    //
                    // /* generate a 440Hz pure tone */
                    // for (i = 0; i < SDL_arraysize(samples); i++) {
                    //     constexpr int freq  = 220;
                    //     const float   phase = current_sine_sample * freq / 48000.0f;
                    //     samples[i]          = SDL_sinf(phase * 2 * SDL_PI_F);
                    //     current_sine_sample++;
                    // }
                    //
                    // /* wrapping around to avoid floating-point errors */
                    // current_sine_sample %= 48000;
                    //
                    // /* this will feed 512 samples each frame until we get to our maximum. */
                    // SDL_PutAudioStreamData(_device->sdl_stream, samples, sizeof(samples));
                    const float* buffer      = _device->audio_device->output_buffer;
                    const int    buffer_size = _device->audio_device->buffer_size * _device->audio_device->output_channels;
                    SDL_PutAudioStreamData(_device->sdl_stream, buffer, buffer_size * sizeof(float));
                }
            }
        }
    }

    static void shutdown() {
        for (const auto audio_device: _audio_devices) {
            if (audio_device != nullptr) {
                SDL_DestroyAudioStream(audio_device->sdl_stream);
                // delete(audio_device->audio_device); // this needs to be handled by caller of `create_audio`
                delete audio_device;
            }
        }
        _audio_devices.clear();
    }

    static void register_audio_devices(PAudio* device) {
        if (device == nullptr) {
            return;
        }

        if (device->input_channels > 0 && device->output_channels > 0) {
            warning("NOT IMPLEMENTED: trying to create device with input and output. ",
                    "currently only either out or input works. ",
                    "defaulting to just output device.");
            device->input_channels = 0;
        }

        if (device->id == DEFAULT_AUDIO_DEVICE) {
            console("trying to create default audio device");
            if (device->input_channels > 0) {
                device->id = SDL_AUDIO_DEVICE_DEFAULT_RECORDING;
            } else if (device->output_channels > 0) {
                device->id = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
            } else if (device->output_channels <= 0 && device->input_channels <= 0) {
                error("no audio channels specified. not creating audio device: ", device->id);
            } else {
                error("this should not happen");
            }
        }

        if (device->id == FIND_AUDIO_DEVICE_BY_NAME) {
            console("trying to find audio device by name (IMPLEMENT!!!)", device->name);
            // TODO implement find begins with or *almost* …
        }

        if (device->input_channels < 1 && device->output_channels < 1) {
            error("either input channels or output channels must be greater than 0. not creating audio device: ", device->id);
            return;
        }

        SDL_AudioSpec                spec;
        std::vector<AudioDeviceInfo> _devices_found;
        if (device->input_channels > 0) {
            spec.channels = device->input_channels;
            find_audio_input_devices(_devices_found);
        } else if (device->output_channels > 0) {
            spec.channels = device->output_channels;
            find_audio_output_devices(_devices_found);
        }

        spec.freq   = device->sample_rate;
        spec.format = SDL_AUDIO_F32; // NOTE currently only F32 is supported
                                     // float**     input_buffer; //  NOTE will be set later
                                     // float**     output_buffer; // NOTE will be set later
                                     // int         buffer_size; // NOTE will be set later
                                     // std::string name;
                                     // TODO make it an option to run via callback ( on own thread? ) or use `loop`

// #define USE_SDL_OPEN_AUDIO_DEVICE_STREAM
#ifdef USE_SDL_OPEN_AUDIO_DEVICE_STREAM
        stream     = SDL_OpenAudioDeviceStream(device->id, &spec, request_audio_samples, nullptr);
        device->id = SDL_GetAudioStreamDevice(stream);
#else
        const SDL_AudioSpec* _spec_will_be_set_internal = nullptr; // NOTE specs will be set internally

        static bool get_device_id_once = false;
        static int  _device_id;
        if (!get_device_id_once) {
            _device_id = SDL_OpenAudioDevice(device->id, _spec_will_be_set_internal);
            console("opening (physical) audio device once: ", _device_id);
            // device->id = SDL_OpenAudioDevice(device->id, &spec);
            if (_device_id == 0) {
                console("could not open audio device: (", _device_id, ") ", SDL_GetError());
                return;
            }
            get_device_id_once = true;
        } else {
            console("(physical) audio device already opened: ", _device_id);
        }
        device->id = _device_id;
        // NOTE streams will be created here, but only bound when stream i started with `start`
        SDL_AudioStream* stream = SDL_CreateAudioStream(&spec, _spec_will_be_set_internal);
        // SDL_PauseAudioDevice(device->id);

        // SDL_AudioStream* stream = SDL_CreateAudioStream(&spec, &spec);
        // SDL_BindAudioStream(device->id, stream);
        // SDL_PauseAudioStreamDevice(stream);

        // SDL_AudioSpec src_spec;
        // SDL_AudioSpec dst_spec;
        // if (SDL_GetAudioStreamFormat(stream, &src_spec, &dst_spec)) {
        //     console("audio stream info:");
        //     console("src: ", src_spec.channels, ", ", src_spec.freq, ", ", SDL_GetAudioFormatName(src_spec.format));
        //     console("dst: ", dst_spec.channels, ", ", dst_spec.freq, ", ", SDL_GetAudioFormatName(dst_spec.format));
        // }

#endif
        if (!stream) {
            error("couldn't create audio device stream: ", SDL_GetError(), "(", device->id, ")");
            return;
        }

        const char* _name = SDL_GetAudioDeviceName(device->id);
        if (_name == nullptr) {
            device->name = "(nA)"; // TODO can we need to do better than that …
        } else {
            device->name = _name;
        }

        console("created audio device: ", device->name);
        print_device_info(*device); // AudioDeviceInfo*

        const auto _device    = new PAudioSDL();
        _device->audio_device = device;
        _device->sdl_stream   = stream;
        _audio_devices.push_back(_device);
    }

    static PAudio* create_audio(const AudioDeviceInfo* device_info) {
        const auto audio_device = new PAudio{device_info};
        register_audio_devices(audio_device);
        audio_device->input_buffer  = new float[audio_device->input_channels * audio_device->buffer_size]{};
        audio_device->output_buffer = new float[audio_device->output_channels * audio_device->buffer_size]{};
        return audio_device;
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
    audio->start        = umgebung::start;
    audio->stop         = umgebung::stop;
    return audio;
}