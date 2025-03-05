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
#include <SDL3/SDL.h>

#include "UmgebungConstants.h"

namespace umgebung {

    class PGraphics;
    class PImage {
    public:
        explicit PImage(const std::string& filename);
        PImage(int width, int height, int format);
        PImage();
        virtual ~PImage() = default;

        // virtual void bind();
        virtual void loadPixels(PGraphics* graphics);
        virtual void init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap);

        void updatePixels(PGraphics* graphics);
        void updatePixels(PGraphics* graphics, int x, int y, int w, int h);
        void update(PGraphics* graphics, uint32_t* pixel_data);
        void update(PGraphics* graphics, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y);
        void update(PGraphics* graphics, const float* pixel_data, int _width, int _height, int offset_x, int offset_y);

        // ReSharper disable once CppMemberFunctionMayBeConst
        void set(const uint16_t x, const uint16_t y, const uint32_t c) {
            if (x >= static_cast<uint16_t>(width) || y >= static_cast<uint16_t>(height)) {
                return;
            }
            pixels[y * static_cast<uint16_t>(width) + x] = c;
        }

        uint32_t get(const uint16_t x, const uint16_t y) const {
            if (x >= static_cast<uint16_t>(width) || y >= static_cast<uint16_t>(height)) {
                return 0;
            }
            const uint32_t c = pixels[y * static_cast<uint16_t>(width) + x];
            return c;
        }

        float        width;
        float        height;
        uint8_t      format;
        uint32_t*    pixels;
        int          texture_id  = TEXTURE_NOT_GENERATED;
        SDL_Texture* sdl_texture = nullptr;

    protected:
        void update_full_internal(PGraphics* graphics);
    };
} // namespace umgebung