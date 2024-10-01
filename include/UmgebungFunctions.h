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

#include <iostream>
#include <sstream>
#include <string>

// TODO check if these functions should be moved to PApplet?

namespace umgebung {
    void exit();

    void audio_devices(int input_device, int output_device);

    /* static functions for PApplet */

    uint32_t color(float r, float g, float b, float a);

    uint32_t color(float r, float g, float b);

    float red(uint32_t color);

    float green(uint32_t color);

    float blue(uint32_t color);

    float alpha(uint32_t color);

    float random(float min, float max);

    float random(float max);

    std::string nf(int number, int width);

    float noise(float x);

    float noise(float x, float y);

    float noise(float x, float y, float z);

    float radians(float degrees);

    float degrees(float radians);

    bool exists(const std::string& file_path);

    std::string sketchPath();

#define FLUSH_PRINT

    template<typename T>
    static auto to_printable(const T& value) -> typename std::conditional<std::is_same<T, uint8_t>::value, int, const T&>::type {
        if constexpr (std::is_same<T, uint8_t>::value) {
            return static_cast<int>(value);
        } else {
            return value;
        }
    }

    template<typename... Args>
    void print(const Args&... args) {
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cout << os.str();
#ifdef FLUSH_PRINT
        std::flush(std::cout);
#endif
    }

    template<typename... Args>
    void println(const Args&... args) {
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cout << os.str() << std::endl;
#ifdef FLUSH_PRINT
        std::flush(std::cout);
#endif
    }

    template<typename T>
    void printArray(const std::vector<T>& vec) {
        for (size_t i = 0; i < vec.size(); ++i) {
            std::cout << "[" << i << "] " << vec[i] << std::endl;
        }
#ifdef FLUSH_PRINT
        std::flush(std::cout);
#endif
    }

    template<typename... Args>
    std::string to_string(const Args&... args) {
        std::ostringstream oss;
        (oss << ... << args);
        return oss.str();
    }

    std::string get_executable_location();
    std::string find_file_in_paths(const std::vector<std::string>& paths, const std::string& filename);
    std::string find_in_environment_path(const std::string& filename);

} // namespace umgebung
