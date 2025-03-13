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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

#include "UmgebungFunctionsAdditional.h"
#include "PAudio.h"

namespace umgebung {

    uint32_t color(float gray);
    uint32_t color(float gray, float alpha);
    uint32_t color(float v1, float v2, float v3);              // TODO correct parameter name
    uint32_t color(float v1, float v2, float v3, float alpha); // TODO correct parameter name

    uint32_t color_i(uint32_t gray);
    uint32_t color_i(uint32_t gray, uint32_t alpha);
    uint32_t color_i(uint32_t v1, uint32_t v2, uint32_t v3);
    uint32_t color_i(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t alpha);

    float red(uint32_t color);
    float green(uint32_t color);
    float blue(uint32_t color);
    float alpha(uint32_t color);

    float degrees(float radians);
    float radians(float degrees);

    void exit();

    std::string join(const std::vector<std::string>& strings, const std::string& separator);

    float map(float value, float start0, float stop0, float start1, float stop1);

    std::vector<std::string>              match(const std::string& text, const std::regex& regexp);
    std::vector<std::vector<std::string>> matchAll(const std::string& text, const std::regex& regexp);

    std::string nf(float num, int digits = 2);
    std::string nf(float num, int left, int right);
    std::string nf(int num, int digits = 2);
    std::string nfc(int num);
    std::string nfc(float num, int right);
    std::string nfp(float num, int digits = 2);
    std::string nfp(float num, int left, int right);
    std::string nfs(float num, int left, int right);
    std::string nfs(float num, int digits = 2);
    std::string nfs(int num, int digits = 2);

    float noise(float x);
    float noise(float x, float y);
    float noise(float x, float y, float z);
    void  noiseSeed(int seed);

#define FLUSH_PRINT

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

    float random(float max);
    float random(float min, float max);

    void size(int width, int height);

    std::string sketchPath();

    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    std::vector<std::string> splitTokens(const std::string& str, const std::string& tokens);

    std::string trim(const std::string& str);

    PGraphics* createGraphics();
    PAudio*    createAudio(const AudioUnitInfo* device_info);
} // namespace umgebung
