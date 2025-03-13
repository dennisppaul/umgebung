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

    static std::vector<PAudioSDL*> _audio_devices;
    static bool                    _audio_device_callback_mode = false; // TODO callback mode is not working well. also it appears to be slightly deprecated by SDL devs. maybe drop it at some point

    static void setup_pre() {}
    static void draw_pre() {}
    static void draw_post() {}
    static void event(SDL_Event* event) {}

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_AUDIO;
    }

    static bool request_audio_samples(PAudioSDL* const _device) {
        if (SDL_AudioDevicePaused(_device->audio_device->id)) {
            return true;
        }
        if (SDL_AudioStreamDevicePaused(_device->sdl_stream)) {
            return true;
        }
        // if (SDL_GetAudioStreamQueued(_device->sdl_stream) < minimum_audio) {
        const int request_num_sample_frames = _device->audio_device->buffer_size;
        if (SDL_GetAudioStreamQueued(_device->sdl_stream) < request_num_sample_frames) {
            // NOTE for default audio device
            if (_device->audio_device == a) {
                audioEvent();
            }

            // NOTE for all registered audio devices ( including default audio device )
            audioEvent(*_device->audio_device);

            const float* buffer      = _device->audio_device->output_buffer;
            const int    buffer_size = _device->audio_device->buffer_size * _device->audio_device->output_channels;
            SDL_PutAudioStreamData(_device->sdl_stream, buffer, buffer_size * sizeof(float));

            int input_bytes_available = SDL_GetAudioStreamAvailable(_device->sdl_stream);
            console("input_bytes_available [", _device->audio_device->id, "]: ", input_bytes_available);
            // extern SDL_DECLSPEC int SDLCALL SDL_GetAudioStreamData(SDL_AudioStream *stream, void *buf, int len);
            // SDL_GetAudioStreamData(_device->sdl_stream);
        }
        return false;
    }

    static void SDLCALL request_audio_samples_callback(void* userdata, SDL_AudioStream* astream, int additional_amount, int total_amount) {

        /* total_amount is how much data the audio stream is eating right now, additional_amount is how much more it needs
        than what it currently has queued (which might be zero!). You can supply any amount of data here; it will take what
        it needs and use the extra later. If you don't give it enough, it will take everything and then feed silence to the
        hardware for the rest. Ideally, though, we always give it what it needs and no extra, so we aren't buffering more
        than necessary. */

        for (const auto _device: _audio_devices) {
            if (_device != nullptr &&
                _device->audio_device != nullptr &&
                _device->sdl_stream != nullptr &&
                _device->sdl_stream == astream) {
                if (request_audio_samples(_device)) {
                    continue;
                }
            }
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
                _device.input_channels  = 0;
                _device.output_buffer   = nullptr;
                _device.output_channels = spec.channels;
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

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
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
                console("client side format ( application or src ): ", src_spec.channels, ", ", src_spec.freq, ", ", SDL_GetAudioFormatName(src_spec.format));
                console("driver side format ( phyisical or dst )  : ", dst_spec.channels, ", ", dst_spec.freq, ", ", SDL_GetAudioFormatName(dst_spec.format));
            }
            console("binding audio stream to device: ", device->name, " [", device->id, "]");
        }

        SDL_ResumeAudioStreamDevice(_stream);
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
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
                if (request_audio_samples(_device)) {
                    continue;
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

    static int find_audio_devices_by_name(const std::vector<AudioDeviceInfo>& vector, const std::string& name) {
        for (const auto& device: vector) {
            if (begins_with(device.name, name)) {
                console("found audio device by name: ", name, " [", device.id, "]");
                return device.id;
            }
        }
        return AUDIO_DEVICE_NOT_FOUND;
    }

    static void register_audio_devices(PAudio* device) {
        if (device == nullptr) {
            // ReSharper disable once CppDFAUnreachableCode
            return; // NOTE this should never happen …
        }

        if (device->input_channels > 0 && device->output_channels > 0) {
            warning("NOT IMPLEMENTED: trying to create device with input and output. ",
                    "currently only either out or input works. ",
                    "defaulting to just output device.");
            device->input_channels = 0;
        }

        if (device->id == AUDIO_DEVICE_DEFAULT) {
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

        if (device->id == AUDIO_DEVICE_FIND_BY_NAME) {
            console("trying to find audio device by name: ", device->name);
            std::vector<AudioDeviceInfo> _devices_found;
            find_audio_input_devices(_devices_found);
            find_audio_output_devices(_devices_found);
            const int _id = find_audio_devices_by_name(_devices_found, device->name);
            if (_id == AUDIO_DEVICE_NOT_FOUND) {
                console("did not find audio device '", device->name, "' trying default device");
                device->id = AUDIO_DEVICE_DEFAULT;
            }
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

        SDL_AudioStream* stream = nullptr;
        if (_audio_device_callback_mode) {
            // TODO this currently does not work with multilpe ( logical ) audio devices on the same physical audio device
            stream     = SDL_OpenAudioDeviceStream(device->id, &spec, request_audio_samples_callback, nullptr);
            device->id = SDL_GetAudioStreamDevice(stream);
        } else {
            const SDL_AudioSpec* _spec_will_be_set_internal = nullptr; // NOTE specs will be set internally
            static bool          get_device_id_once         = false;
            static int           _device_id;
            if (!get_device_id_once) {
                _device_id = SDL_OpenAudioDevice(device->id, _spec_will_be_set_internal);
                console("opening ( physical ) audio device once: ", _device_id);
                // device->id = SDL_OpenAudioDevice(device->id, &spec);
                if (_device_id == 0) {
                    console("could not open audio device: [", _device_id, "] ", SDL_GetError());
                    return;
                }
                get_device_id_once = true;
            } else {
                console("( physical ) audio device already opened: ", _device_id);
            }
            device->id = _device_id;
            // NOTE streams will be created here, but only bound when stream started with `start`
            stream = SDL_CreateAudioStream(&spec, _spec_will_be_set_internal);
        }
        if (!stream) {
            error("couldn't create audio device stream: ", SDL_GetError(), "[", device->id, "]");
            return;
        }

        const char* _name = SDL_GetAudioDeviceName(device->id);
        if (_name == nullptr) {
            device->name = "(nA)"; // TODO can we need to do better than that?
        } else {
            if (device->name != _name) {
                console("updating device name from '", device->name, "' to '", _name, "'");
            }
            device->name = _name;
        }

        console("created audio device: ", device->name, " [", device->id, "]");
        print_device_info(*device);

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