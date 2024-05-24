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

#pragma once

#include <string>
#include <stdbool.h>
#include <iostream>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include "Umgebung.h"

/**
 * @brief write audio files in WAV format, mono 32-bit float
 * TODO add support for other formats and multi-channels
 */
class AudioFileWriter {
public:
    AudioFileWriter() : opened(false) {
        format.container     = drwav_container_riff; // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
        format.format        = DR_WAVE_FORMAT_IEEE_FLOAT;
        format.channels      = 1;
        format.sampleRate    = DEFAULT_AUDIO_SAMPLE_RATE;
        format.bitsPerSample = 32;
    }

    ~AudioFileWriter() {
        drwav_uninit(&wav);
    }

    bool open(const std::string& filename) {
        if (opened) {
            return false;
        }
        if (!drwav_init_file_write(&wav, filename.c_str(), &format, NULL)) {
            std::cerr << "+++ @AudioFileWriter / error opening WAV file: " << filename << std::endl;
            return false;
        }
        opened = true;
        return true;
    }

    int write(size_t length, const float* buffer) {
        if (!opened) {
            return 0;
        }
        drwav_uint64 samples_written = drwav_write_pcm_frames(&wav, length, buffer);
        if (samples_written != length) {
            std::cerr << "+++ @AudioFileWriter / error writing WAV file" << std::endl;
            return 0;
        }
        return samples_written;
    }

    void close() {
        if (!opened) {
            return;
        }
        drwav_uninit(&wav);
        opened = false;
    }

private:
    bool              opened;
    drwav             wav;
    drwav_data_format format;
};