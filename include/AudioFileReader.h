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

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

/**
 * @brief read audio files in WAV format, mono 32-bit float
 * TODO add support for other formats and multi-channels
 */
class AudioFileReader {
private:
    class Reader {
    public:
        virtual ~Reader() {}
        virtual bool open(const std::string& filepath)       = 0;
        virtual bool open(const char* data, size_t size)     = 0;
        virtual void close()                                 = 0;
        virtual int  read(int frames_to_read, float* buffer) = 0;
        virtual void rewind()                                = 0;
        virtual void seek(int frame)                         = 0;
        virtual int  channels()                              = 0;
        virtual int  length()                                = 0;
        virtual int  current_position()                      = 0;
        virtual bool eof()                                   = 0;
        virtual int  sample_rate()                           = 0;
    };

    class MP3Reader : public Reader {
        drmp3 mp3;
        int   frame_size;

    public:
        bool open(const std::string& filepath) {
            bool mInitState = drmp3_init_file(&mp3, filepath.c_str(), NULL);
            if (mInitState) {
                frame_size = drmp3_get_pcm_frame_count(&mp3);
            }
            return mInitState;
        }

        bool open(const char* data, size_t size) {
            return drmp3_init_memory(&mp3, data, size, NULL);
        }

        void close() {
            drmp3_uninit(&mp3);
        }

        int read(int frames_to_read, float* buffer) {
            return drmp3_read_pcm_frames_f32(&mp3, frames_to_read, buffer);
        }

        void rewind() {
            drmp3_seek_to_pcm_frame(&mp3, 0);
        }

        void seek(int frame) {
            drmp3_seek_to_pcm_frame(&mp3, frame);
        }

        int channels() {
            return mp3.channels;
        }

        int length() {
            return frame_size;
        }

        int current_position() {
            return mp3.currentPCMFrame;
        }

        bool eof() {
            return mp3.atEnd;
        }

        int sample_rate() {
            return mp3.sampleRate;
        }
    };

    class WAVReader : public Reader {
        drwav wav;

    public:
        bool open(const std::string& filepath) {
            return drwav_init_file(&wav, filepath.c_str(), NULL);
        }

        bool open(const char* data, size_t size) {
            return drwav_init_memory(&wav, data, size, NULL);
        }

        void close() {
            drwav_uninit(&wav);
        }

        int read(int frames_to_read, float* buffer) {
            return drwav_read_pcm_frames_f32(&wav, frames_to_read, buffer);
        }

        void rewind() {
            drwav_seek_to_pcm_frame(&wav, 0);
        }

        void seek(int frame) {
            drwav_seek_to_pcm_frame(&wav, frame);
        }

        int channels() {
            return wav.channels;
        }

        int length() {
            return wav.totalPCMFrameCount;
        }

        int bits_per_sample() {
            return wav.bitsPerSample;
        }

        int current_position() {
            return wav.readCursorInPCMFrames;
        }

        bool eof() {
            return current_position() >= length();
        }

        int sample_rate() {
            return wav.sampleRate;
        }
    };

public:
    enum ReadStyle {
        NORMAL = 0,
        LOOP, // @TODO does not work so well with MP3 files
        FILL_WITH_ZERO
    };

    AudioFileReader() : fIsOpened(false) {}

    ~AudioFileReader() {
        close();
    }

    bool eof() {
        if (!fIsOpened) {
            return false;
        }
        return fReader->eof();
    }

    static float* load(const std::string& filepath,
                       unsigned int&      channels,
                       unsigned int&      sample_rate,
                       drwav_uint64&      length) {
        FileType mFileType = determineFileType(filepath);
        if (mFileType == FileType::WAV) {
            float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(filepath.c_str(),
                                                                         &channels,
                                                                         &sample_rate,
                                                                         &length,
                                                                         NULL);
            if (pSampleData == NULL) {
                std::cerr << "+++ error opening file: " << filepath << std::endl;
            }
            return pSampleData;
        } else if (mFileType == FileType::MP3) {
            drmp3_config config;
            float*       pSampleData = drmp3_open_file_and_read_pcm_frames_f32(filepath.c_str(),
                                                                               &config,
                                                                               &length,
                                                                               NULL);
            // set channels from config
            channels    = config.channels;
            sample_rate = config.sampleRate;
            if (pSampleData == NULL) {
                std::cerr << "+++ error opening file: " << filepath << std::endl;
            }
            return pSampleData;
        }
        std::cerr << "+++ unknown file type: " << filepath << std::endl;
        return nullptr;
    }

    bool open(const std::string& filepath) {
        if (fIsOpened) {
            return false;
        }

        FileType mFileType = determineFileType(filepath);
        if (mFileType == FileType::WAV) {
            fFileType = FileType::WAV;
            fReader   = new WAVReader();
        } else if (mFileType == FileType::MP3) {
            fFileType = FileType::MP3;
            fReader   = new MP3Reader();
        } else {
            std::cerr << "+++ unknown file type: " << filepath << std::endl;
            return false;
        }

        if (!fReader->open(filepath)) {
            delete fReader;
            std::cerr << "+++ error opening file: " << filepath << std::endl;
            return false;
        }

        fIsOpened = true;
        return true;
    }

    void close() {
        if (!fIsOpened) {
            return;
        }
        fReader->close();
        fIsOpened = false;
    }

    int read(int frames_to_read, float* buffer, ReadStyle read_style = NORMAL) {
        if (!fIsOpened) {
            return 0;
        }
        int mReadFrames = fReader->read(frames_to_read, buffer);
        switch (read_style) {
            case NORMAL:
                break;
            case LOOP:
                if (mReadFrames < frames_to_read) {
                    fReader->rewind();
                    mReadFrames += fReader->read(frames_to_read - mReadFrames, buffer + mReadFrames);
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
        if (!fIsOpened) {
            return;
        }
        fReader->rewind();
    }

    void seek(int frame) {
        if (!fIsOpened) {
            return;
        }
        fReader->seek(frame);
    }

    int channels() {
        if (!fIsOpened) {
            return 0;
        }
        return fReader->channels();
    }

    int length() {
        if (!fIsOpened) {
            return 0;
        }
        return fReader->length();
    }

    int current_position() {
        if (!fIsOpened) {
            return 0;
        }
        return fReader->current_position();
    }

    int sample_rate() {
        if (!fIsOpened) {
            return 0;
        }
        return fReader->sample_rate();
    }

    int type() {
        if (!fIsOpened) {
            return 0;
        }
        return fFileType;
    }

private:
    enum FileType {
        UNKNOWN = 0,
        WAV,
        MP3
    };

    bool     fIsOpened;
    Reader*  fReader;
    FileType fFileType = FileType::UNKNOWN;

    static std::string toLower(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

    static FileType determineFileType(const std::string& filepath) {
        size_t dotPos = filepath.find_last_of('.');

        if (dotPos == std::string::npos || dotPos == filepath.length() - 1) {
            return FileType::UNKNOWN;
        }

        std::string extension = toLower(filepath.substr(dotPos + 1));

        if (extension == "wav") {
            return FileType::WAV;
        } else if (extension == "mp3") {
            return FileType::MP3;
        } else {
            return FileType::UNKNOWN;
        }
    }
};