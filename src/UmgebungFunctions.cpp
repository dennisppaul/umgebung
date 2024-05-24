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

#include <iomanip>
#include <string>
#include <random>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <vector>

#if defined(SYSTEM_WIN32)
#include <windows.h>
#endif

#include "Umgebung.h"
#include "SimplexNoise.h"

namespace umgebung {

    static SimplexNoise fSimplexNoise;

    void audio_devices(int input_device, int output_device) {
        audio_input_device  = input_device;
        audio_output_device = output_device;
    }

    static unsigned int fRandomSeed = static_cast<unsigned int>(std::time(nullptr));
    static std::mt19937 gen(fRandomSeed); // Create a Mersenne Twister pseudo-random number generator with the specified seed

    float random(float min, float max) {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(gen);
    }

    float random(float max) {
        return random(0, max);
    }

    std::string nf(int number, int _width) {
        std::ostringstream oss;
        oss << std::setw(_width) << std::setfill('0') << number;
        return oss.str();
    }

    static int fNoiseSeed = static_cast<unsigned int>(std::time(nullptr));

    void noiseSeed(int seed) {
        fNoiseSeed = seed;
    }

    float noise(float x) {
        return fSimplexNoise.noise(x);
    }

    float noise(float x, float y) {
        return fSimplexNoise.noise(x, y);
    }

    float noise(float x, float y, float z) {
        return fSimplexNoise.noise(x, y, z);
    }

    float radians(float degrees) {
        return degrees * M_PI / 180.0f;
    }

    float degrees(float radians) {
        return radians * 180.0f / M_PI;
    }

    bool exists(const std::string& file_path) {
        std::filesystem::path path(file_path);
        return std::filesystem::exists(path);
    }

#if defined(SYSTEM_WIN32)
    std::string sketchPath_impl() {
        std::vector<char> buffer(MAX_PATH);
        DWORD             length = GetModuleFileNameA(NULL, buffer.data(), buffer.size());

        while (length == buffer.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            // Increase buffer size if the path was truncated
            buffer.resize(buffer.size() * 2);
            length = GetModuleFileNameA(NULL, buffer.data(), buffer.size());
        }

        if (length == 0) {
            // GetModuleFileName failed
            std::cerr << "Error retrieving path, error code: " << GetLastError() << std::endl;
            return std::string();
        }

        std::filesystem::path exePath(buffer.data());
        std::filesystem::path dirPath = exePath.parent_path();
        return dirPath.string() + "/";
    }
#elif defined(SYSTEM_UNIX)
#include <unistd.h>

    std::string sketchPath_impl() {
        std::vector<char> buf(1024);
        ssize_t           len = readlink("/proc/self/exe", buf.data(), buf.size());
        if (len != -1) {
            buf[len] = '\0'; // Null-terminate the string
            std::filesystem::path exePath(buf.data());
            std::filesystem::path dirPath = exePath.parent_path();
            return dirPath.string() + std::string("/");
        } else {
            std::cerr << "Error retrieving path" << std::endl;
            return std::string(); // Return an empty string in case of error
        }
    }

#elif defined(SYSTEM_MACOS)
#include <mach-o/dyld.h>

    const std::string sketchPath_impl() {
        uint32_t          size = 1024;
        std::vector<char> buf(size);
        if (_NSGetExecutablePath(buf.data(), &size) == 0) {
            std::filesystem::path exePath(buf.data());
            std::filesystem::path dirPath = exePath.parent_path();
            return dirPath.string() + std::string("/");
        } else {
            std::cerr << "Error retrieving path" << std::endl;
            return std::string(); // Return an empty string in case of error
        }
    }
#else

    std::string sketchPath_impl() {
        std::filesystem::path currentPath = std::filesystem::current_path();
        return currentPath.string() + std::string("/");
    }

#endif

    std::string sketchPath() {
        return sketchPath_impl();
    }
} // namespace umgebung