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

#pragma once

#include <string>

namespace umgebung {
    struct AudioUnitInfo {
        /** device id as specified by audio driver.
         * to use the default device as selected in system preferences set to `AUDIO_DEVICE_DEFAULT`:
         * <code>
         * audio_device_info.id   = AUDIO_DEVICE_DEFAULT;
         * </code>
         * to identify the device by name set to `FIND_AUDIO_DEVICE_BY_NAME`:
         * <code>
         * audio_device_info.id   = FIND_AUDIO_DEVICE_BY_NAME;
         * </code>
         * might be reset by audio system.
         */
        int unique_id{AUDIO_DEVICE_NOT_INITIALIZED};
        /** buffer for audio input samples. buffer is interleaved, i.e. samples for each channel are contiguous in memory. */
        float* input_buffer{nullptr};
        int    input_channels{0};
        /** buffer for audio output samples. buffer is interleaved, i.e. samples for each channel are contiguous in memory. */
        float* output_buffer{nullptr};
        int    output_channels{0};
        /** number of samples per channel ( also referred to as *frames* )
         * e.g for a 2 channel device with `output_channels = 2` length of `output_buffer` length is `output_channels * buffer_size`
         */
        int buffer_size{DEFAULT_AUDIO_BUFFER_SIZE}; // TODO this defines the size in samples per channel. research if it is good that it s the same for input and out buffer
        int sample_rate{DEFAULT_SAMPLE_RATE};
        // int         format; // TODO currently supporting F32 only
        /** name of the audio device.
         * if audio device is supposed to be intialized by this name make sure to set `id` to `FIND_AUDIO_DEVICE_BY_NAME`.
         * <code>
         * audio_device_info.id   = FIND_AUDIO_DEVICE_BY_NAME;
         * audio_device_info.name = "MacBook";
         * </code>
         * might be reset by audio system if device is intialized by `id`.
         */
        // TODO does this really make sense how e.g does portaudio handle this? also 2 names for in and output … i think not
        std::string input_device_name;
        std::string output_device_name;
        // std::string unit_name; // TODO maybe better do it like this but how do we then choose the device?
    };

    class PAudio : public AudioUnitInfo {
    public:
        explicit PAudio(const AudioUnitInfo* device_info);
        void copy_input_buffer_to_output_buffer() const;
    };
} // namespace umgebung