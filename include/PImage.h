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

#include <iostream>
#include <cstdint>

namespace umgebung {
    class PImage {
    public:
        virtual ~PImage() = default;
        explicit PImage(const std::string& filename);
                 PImage(int width, int height, int format);
                 PImage();

        virtual void bind() const;
        virtual void loadPixels() const;
        virtual void init(uint32_t* pixels, int width, int height, int format);

        void updatePixels() const;
        void updatePixels(int x, int y, int w, int h) const;
        void update(const uint32_t* pixel_data) const;
        void update(const uint32_t* pixel_data, int _width, int _height, int offset_x, int offset_y) const;
        void update(const float* pixel_data, int _width, int _height, int offset_x, int offset_y) const;

        void set(const uint16_t x, const uint16_t y, const uint32_t c) const {
            if (x >= width || y >= height) {
                return;
            }
            pixels[y * width + x] = c;
        }

        [[nodiscard]] uint32_t get(const uint16_t x, const uint16_t y) const {
            if (x >= width || y >= height) {
                return 0;
            }
            const uint32_t c = pixels[y * width + x];
            return c;
        }

        uint16_t  width;
        uint16_t  height;
        uint8_t   format;
        uint32_t* pixels;

    protected:
        unsigned int textureID = -1;

        void update_full_internal() const;
    };
} // namespace umgebung