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

#include <string>
#include <iostream>
#include <sstream>

// TODO check if these functions should be moved to PApplet?

void exit();

void audio_devices(int input_device, int output_device);

/* static functions for PApplet */

float random(float min, float max);

std::string nf(int number, int width);

float noise(float x);

float noise(float x, float y);

float noise(float x, float y, float z);

float radians(float degrees);

float degrees(float radians);

template<typename... Args>
void println(const Args &... args) {
    std::ostringstream os;
    (os << ... << args);
    std::cout << os.str() << std::endl;
}

template<typename... Args>
std::string to_string(const Args &... args) {
    std::ostringstream oss;
    (oss << ... << args);
    return oss.str();
}
