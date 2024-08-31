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
#include <cstdint>

namespace umgebung {
    class PImage {
    public:
        explicit PImage(const std::string& filename);
                 PImage(int _width, int _height, int _channels);
                 PImage();

        void bind() const;

        void update() const;
        void update(const uint32_t* pixel_data) const;
        void update(const uint32_t* pixel_data, int _width, int _height, int offset_x, int offset_y) const;
        void update(const float* pixel_data, int _width, int _height, int offset_x, int offset_y) const;

        void set(const int x, const int y, const uint32_t color) const {
            pixels[y * width + x] = color;
        }

        uint32_t get(const int x, const int y) const {
            return pixels[y * width + x];
        }

        int       width{};
        int       height{};
        int       channels{};
        uint32_t* pixels;

    protected:
        unsigned int textureID = -1;

        void init(const uint32_t* _data, int _width, int _height, int channels);
    };
} // namespace umgebung