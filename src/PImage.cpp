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

// TODO add `glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);`

#include "Umgebung.h"

#ifndef DISABLE_GRAPHICS

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#endif // DISABLE_GRAPHICS

using namespace umgebung;

PImage::PImage() {
    width    = 0;
    height   = 0;
    channels = 0;
}

PImage::PImage(const std::string& filename) {
    int _width    = 0;
    int _height   = 0;
    int _channels = 0;
#ifndef DISABLE_GRAPHICS
    data = stbi_load(filename.c_str(), &_width, &_height, &_channels, 0);
    if (data) {
        init(_width, _height, _channels, data);
    } else {
        std::cerr << "Failed to load image: " << filename << std::endl;
    }
    stbi_image_free(data); // @TODO maybe not release the data for later use?
#endif                     // DISABLE_GRAPHICS
}

void PImage::init(int _width, int _height, int _channels, unsigned char* _data) {
#ifndef DISABLE_GRAPHICS
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLint mFormat;
    if (_channels == 4) {
        mFormat = GL_RGBA;
    } else if (_channels == 3) {
        mFormat = GL_RGB;
    } else {
        std::cerr << "Unsupported image format, defaulting to RGBA forcing 4 color channels." << std::endl;
        mFormat   = GL_RGBA;
        _channels = 4;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, mFormat, _width, _height, 0, mFormat, GL_UNSIGNED_BYTE, _data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif // DISABLE_GRAPHICS
    width    = (float) _width;
    height   = (float) _height;
    channels = _channels;
}


void PImage::bind() {
#ifndef DISABLE_GRAPHICS
    glBindTexture(GL_TEXTURE_2D, textureID);
#endif // DISABLE_GRAPHICS
}

void PImage::update(float* _data, int _width, int _height, int offset_x, int offset_y) {
#ifndef DISABLE_GRAPHICS
    const int     length = _width * _height * channels;
    unsigned char mData[length];
    for (int i = 0; i < _width * _height * channels; ++i) {
        mData[i] = _data[i] * 255;
    }
    int mFormat;
    if (channels == 3) {
        mFormat = GL_RGB;
    } else if (channels == 4) {
        mFormat = GL_RGBA;
    } else {
        std::cerr << "Unsupported image format" << std::endl;
        mFormat = GL_RGB;
    }
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, offset_x, offset_y,
                    _width, _height,
                    mFormat,
                    GL_UNSIGNED_BYTE,
                    mData);
#endif // DISABLE_GRAPHICS
}

void PImage::update(float* _data) {
    update(_data, width, height, 0, 0);
}
