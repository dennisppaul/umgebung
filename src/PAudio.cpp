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
#include "PAudio.h"

using namespace umgebung;

PAudio::PAudio(const AudioDeviceInfo* device_info) : AudioDeviceInfo(*device_info) {
    // console("PAudio / created audio device: ");
    // console("name       : ", name);
    // console("id         : ", (id == DEFAULT_AUDIO_DEVICE ? "DEFAULT_AUDIO_DEVICE" : (id == FIND_AUDIO_DEVICE_BY_NAME ? "FIND_AUDIO_DEVICE_BY_NAME" : std::to_string(id))));
    // console("sample_rate: ", sample_rate);
}

void PAudio::copy_input_buffer_to_output_buffer() const {
    std::memcpy(output_buffer,                                 // destination
                input_buffer,                                  // source
                input_channels * buffer_size * sizeof(float)); // size
}
