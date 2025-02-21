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

#pragma once

#include <cstdint>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "UmgebungDefines.h"

namespace umgebung {

    template<typename T>
    static auto to_printable(const T& value) -> typename std::conditional<std::is_same<T, uint8_t>::value, int, const T&>::type {
        if constexpr (std::is_same_v<T, uint8_t>) {
            return static_cast<int>(value);
        } else {
            return value;
        }
    }

    template<typename... Args>
    std::string to_string(const Args&... args) {
        std::ostringstream oss;
        (oss << ... << args);
        return oss.str();
    }

    void audio_devices(int input_device, int output_device);

    void color_inv(uint32_t color, float& r, float& g, float& b, float& a);

    inline std::string timestamp() {
        auto               now   = std::chrono::system_clock::now();
        auto               ms    = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        auto               timer = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&timer), "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    template<typename... Args>
    void error(const Args&... args) {
#if (UMGEBUNG_PRINT_ERRORS)
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cerr
            << timestamp() << " "
            << "UMG.ERROR   : "
            << os.str()
            << std::endl;
        std::flush(std::cerr);
#endif
    }

    template<typename... Args>
    void warning(const Args&... args) {
#if (UMGEBUNG_PRINT_WARNINGS)
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cerr
            << timestamp() << " "
            << "UMG.WARNING : "
            << os.str()
            << std::endl;
        std::flush(std::cerr);
#endif
    }

    bool exists(const std::string& file_path);

    std::string get_executable_location();
    std::string find_file_in_paths(const std::vector<std::string>& paths, const std::string& filename);
    std::string find_in_environment_path(const std::string& filename);

} // namespace umgebung