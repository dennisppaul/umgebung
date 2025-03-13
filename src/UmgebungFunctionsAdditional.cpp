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

    bool begins_with(const std::string& str, const std::string& prefix) {
        if (prefix.size() > str.size()) {
            return false;
        }
        return str.substr(0, prefix.size()) == prefix;
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

    std::vector<std::string> get_files(const std::string& directory, const std::string& extension) {
        std::vector<std::string> files;
        for (const auto& entry: std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == extension || extension == "*" || extension == "*.*" || extension.empty()) {
                files.push_back(entry.path().string());
            }
        }
        return files;
    }

    int get_int_from_argument(const std::string& argument) {
        /* get an int value from an argument formated like e.g this "value=23" */
        const std::size_t pos = argument.find('=');
        if (pos == std::string::npos) {
            throw std::invalid_argument("no '=' character found in argument.");
        }
        const std::string valueStr = argument.substr(pos + 1);
        return std::stoi(valueStr);
    }

    std::string get_string_from_argument(const std::string& argument) {
        /* get a string value from an argument formated like e.g this "value=twentythree" */
        std::size_t pos = argument.find('=');
        if (pos == std::string::npos) {
            throw std::invalid_argument("no '=' character found in argument.");
        }
        std::string valueStr = argument.substr(pos + 1);
        return valueStr;
    }

    std::string timestamp() {
        const auto         now   = std::chrono::system_clock::now();
        const auto         ms    = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        const auto         timer = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&timer), "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    void audio(const int input_channels, const int output_channels, const int sample_rate, const int buffer_size) {
        umgebung::input_channels    = input_channels;
        umgebung::output_channels   = output_channels;
        umgebung::sample_rate       = sample_rate;
        umgebung::audio_buffer_size = buffer_size;
    }

    void audio_start(PAudio* device) {
        if (device == nullptr) {
            subsystem_audio->start(a);
        } else {
            subsystem_audio->start(device);
        }
    }

    void audio_stop(PAudio* device) {
        if (device == nullptr) {
            subsystem_audio->stop(a);
        } else {
            subsystem_audio->stop(device);
        }
    }
} // namespace umgebung
