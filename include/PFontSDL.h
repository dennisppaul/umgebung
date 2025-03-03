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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_opengl.h>

#include "Umgebung.h"

namespace umgebung {
    static bool TTF_initialized = false;

    class PFontSDL {
    public:
        explicit PFontSDL(const std::string& font_file) : font(nullptr) {

            /* Initialize the TTF library */
            if (!TTF_initialized) {
                if (!TTF_Init()) {
                    error("Couldn't initialize TTF: ", SDL_GetError());
                    return;
                }
                TTF_initialized = true;
            }

            console("font_file: ", font_file);
            font = TTF_OpenFont(font_file.c_str(), ptsize);

            if (font == nullptr) {
                error("Couldn't load ", ptsize, " pt font from '", font_file, "' ", SDL_GetError());
                return;
            }
            TTF_SetFontStyle(font, renderstyle);

            constexpr SDL_Color forecol = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
            // constexpr SDL_Color backcol = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};
            // if (true) {
            //     for (int i = 48; i < 123; i++) {
            //         SDL_Surface* glyph = NULL;
            //
            //         glyph = TTF_RenderGlyph_Shaded(font, i, forecol, backcol);
            //
            //         if (glyph) {
            //             char outname[64];
            //             console();
            //             sprintf(outname, "glyph-%d.bmp", i);
            //             SDL_SaveBMP(glyph, outname);
            //         }
            //     }
            // }
            const auto   message = "hello world";
            SDL_Surface* text    = TTF_RenderText_Blended(font, message, 0, forecol);
            if (text == nullptr) {
                error("Couldn't render text: ", SDL_GetError());
                TTF_CloseFont(font);
                return;
            }

            width  = text->w;
            height = text->h;
            console("font is generally ", TTF_GetFontHeight(font), " big, and string is ", text->h, " big");
            console("text size: ", width, "x", height);

            /* Convert the text into an OpenGL texture */

            glGetError();
            texture_id = SDL_GL_LoadTexture(text, texcoord);

            /* Make texture coordinates easy to understand */
            texMinX = texcoord[0];
            texMinY = texcoord[1];
            texMaxX = texcoord[2];
            texMaxY = texcoord[3];
            console("texcoord: ",
                    texcoord[0], ", ",
                    texcoord[1], ", ",
                    texcoord[2], ", ",
                    texcoord[3], ", ");

            /* We don't need the original text surface anymore */
            SDL_DestroySurface(text);
        }

        int     width{0};
        int     height{0};
        GLfloat texcoord[4];
        GLuint  texture_id{0};

    private:
        TTF_Font*          font;
        float              ptsize      = 48;
        TTF_FontStyleFlags renderstyle = TTF_STYLE_NORMAL;
        GLfloat            texMinX;
        GLfloat            texMinY;
        GLfloat            texMaxX;
        GLfloat            texMaxY;

        // static int power_of_two(const int input) {
        //     int value = 1;
        //
        //     while (value < input) {
        //         value <<= 1;
        //     }
        //     return value;
        // }

        static GLuint SDL_GL_LoadTexture(SDL_Surface* surface, GLfloat* texcoord) {
            GLuint        texture;
            int           w, h;
            SDL_Rect      area;
            Uint8         saved_alpha;
            SDL_BlendMode saved_mode;

            /* Use the surface width and height expanded to powers of 2 */
            w = (surface->w);
            h = (surface->h);
            // w           = power_of_two(surface->w);
            // h           = power_of_two(surface->h);
            texcoord[0] = 0.0f;                                 /* Min X */
            texcoord[1] = 0.0f;                                 /* Min Y */
            texcoord[2] = static_cast<GLfloat>(surface->w) / w; /* Max X */
            texcoord[3] = static_cast<GLfloat>(surface->h) / h; /* Max Y */

            SDL_Surface* image = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
            if (image == nullptr) {
                return 0;
            }

            /* Save the alpha blending attributes */
            SDL_GetSurfaceAlphaMod(surface, &saved_alpha);
            SDL_SetSurfaceAlphaMod(surface, 0xFF);
            SDL_GetSurfaceBlendMode(surface, &saved_mode);
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

            /* Copy the surface into the GL texture image */
            area.x = 0;
            area.y = 0;
            area.w = surface->w;
            area.h = surface->h;
            SDL_BlitSurface(surface, &area, image, &area);

            /* Restore the alpha blending attributes */
            SDL_SetSurfaceAlphaMod(surface, saved_alpha);
            SDL_SetSurfaceBlendMode(surface, saved_mode);

            /* Create an OpenGL texture for the image */
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);


            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         w, h,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         image->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            SDL_DestroySurface(image); /* No longer needed */

            return texture;
        }
    };
} // namespace umgebung