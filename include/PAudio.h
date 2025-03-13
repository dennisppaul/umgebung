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
    struct AudioDeviceInfo {
        int    id{DEFAULT_AUDIO_DEVICE};
        float* input_buffer{nullptr}; // TODO should this go to PAudio?
        int    input_channels{0};
        float* output_buffer{nullptr}; // TODO should this go to PAudio?
        int    output_channels{0};
        int    buffer_size{DEFAULT_AUDIO_BUFFER_SIZE};
        int    sample_rate{DEFAULT_SAMPLE_RATE};
        // int         format; // TODO currently supporting F32
        std::string name;
    };

    class PAudio : public AudioDeviceInfo {
    public:
        explicit PAudio(const AudioDeviceInfo* device_info);
        void copy_input_buffer_to_output_buffer() const;
    };
} // namespace umgebung