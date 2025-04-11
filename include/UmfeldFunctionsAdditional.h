/*
 * Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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

#include <SDL3/SDL.h>

#include <cstdint>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include "UmfeldDefines.h"
#include "UmfeldConstants.h"

namespace umfeld {

    class PAudio;
    struct AudioUnitInfo;
    class LibraryListener;
    struct Vertex;
    template<typename T>
    class SamplerT;
    using Sampler = SamplerT<float>;

    bool                     begins_with(const std::string& str, const std::string& prefix);
    bool                     ends_with(const std::string& str, const std::string& suffix);
    void                     color_inv(uint32_t color, float& r, float& g, float& b, float& a);
    bool                     exists(const std::string& file_path);
    std::string              find_file_in_paths(const std::vector<std::string>& paths, const std::string& filename);
    std::string              find_in_environment_path(const std::string& filename);
    std::string              get_executable_location();
    std::vector<std::string> get_files(const std::string& directory, const std::string& extension = "");
    int                      get_int_from_argument(const std::string& argument);
    std::string              get_string_from_argument(const std::string& argument);
    std::string              timestamp();
    void                     audio(int input_channels  = DEFAULT_INPUT_CHANNELS,
                                   int output_channels = DEFAULT_OUTPUT_CHANNELS,
                                   int sample_rate     = DEFAULT_SAMPLE_RATE,
                                   int buffer_size     = DEFAULT_AUDIO_BUFFER_SIZE,
                                   int input_device    = DEFAULT_AUDIO_DEVICE,
                                   int output_device   = DEFAULT_AUDIO_DEVICE);
    void                     audio(int                input_channels,
                                   int                output_channels,
                                   int                sample_rate,
                                   int                buffer_size,
                                   const std::string& input_device_name,
                                   const std::string& output_device_name);
    void                     audio(const AudioUnitInfo& info);
    void                     audio_start(PAudio* device = nullptr);
    void                     audio_stop(PAudio* device = nullptr);
    bool                     is_initialized();
    std::string              get_window_title(); // TODO maybe add setter
    void                     set_frame_rate(float fps);
    SDL_WindowFlags          get_SDL_WindowFlags(SDL_WindowFlags& flags);
    void                     register_library(LibraryListener* listener);         /* implemented in subsystems */
    void                     unregister_library(const LibraryListener* listener); /* implemented in subsystems */
    void                     handle_events_in_loop(bool events_in_loop);          /* implemented in subsystems */
    std::vector<Vertex>      loadOBJ(const std::string& filename, bool material = true);
    Sampler*                 loadSample(const std::string& filename);
    PAudio*                  createAudio(const AudioUnitInfo* device_info);

    /* --- templated functions --- */

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

    template<typename... Args>
    void error(const Args&... args) {
#if (UMFELD_PRINT_ERRORS)
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cerr
            << timestamp() << " "
            << "UMFELD.ERROR   : "
            << os.str()
            << std::endl;
        std::flush(std::cerr);
#endif
    }

    template<typename... Args>
    void warning(const Args&... args) {
#if (UMFELD_PRINT_WARNINGS)
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cerr
            << timestamp() << " "
            << "UMFELD.WARNING : "
            << os.str()
            << std::endl;
        std::flush(std::cerr);
#endif
    }

    template<typename... Args>
    void console(const Args&... args) {
#if (UMFELD_PRINT_CONSOLE)
        std::ostringstream os;
        ((os << to_printable(args)), ...);
        std::cout
            << timestamp() << " "
            << "UMFELD.CONSOLE : "
            << os.str()
            << std::endl;
        std::flush(std::cout);
#endif
    }

    inline std::string format_label(const std::string& label, const size_t width) {
        if (label.length() >= width) {
            return label + " : "; // Ensure spacing even if label is too long
        }
        return label + std::string(width - label.length(), ' ') + " : ";
    }

    inline std::string separator(const bool equal_sign = true, const std::size_t length = 80) {
        return std::string(length, equal_sign ? '=' : '-');
    }

    using namespace std::chrono;

    template<typename Func, typename... Args>
    double time_function_ms(Func&& func, Args&&... args) {
        const auto start = high_resolution_clock::now();

        // Call the function with forwarded arguments
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);

        const auto                         end     = high_resolution_clock::now();
        const duration<double, std::milli> elapsed = end - start;
        return elapsed.count(); // in milliseconds
    }
} // namespace umfeld