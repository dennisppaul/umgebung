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

#if defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "Umgebung.h"
#include "SimplexNoise.h"
#include "UmgebungFunctions.h"

namespace umgebung {

    void audio_devices(const int input_device, const int output_device) {
        audio_input_device  = input_device;
        audio_output_device = output_device;
    }

    static unsigned int fRandomSeed = static_cast<unsigned int>(std::time(nullptr));
    static std::mt19937 gen(fRandomSeed); // Create a Mersenne Twister pseudo-random number generator with the specified seed

    float random(const float min, const float max) {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(gen);
    }

    float random(const float max) {
        return random(0, max);
    }

    void color_inv(const uint32_t color, float& r, float& g, float& b, float& a) {
        a = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
        b = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        r = static_cast<float>(color & 0xFF) / 255.0f;
    }

    uint32_t color(const float r, const float g, const float b, const float a) {
        return static_cast<uint32_t>(a * 255) << 24 |
               static_cast<uint32_t>(b * 255) << 16 |
               static_cast<uint32_t>(g * 255) << 8 |
               static_cast<uint32_t>(r * 255);
    }

    uint32_t color(const float c, const float a) {
        return color(c, c, c, a);
    }

    uint32_t color(const float r, const float g, const float b) {
        return color(r, g, b, 1);
    }

    float red(const uint32_t color) {
        return static_cast<float>((color & 0x000000FF) >> 0) / 255.0f;
    }

    float green(const uint32_t color) {
        return static_cast<float>((color & 0x0000FF00) >> 8) / 255.0f;
    }

    float blue(const uint32_t color) {
        return static_cast<float>((color & 0x00FF0000) >> 16) / 255.0f;
    }

    float alpha(const uint32_t color) {
        return static_cast<float>((color & 0xFF000000) >> 24) / 255.0f;
    }

    std::string nf(const int number, const int width) {
        std::ostringstream oss;
        oss << std::setw(width) << std::setfill('0') << number;
        return oss.str();
    }

    static int fNoiseSeed = static_cast<int>(std::time(nullptr));

    void noiseSeed(const int seed) {
        fNoiseSeed = seed;
    }

    float noise(const float x) {
        return SimplexNoise::noise(x);
    }

    float noise(const float x, const float y) {
        return SimplexNoise::noise(x, y);
    }

    float noise(const float x, const float y, const float z) {
        return SimplexNoise::noise(x, y, z);
    }

    float radians(const float degrees) {
        return M_PI * degrees / 180.0f;
    }

    float degrees(const float radians) {
        return static_cast<float>(radians * 180.0f / M_PI);
    }

    bool exists(const std::string& file_path) {
        const std::filesystem::path path(file_path);
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

    std::string sketchPath_impl() {
        uint32_t          size = 1024;
        std::vector<char> buf(size);
        if (_NSGetExecutablePath(buf.data(), &size) == 0) {
            const std::filesystem::path exePath(buf.data());
            const std::filesystem::path dirPath = exePath.parent_path();
            return dirPath.string() + std::string("/");
        } else {
            std::cerr << "Error retrieving path" << std::endl;
            return {}; // Return an empty string in case of error
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

    std::string find_file_in_paths(const std::vector<std::string>& paths, const std::string& filename) {
        for (const auto& path: paths) {
            std::filesystem::path full_path = std::filesystem::path(path) / filename;
            if (std::filesystem::exists(full_path)) {
                return full_path.string();
            }
        }
        return "";
    }

    std::string find_in_environment_path(const std::string& filename) {
        std::string path;
        char*       env = getenv("PATH");
        if (env != nullptr) {
            std::string path_env(env);
            // On Windows, PATH entries are separated by ';', on Linux/macOS by ':'
#if defined(_WIN32)
            char path_separator = ';';
#else
            char path_separator = ':';
#endif

            std::istringstream ss(path_env);
            std::string        token;

            while (std::getline(ss, token, path_separator)) {
                std::filesystem::path candidate = std::filesystem::path(token) / filename;
                if (std::filesystem::exists(candidate)) {
                    path = candidate.string();
                    break;
                }
            }
        }
        return path;
    }

    std::string get_executable_location() {
#if defined(__APPLE__) || defined(__linux__)
        Dl_info info;
        // Get the address of a function within the library (can be any function)
        if (dladdr((void*) &get_executable_location, &info)) {
            std::filesystem::path lib_path(info.dli_fname);                                      // Full path to the library
            return lib_path.parent_path().string() + std::filesystem::path::preferred_separator; // Return the directory without the library name
        } else {
            std::cerr << "Could not retrieve library location (dladdr)" << std::endl;
            return "";
        }
#elif defined(_WIN32)
        HMODULE hModule = nullptr; // Handle to the DLL
        char    path[MAX_PATH];

        if (GetModuleFileNameA(hModule, path, MAX_PATH) != 0) {
            std::filesystem::path lib_path(path);                                                // Full path to the DLL
            return lib_path.parent_path().string() + std::filesystem::path::preferred_separator; // Add the separator
        } else {
            std::cerr << "Could not retrieve library location (GetModuleFileName)" << std::endl;
            return "";
        }
#endif
    }
} // namespace umgebung