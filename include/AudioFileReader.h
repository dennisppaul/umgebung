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

/**
 * @brief read audio files in WAV format, mono 32-bit float
 * TODO add support for other formats and multi-channels
 */
class AudioFileReader {
public:
    enum ReadStyle {
        NORMAL = 0,
        LOOP,
        FILL_WITH_ZERO
    };

    AudioFileReader() : opened(false) {}

    ~AudioFileReader() {
        drwav_uninit(&wav);
    }

    bool eof() { return current_position() >= length(); }

    bool open(const std::string& filename) {
        if (opened) {
            return false;
        }
        if (drwav_init_file(&wav, filename.c_str(), NULL) == DRWAV_FALSE) {
            std::cerr << "+++ error opening WAV file: " << filename << std::endl;
            return false;
        }
        opened = true;
        return true;
    }

    void close() {
        if (!opened) {
            return;
        }
        drwav_uninit(&wav);
        opened = false;
    }

    int read(int frames_to_read, float* buffer, ReadStyle read_style = NORMAL) {
        if (!opened) {
            return 0;
        }
        int mReadFrames = drwav_read_pcm_frames_f32(&wav, frames_to_read, buffer);
        switch(read_style) {
            case NORMAL:
                break;
            case LOOP:
                if (mReadFrames < frames_to_read) {
                    drwav_seek_to_pcm_frame(&wav, 0);
                    mReadFrames += drwav_read_pcm_frames_f32(&wav, frames_to_read - mReadFrames, buffer + mReadFrames);
                }
                break;
            case FILL_WITH_ZERO:
                for (int i = mReadFrames; i < frames_to_read; i++) {
                    buffer[i] = 0.0;
                }
                break;
        }
        return mReadFrames;
    }

    void rewind() {
        if (!opened) {
            return;
        }
        drwav_seek_to_pcm_frame(&wav, 0);
    }

    void seek(int frame) {
        if (!opened) {
            return;
        }
        drwav_seek_to_pcm_frame(&wav, frame);
    }

    int channels() {
        if (!opened) {
            return 0;
        }
        return wav.channels;
    }

    int length() {
        if (!opened) {
            return 0;
        }
        return wav.totalPCMFrameCount;
    }

    int bits_per_sample() {
        if (!opened) {
            return 0;
        }
        return wav.bitsPerSample;
    }

    int current_position() {
        if (!opened) {
            return 0;
        }
        return wav.readCursorInPCMFrames;
    }

private:
    bool  opened;
    drwav wav;
};