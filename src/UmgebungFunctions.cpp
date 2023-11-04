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
#include "Umgebung.h"
#include "SimplexNoise.h"

static SimplexNoise fSimplexNoise;

void audio_devices(int input_device, int output_device) {
    audio_input_device = input_device;
    audio_output_device = output_device;
}

static unsigned int fRandomSeed = static_cast<unsigned int>(std::time(nullptr));
static std::mt19937 gen(fRandomSeed); // Create a Mersenne Twister pseudo-random number generator with the specified seed

float random(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(gen);
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
