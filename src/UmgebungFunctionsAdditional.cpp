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

#include <filesystem>

#if defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "Umgebung.h"

namespace umgebung {

    void audio_devices(const int input_device, const int output_device) {
        audio_input_device  = input_device;
        audio_output_device = output_device;
    }

    void color_inv(const uint32_t color, float& r, float& g, float& b, float& a) {
        a = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
        b = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        r = static_cast<float>(color & 0xFF) / 255.0f;
    }

    bool exists(const std::string& file_path) {
        const std::filesystem::path path(file_path);
        return std::filesystem::exists(path);
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
