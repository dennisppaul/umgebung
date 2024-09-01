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

#include "Umgebung.h"
#include "PImage.h"

#ifndef DISABLE_GRAPHICS

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#endif // DISABLE_GRAPHICS

using namespace umgebung;

#define RGBA(r, g, b, a) (((uint32_t) (a) << 24) | ((uint32_t) (b) << 16) | ((uint32_t) (g) << 8) | ((uint32_t) (r)))

PImage::PImage() : width(0),
                   height(0),
                   format(0),
                   pixels(nullptr) {
    // note that PImage is not initialized with any data
}

PImage::PImage(const int width, const int height, const int format) : width(width),
                                                                      height(height),
                                                                      format(format),
                                                                      pixels(nullptr) {
    const int length = width * height;
    if (length <= 0) {
        return;
    }
    pixels = new uint32_t[length]{0x00000000};
    init(pixels, width, height, format);
}

PImage::PImage(const std::string& filename) : width(0),
                                              height(0),
                                              format(0),
                                              pixels(nullptr) {
#ifndef DISABLE_GRAPHICS
    int            _width    = 0;
    int            _height   = 0;
    int            _channels = 0;
    unsigned char* data      = stbi_load(filename.c_str(), &_width, &_height, &_channels, 0);
    if (data) {
        if (_channels != 4 && _channels != 3) {
            std::cerr << "Unsupported image format, defaulting to RGBA forcing 4 color channels." << std::endl;
            _channels = 4;
        }
        pixels = new uint32_t[_width * _height];
        for (int i = 0; i < _width * _height; ++i) {
            const int j = i * _channels;
            if (_channels == 4) {
                pixels[i] = RGBA(data[j + 0], data[j + 1], data[j + 2], data[j + 3]);
            } else if (_channels == 3) {
                pixels[i] = RGBA(data[j + 0], data[j + 1], data[j + 2], 0xFF);
            }
        }

        if (_channels == 3) {
            std::cout << "Note that RGB is converted to RGBA and number of channels is changed to 4" << std::endl;
            _channels = 4;
        }
        init(pixels, _width, _height, _channels);
    } else {
        std::cerr << "Failed to load image: " << filename << std::endl;
    }
    stbi_image_free(data);
#endif // DISABLE_GRAPHICS
}

void PImage::loadPixels() const {
#ifndef DISABLE_GRAPHICS
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#endif // DISABLE_GRAPHICS
}

void PImage::init(uint32_t* pixels, const int width, const int height, const int format) {
#ifndef DISABLE_GRAPHICS
    if (pixels == nullptr) {
        std::cerr << "unitialized pixel buffer" << std::endl;
        return;
    }
    this->pixels = pixels;
    this->width  = width;
    this->height = height;
    this->format = format;
    if (format != 4) {
        std::cerr << "Unsupported image format, defaulting to RGBA forcing 4 color channels." << std::endl;
        this->format = 4;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    constexpr GLint mFormat = GL_RGBA; // internal format is always RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, mFormat, width, height, 0, mFormat, GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif // DISABLE_GRAPHICS
}

void PImage::bind() const {
#ifndef DISABLE_GRAPHICS
    glBindTexture(GL_TEXTURE_2D, textureID);
#endif // DISABLE_GRAPHICS
}

static float clamp(const float x) {
    return (x < 0 ? 0 : x) > 1 ? 1 : x;
}

void PImage::update(const float* pixel_data,
                    const int    _width,
                    const int    _height,
                    const int    offset_x,
                    const int    offset_y) const {
    /* NOTE pixel data must be 4 times the length of pixels */
    const int length = _width * _height;
    uint32_t  mPixels[length];
    for (int i = 0; i < _width * _height; ++i) {
        const int j = i * 4;
        mPixels[i]  = RGBA(clamp(pixel_data[j + 0]) * 255,
                           clamp(pixel_data[j + 1]) * 255,
                           clamp(pixel_data[j + 2]) * 255,
                           clamp(pixel_data[j + 3]) * 255);
    }
    update(mPixels, _width, _height, offset_x, offset_y);
}

void PImage::update(const uint32_t* pixel_data,
                    const int       _width,
                    const int       _height,
                    const int       offset_x,
                    const int       offset_y) const {
#ifndef DISABLE_GRAPHICS
    if (!pixel_data) {
        std::cerr << "invalid pixel data" << std::endl;
        return;
    }
    if (!pixels) {
        std::cerr << "pixel array not initialized" << std::endl;
        return;
    }

    /* copy subregion `pixel_data` with offset into `pixels` */

    // Ensure the region is within bounds
    if (offset_x < 0 || offset_y < 0 ||
        offset_x + _width > width ||
        offset_y + _height > height) {
        std::cerr << "subregion is out of bounds" << std::endl;
        return;
    }

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            const int src_index  = y * _width + x;
            const int dest_index = (offset_y + y) * width + (offset_x + x);
            pixels[dest_index]   = pixel_data[src_index];
        }
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    constexpr GLint mFormat = GL_RGBA; // internal format is always RGBA
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, offset_x, offset_y,
                    _width, _height,
                    mFormat,
                    GL_UNSIGNED_BYTE,
                    pixel_data);
#endif // DISABLE_GRAPHICS
}

void PImage::update(const uint32_t* pixel_data) const {
    update(pixel_data, width, height, 0, 0);
}

void PImage::updatePixels() const {
    update(pixels, width, height, 0, 0);
}

void PImage::updatePixels(const int x, const int y, const int w, const int h) const {
    if (!pixels) {
        std::cerr << "pixel array not initialized" << std::endl;
        return;
    }

    if (x < 0 || y < 0 || x + w > width || y + h > height) {
        return;
    }

    const int length  = w * h;
    auto*     mPixels = new uint32_t[length];
    for (int i = 0; i < length; ++i) {
        const int src_index = (y + i / w) * width + (x + i % w);
        mPixels[i]          = pixels[src_index];
    }
    update(mPixels, w, h, x, y);
    delete[] mPixels;
}
