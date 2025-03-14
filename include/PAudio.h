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

    void merge_interleaved_stereo(float* left, float* right, float* interleaved, size_t frames);
    void split_interleaved_stereo(float* left, float* right, const float* interleaved, size_t frames);

    struct AudioUnitInfo {
        /**
         * unique id of audio unit. id is set by audio subsystem. it is not to be confused with the device id.
         * a unit is a combination of input and output device.
         */
        int unique_id{AUDIO_UNIT_NOT_INITIALIZED};
        /** case-sensitive audio device name ( or beginning of the name )
         * if audio device is supposed to be intialized by name
         * make sure to set `input_device_id` to `FIND_AUDIO_DEVICE_BY_NAME`.
         * <code>
         * audio_device_info.input_device_id = FIND_AUDIO_DEVICE_BY_NAME;
         * audio_device_info.name            = "MacBook";
         * </code>
         * name may be reset or completed by audio system.
         */
        int         input_device_id{DEFAULT_AUDIO_DEVICE};
        std::string input_device_name{DEFAULT_AUDIO_DEVICE_NAME};
        /** buffer for audio input samples. buffer is interleaved, i.e. samples for each channel are contiguous in memory. */
        float*      input_buffer{nullptr};
        int         input_channels{0};
        int         output_device_id{DEFAULT_AUDIO_DEVICE};
        std::string output_device_name{DEFAULT_AUDIO_DEVICE_NAME};
        /** buffer for audio output samples. buffer is interleaved, i.e. samples for each channel are contiguous in memory. */
        float* output_buffer{nullptr};
        int    output_channels{0};
        /** number of samples per channel ( also referred to as *frames* )
         * e.g for a 2 channel device with `output_channels = 2` length of `output_buffer` length is `output_channels * buffer_size`
         */
        int buffer_size{DEFAULT_AUDIO_BUFFER_SIZE};
        int sample_rate{DEFAULT_SAMPLE_RATE};
        // int         format; // TODO currently supporting F32 only
    };

    class PAudio : public AudioUnitInfo {
    public:
        explicit PAudio(const AudioUnitInfo* device_info);
        void copy_input_buffer_to_output_buffer() const;
    };
} // namespace umgebung