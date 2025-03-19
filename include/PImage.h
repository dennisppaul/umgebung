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
        void update(PGraphics* graphics, const uint32_t* pixel_data);
        void update(PGraphics* graphics, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y);
        void update(PGraphics* graphics, const float* pixel_data, int width, int height, int offset_x, int offset_y);

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

    public:
        static PImage convert_SDL_Surface_to_PImage(SDL_Surface* surface) {
            if (!surface) {
                return PImage(); // Return empty image if surface is null
            }

            const int     width  = surface->w;
            const int     height = surface->h;
            constexpr int format = SDL_PIXELFORMAT_RGBA8888; // Assuming default RGBA format

            PImage image(width, height, format);

            // Convert surface to the correct format if necessary
            SDL_Surface* convertedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
            if (!convertedSurface) {
                return PImage(); // Conversion failed, return empty image
            }

            // Copy pixels
            image.pixels = new uint32_t[width * height];
            std::memcpy(image.pixels, convertedSurface->pixels, width * height * sizeof(uint32_t));

            SDL_DestroySurface(convertedSurface); // Clean up converted surface

            return image;
        }

        static SDL_Surface* convert_PImage_to_SDL_Surface(const PImage& image) {
            if (!image.pixels || image.width <= 0 || image.height <= 0) {
                return nullptr;
            }

            // Create SDL surface with the correct format
            SDL_Surface* surface = SDL_CreateSurface(image.width, image.height, SDL_PIXELFORMAT_RGBA8888);
            if (!surface) {
                return nullptr;
            }

            // Copy pixel data into the surface
            std::memcpy(surface->pixels, image.pixels, image.width * image.height * sizeof(uint32_t));

            return surface;
        }
    };
} // namespace umgebung