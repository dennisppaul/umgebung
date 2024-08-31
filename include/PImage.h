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
#include <string>

namespace umgebung {
    class PImage {
    public:
        explicit PImage(const std::string& filename);
                 PImage(int _width, int _height, int _channels);
                 PImage();

        void bind() const;

        void  update() const;
        void  update(const float* data) const; // maybe change to 32bit int format
        void  update(const float* _data, int _width, int _height, int offset_x, int offset_y) const;
        void  update(const unsigned char* _data, int _width, int _height, int offset_x, int offset_y) const;
        float width{};
        float height{};
        int   channels{};

    protected:
        uint32_t*    data{};
        unsigned int textureID{};

        void init(int width, int height, int channels, const unsigned char* _data);
    };
} // namespace umgebung