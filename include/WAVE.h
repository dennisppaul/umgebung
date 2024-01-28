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

#include <fstream>
#include <vector>
#include <array>
#include <cstdint>
#include <iostream>
#include <cstdint>

class WAVE {
public:
    static constexpr uint16_t AUDIO_FORMAT_PCM       = 1;
    static constexpr uint16_t AUDIO_FORMAT_FLOAT     = 3;
    static constexpr uint16_t AUDIO_FORMAT_MULAW     = 6;
    static constexpr uint16_t AUDIO_FORMAT_ALAW      = 7;
    static constexpr uint16_t AUDIO_FORMAT_IBM_MULAW = 257;
    static constexpr uint16_t AUDIO_FORMAT_ADPCM     = 259;

    explicit WAVE(const std::string &filename) {
        file.open(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file");
        }
        readInfo();
    }

    ~WAVE() {
        if (file.is_open()) {
            file.close();
        }
    }

    std::vector<uint8_t> get_raw_data_block(int num_bytes) {
        std::vector<uint8_t> buffer(num_bytes);
        file.read(reinterpret_cast<char *>(buffer.data()), num_bytes);

        const int bytesRead = file.gcount();
        buffer.resize(bytesRead);

        return buffer;
    }

    std::vector<float> get_frame_block(int num_frame) {
        return get_sample_block(num_frame * fNumChannels);
    }

    std::vector<float> get_sample_block(int num_samples) {
        // Calculate the number of bytes per sample (taking into account the bit depth and number of channels)
        const int bytesPerSample = (fBitDepth / 8);
        const int numBytes       = num_samples * bytesPerSample;

        // Create a buffer of the calculated size
        std::vector<uint8_t> buffer(numBytes);
        file.read(reinterpret_cast<char *>(buffer.data()), numBytes);

        // Check how many bytes were actually read
        const int bytesRead = file.gcount();
        // if (bytesRead < numBytes) {
        //     // Less data than expected - could be EOF or read error
        //     if (file.eof()) {
        //         std::cerr << "Reached end of file." << std::endl;
        //         // Handle EOF if necessary
        //     } else if (file.fail()) {
        //         std::cerr << "Read error." << std::endl;
        //         // Handle read error if necessary
        //     }
        // }
        buffer.resize(bytesRead);

        // Convert the buffer to float and return
        return convertToFloat(buffer, fAudioFormat); // to array `float* array = vec.data();`
    }

    bool end_of_file() const {
        return file.eof();
    }

    float *get_samples(float *samples) {
        if (!file.is_open()) {
            throw std::runtime_error("File not open");
        }

        rewind();

        // const uint32_t numSamples = get_total_samples();
        // auto *         samples    = new float[numSamples];

        constexpr size_t chunkSize = 4096; // 4 KB
        auto *           buffer    = new uint8_t[chunkSize];

        try {
            size_t   totalBytesProcessed = 0;
            uint32_t sampleIndex         = 0;
            while (totalBytesProcessed < fDataChunkSize) {
                const size_t bytesToRead = std::min(chunkSize, fDataChunkSize - totalBytesProcessed);
                file.read(reinterpret_cast<char *>(buffer), bytesToRead);

                if (!file) {
                    std::cerr << "Warning: problem reading data." << std::endl;
                    break;
                }

                const std::streamsize bytesRead = file.gcount();
                totalBytesProcessed += bytesRead;

                if (bytesRead < static_cast<std::streamsize>(bytesToRead)) {
                    std::cerr << "Warning: Less data read than expected." << std::endl;
                }

                // Process the buffer and convert to float values
                for (size_t i = 0; i < bytesRead; i += (fBitDepth / 8)) {
                    if (fBitDepth == 16) {
                        // 16-bit PCM
                        const int16_t val      = buffer[i] | (buffer[i + 1] << 8);
                        samples[sampleIndex++] = static_cast<float>(val) / 32768.0f;
                    } else if (fBitDepth == 8) {
                        // 8-bit PCM
                        samples[sampleIndex++] = (static_cast<float>(buffer[i]) - 128.0f) / 128.0f;
                    } else {
                        // Other bit depths can be handled similarly
                        throw std::runtime_error("Unsupported bit depth");
                    }
                }
            }
        } catch (...) {
            delete[] buffer;  // Ensure buffer is deleted in case of an exception
            delete[] samples; // Also delete samples to avoid memory leak
            throw;            // Re-throw the exception
        }

        delete[] buffer; // Don't forget to delete the buffer after use
        return samples;
    }

    uint16_t get_audio_format() const {
        return fAudioFormat;
    }

    uint16_t get_num_channels() const {
        return fNumChannels;
    }

    uint32_t get_sample_rate() const {
        return fSampleRate;
    }

    uint32_t get_byte_rate() const {
        return fByteRate;
    }

    uint16_t get_block_align() const {
        return fBlockAlign;
    }

    uint16_t get_bit_depth() const {
        return fBitDepth;
    }

    float get_length_seconds() const {
        if (fByteRate == 0) {
            throw std::runtime_error("Byte rate is zero, can't calculate length.");
        }
        return static_cast<float>(fDataChunkSize) / static_cast<float>(fByteRate);
    }

    uint32_t get_total_samples() const {
        const uint32_t samplesPerChannel = fDataChunkSize / (fNumChannels * (fBitDepth / 8));
        return samplesPerChannel * fNumChannels;
    }

    void rewind() {
        file.seekg(fStartOfSampleData);
    }

    static std::string get_audio_format_name(uint16_t audio_format_id) {
        switch (audio_format_id) {
            case AUDIO_FORMAT_PCM:
                return "PCM";
            case AUDIO_FORMAT_FLOAT:
                return "FLOAT";
            case AUDIO_FORMAT_MULAW:
                return "MULAW";
            case AUDIO_FORMAT_ALAW:
                return "ALAW";
            case AUDIO_FORMAT_IBM_MULAW:
                return "IBM_MULAW";
            case AUDIO_FORMAT_ADPCM:
                return "ADPCM";
            default:
                return "UNKNOWN";
        }
    }

    void print_info() const {
        std::cout << "audio format       : " << get_audio_format_name(fAudioFormat);
        std::cout << " (" << fAudioFormat << ")" << std::endl;
        std::cout << "number of channels : " << fNumChannels << std::endl;
        std::cout << "sample rate        : " << fSampleRate << std::endl;
        std::cout << "byte rate          : " << fByteRate << std::endl;
        std::cout << "block align        : " << fBlockAlign << std::endl;
        std::cout << "bit depth          : " << fBitDepth << std::endl;
        std::cout << "length(sec)        : " << get_length_seconds() << std::endl;
        std::cout << "total samples      : " << get_total_samples() << std::endl;
        std::cout << "samples per channel: " << get_total_samples() / fNumChannels << std::endl;
    }

private:
    std::ifstream  file;
    uint16_t       fAudioFormat{};
    uint16_t       fNumChannels{};
    uint32_t       fSampleRate{};
    uint32_t       fByteRate{};
    uint16_t       fBlockAlign{};
    uint16_t       fBitDepth{};
    uint32_t       fDataChunkSize{};
    std::streampos fStartOfSampleData = 0;

    void readInfo() {
        if (!file.is_open()) {
            throw std::runtime_error("File not open");
        }

        // WAV file header
        std::array<char, 44> header{};
        file.read(header.data(), header.size());

        if (file.gcount() != header.size()) {
            throw std::runtime_error("Failed to read the WAV header");
        }

        // check the "RIFF" chunk descriptor and "WAVE" format
        if (std::string(header.data(), 4) != "RIFF" || std::string(header.data() + 8, 4) != "WAVE") {
            throw std::runtime_error("Invalid WAV file");
        }

        // parse header
        fAudioFormat = *reinterpret_cast<uint16_t *>(header.data() + 20);
        fNumChannels = *reinterpret_cast<uint16_t *>(header.data() + 22);
        fSampleRate  = *reinterpret_cast<uint32_t *>(header.data() + 24);
        fByteRate    = *reinterpret_cast<uint32_t *>(header.data() + 28);
        fBlockAlign  = *reinterpret_cast<uint16_t *>(header.data() + 32);
        fBitDepth    = *reinterpret_cast<uint16_t *>(header.data() + 34);

        // number of samples
        file.seekg(36, std::ios::beg);
        // 36 is the standard offset after the 44-byte header, minus the 8 bytes for the chunk ID and size.

        while (file) {
            std::array<char, 4> chunkId{};
            file.read(chunkId.data(), chunkId.size());

            // Convert chunkId to a string for comparison and debug output
            std::string chunkIdStr(chunkId.data(), chunkId.size());

            // // Print each byte in hex format for debugging
            // std::cout << "Chunk ID: ";
            // for (char c: chunkId) {
            //     std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(static_cast<
            //         unsigned char>(c)) << " ";
            // }
            // std::cout << " (" << chunkIdStr << ")" << std::endl;

            if (std::strncmp(chunkId.data(), "data", 4) == 0) {
                // Found the 'data' chunk. The next 4 bytes are the size.
                uint32_t chunkSize;
                file.read(reinterpret_cast<char *>(&chunkSize), sizeof(chunkSize));
                fDataChunkSize     = chunkSize;
                fStartOfSampleData = file.tellg();
                break;
            } else {
                // Read the chunk size
                uint32_t chunkSize;
                file.read(reinterpret_cast<char *>(&chunkSize), sizeof(chunkSize));

                // Skip the chunk data
                file.seekg(chunkSize, std::ios_base::cur);
            }
        }

        // check for supported audio formats
        if (fAudioFormat != AUDIO_FORMAT_PCM && fAudioFormat != AUDIO_FORMAT_FLOAT &&
            fAudioFormat != AUDIO_FORMAT_MULAW && fAudioFormat != AUDIO_FORMAT_ALAW &&
            fAudioFormat != AUDIO_FORMAT_IBM_MULAW && fAudioFormat != AUDIO_FORMAT_ADPCM) {
            std::cerr << "Unsupported audio format: " << fAudioFormat << std::endl;
            throw std::runtime_error("Unsupported audio format");
        }
    }

    std::vector<float> convertToFloat(std::vector<uint8_t> &buffer, uint16_t format) const {
        std::vector<float> floatBuffer;

        if (format == AUDIO_FORMAT_PCM) {
            // Check for bit depth (e.g., 16-bit, 8-bit, 32-bit)
            if (fBitDepth == 16) {
                for (size_t i = 0; i < buffer.size(); i += 2) {
                    const int16_t sample = buffer[i] | (buffer[i + 1] << 8);
                    floatBuffer.push_back(static_cast<float>(sample) / 32768.0f);
                }
            } else if (fBitDepth == 8) {
                for (size_t i = 0; i < buffer.size(); i++) {
                    const auto sample = static_cast<int8_t>(buffer[i] - 128);
                    floatBuffer.push_back(static_cast<float>(sample) / 128.0f);
                }
            } else if (fBitDepth == 32) {
                for (size_t i = 0; i < buffer.size(); i += 4) {
                    const auto sample = reinterpret_cast<float *>(&buffer[i]);
                    floatBuffer.push_back(*sample);
                }
            } else {
                throw std::runtime_error("Unsupported bit depth for PCM");
            }
        } else if (format == AUDIO_FORMAT_FLOAT) {
            for (size_t i = 0; i < buffer.size(); i += 4) {
                const auto *sample = reinterpret_cast<float *>(&buffer[i]);
                floatBuffer.push_back(*sample);
            }
        } else {
            throw std::runtime_error("Unsupported audio format for conversion");
        }

        return floatBuffer;
    }
};
