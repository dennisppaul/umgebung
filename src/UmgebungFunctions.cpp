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

#include <iomanip>
#include <string>
#include <random>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <vector>

#include "Umgebung.h"
#include "SimplexNoise.h"
#include "UmgebungFunctions.h"
#include "UmgebungFunctionsAdditional.h"

namespace umgebung {

    uint32_t color(const float gray) {
        return color(gray, gray, gray, 1);
    }

    uint32_t color(const float gray, const float alpha) {
        return color(gray, gray, gray, alpha);
    }

    uint32_t color(const float r, const float g, const float b) {
        return color(r, g, b, 1);
    }

    uint32_t color(const float r, const float g, const float b, const float a) {
        return static_cast<uint32_t>(a * 255) << 24 |
               static_cast<uint32_t>(b * 255) << 16 |
               static_cast<uint32_t>(g * 255) << 8 |
               static_cast<uint32_t>(r * 255);
    }

    uint32_t color_i(const uint32_t gray) {
        return gray << 24 |
               gray << 16 |
               gray << 8 |
               255;
    }

    uint32_t color_i(const uint32_t gray, const uint32_t alpha) {
        return gray << 24 |
               gray << 16 |
               gray << 8 |
               alpha;
    }

    uint32_t color_i(const uint32_t v1, const uint32_t v2, const uint32_t v3) {
        return 255 << 24 |
               v3 << 16 |
               v2 << 8 |
               v1;
    }

    uint32_t color_i(const uint32_t v1, const uint32_t v2, const uint32_t v3, const uint32_t alpha) {
        return alpha << 24 |
               v3 << 16 |
               v2 << 8 |
               v1;
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

    void exit() {
        // TODO handle exit
        console("TODO handle exit");
    }

    float degrees(const float radians) {
        return static_cast<float>(radians * 180.0f / M_PI);
    }

    float radians(const float degrees) {
        return M_PI * degrees / 180.0f;
    }

    std::string join(const std::vector<std::string>& strings, const std::string& separator) {
        std::string result;
        for (size_t i = 0; i < strings.size(); ++i) {
            result += strings[i];
            if (i < strings.size() - 1) {
                result += separator;
            }
        }
        return result;
    }

    float map(const float value,
              const float start0,
              const float stop0,
              const float start1,
              const float stop1) {
        const float a = value - start0;
        const float b = stop0 - start0;
        const float c = stop1 - start1;
        const float d = a / b;
        const float e = d * c;
        return e + start1;
    }

    std::vector<std::string> match(const std::string& text, const std::regex& regexp) {
        std::vector<std::string> groups;
        std::smatch              match;
        if (std::regex_search(text, match, regexp)) {
            for (size_t i = 1; i < match.size(); ++i) {
                groups.push_back(match[i]);
            }
        }
        return groups;
    }

    std::vector<std::vector<std::string>> matchAll(const std::string& text, const std::regex& regexp) {
        std::vector<std::vector<std::string>> matches;
        std::sregex_iterator                  begin(text.begin(), text.end(), regexp), end;
        for (auto i = begin; i != end; ++i) {
            std::vector<std::string> matchGroup;
            matchGroup.push_back(i->str()); // Full match

            for (size_t j = 1; j < i->size(); ++j) { // Capturing groups
                matchGroup.push_back((*i)[j]);
            }

            matches.push_back(matchGroup);
        }
        return matches;
    }

    std::string nf(const float num, const int digits) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(digits) << num;
        return out.str();
    }

    std::string nf(const float num, const int left, const int right) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(right);
        std::string numStr          = out.str();
        numStr                      = std::to_string(static_cast<int>(num)) + numStr.substr(numStr.find('.'));
        const int integerPartLength = static_cast<int>(numStr.find('.'));
        if (integerPartLength < left) {
            numStr.insert(0, left - integerPartLength, '0');
        }
        return numStr;
    }

    std::string nfc(const int num) {
        std::ostringstream out;
        out.imbue(std::locale("")); // Use system's locale for thousands separator
        out << num;
        return out.str();
    }

    std::string nfc(const float num, const int right) {
        std::ostringstream out;
        out.imbue(std::locale(""));
        out << std::fixed << std::setprecision(right) << num;
        return out.str();
    }

    std::string nfc(const float number) {
        std::stringstream ss;
        ss.imbue(std::locale(""));
        ss << std::fixed << number;
        return ss.str();
    }

    std::string nfp(const float num, const int digits) {
        std::ostringstream out;
        if (num >= 0) {
            out << "+";
        }
        out << std::fixed << std::setprecision(digits) << num;
        return out.str();
    }

    std::string nfp(const float num, const int left, const int right) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(right);
        if (num >= 0) {
            out << "+";
        }
        out << std::setw(left + right + 1) << std::setfill('0') << num;
        return out.str();
    }

    std::string nfs(const float num, const int left, const int right) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(right); // Ensure right decimal places

        if (num >= 0) {
            out << " "; // Space for positive numbers
        }

        out << std::setw(left + right + 1) << std::setfill('0') << num; // Ensure left digits

        return out.str();
    }

    std::string nfs(const float num, const int digits) {
        std::ostringstream out;
        if (num >= 0) {
            out << " ";
        }
        out << std::fixed << std::setprecision(digits) << num;
        return out.str();
    }

    std::string nfs(const int num, const int digits) {
        std::ostringstream out;
        if (num >= 0) {
            out << " ";
        }
        out << std::setw(digits) << std::setfill('0') << num;
        return out.str();
    }

    static unsigned int fRandomSeed = static_cast<unsigned int>(std::time(nullptr));
    static std::mt19937 gen(fRandomSeed); // Create a Mersenne Twister pseudo-random number generator with the specified seed

    float random(const float max) {
        return random(0, max);
    }

    float random(const float min, const float max) {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(gen);
    }

    void size(const int width, const int height) {
        if (is_initialized()) {
            warning("`size()` must be called before or within `settings()`.");
            return;
        }
        umgebung::width  = width;
        umgebung::height = height;
        // TODO create graphics object?
    }

    std::string nf(const int num, const int digits) {
        std::ostringstream oss;
        oss << std::setw(digits) << std::setfill('0') << num;
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

    std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> result;
        size_t                   start = 0, end;
        while ((end = str.find(delimiter, start)) != std::string::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
        }
        if (start < str.length()) {
            result.push_back(str.substr(start));
        }
        return result;
    }

    std::vector<std::string> splitTokens(const std::string& str, const std::string& tokens) {
        std::vector<std::string> result;
        size_t                   start = 0, end;
        while ((end = str.find_first_of(tokens, start)) != std::string::npos) {
            if (end != start) {
                result.push_back(str.substr(start, end - start));
            }
            start = end + 1;
        }
        if (start < str.length()) {
            result.push_back(str.substr(start));
        }
        return result;
    }

    std::string trim(const std::string& str) {
        const size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos) {
            return "";
        }
        const size_t last = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(first, (last - first + 1));
    }
} // namespace umgebung