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

#include <map>
#include <cstring>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Umgebung.h"

namespace umgebung {
    static bool TTF_initialized = false;

    class PFontSDL {
    public:
        struct Glyph {
            float               x{}, y{}, w{}, h{};     // Texture atlas coordinates
            int                 advanceX{};             // How much to move right
            int                 bearingX{}, bearingY{}; // Offset
            std::map<char, int> kerning;                // Kerning adjustments
        };

        std::map<char, Glyph> fontAtlas;
        GLuint                textureAtlas{};
        static constexpr int  ATLAS_WIDTH  = 1024;
        static constexpr int  ATLAS_HEIGHT = 1024;

        void create_atlas() {
            if (font == nullptr) {
                error("could not create font atlas");
                return;
            }

            constexpr SDL_Color forecol = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};

            SDL_Surface* atlasSurface = SDL_CreateSurface(ATLAS_WIDTH, ATLAS_HEIGHT, SDL_PIXELFORMAT_RGBA32);
            SDL_SetSurfaceBlendMode(atlasSurface, SDL_BLENDMODE_NONE);

            int x = 0, y = 0, maxHeight = 0;
            for (Uint32 c = 32; c < 127; ++c) {
                Uint8         saved_alpha;
                SDL_BlendMode saved_mode;

                SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(font, c, SDL_Color{255, 255, 255, 255});
                SDL_GetSurfaceAlphaMod(glyphSurface, &saved_alpha);
                SDL_SetSurfaceAlphaMod(glyphSurface, 0xFF);
                SDL_GetSurfaceBlendMode(glyphSurface, &saved_mode);
                SDL_SetSurfaceBlendMode(glyphSurface, SDL_BLENDMODE_NONE);
                if (!glyphSurface) {
                    continue;
                }

                if (x + glyphSurface->w > ATLAS_WIDTH) {
                    x = 0;
                    y += maxHeight + 2;
                    maxHeight = 0;
                }

                SDL_Rect dstRect = {x, y, glyphSurface->w, glyphSurface->h};
                SDL_BlitSurface(glyphSurface, nullptr, atlasSurface, &dstRect);

                SDL_SetSurfaceAlphaMod(glyphSurface, saved_alpha);
                SDL_SetSurfaceBlendMode(glyphSurface, saved_mode);

                Glyph g;
                g.x = static_cast<float>(x) / static_cast<float>(ATLAS_WIDTH);
                g.y = static_cast<float>(y) / static_cast<float>(ATLAS_HEIGHT);
                g.w = static_cast<float>(glyphSurface->w) / static_cast<float>(ATLAS_WIDTH);
                g.h = static_cast<float>(glyphSurface->h) / static_cast<float>(ATLAS_HEIGHT);
                TTF_GetGlyphMetrics(font, c, &g.bearingX, nullptr, &g.bearingY, nullptr, &g.advanceX);
                // console("bearing: ", c, " > ", g.bearingX, ", ", g.bearingY);

                // When building the atlas
                for (Uint32 k = 32; k < 127; ++k) {
                    int        kerningOffset;
                    const bool success = TTF_GetGlyphKerning(font, k, c, &kerningOffset);
                    if (success && kerningOffset != 0) {
                        g.kerning[k] = kerningOffset;
                        console("kerning pair: ", k, " > ", c, " = ", kerningOffset);
                    }
                }

                fontAtlas[c] = g;
                x += glyphSurface->w + 2;
                maxHeight = std::max(maxHeight, glyphSurface->h);

                SDL_DestroySurface(glyphSurface);
            }

            glGenTextures(1, &textureAtlas);
            glBindTexture(GL_TEXTURE_2D, textureAtlas);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         ATLAS_WIDTH,
                         ATLAS_HEIGHT,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         atlasSurface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            if (!TTF_GetFontKerning(font)) {
                console("Warning: Font does not support kerning!");
            }

            SDL_DestroySurface(atlasSurface);
        }

        struct Vertex {
            glm::vec3 position;
            glm::vec2 tex_coord;
            Vertex(const float x, const float y, const float u, const float v)
                : position(x, y, 0), tex_coord(u, v) {}
        };

        void render(PGraphics* g, const std::string& text, float x, const float y, const float scale, const float y_position_scale) {
            g->bind_texture(static_cast<int>(textureAtlas));

            std::vector<Vertex> vertices;
            char                prevChar = 0;

            for (char c: text) {
                if (fontAtlas.find(c) == fontAtlas.end()) {
                    continue;
                }

                Glyph& glyph = fontAtlas[c];

                // Apply kerning adjustment from the previous character
                const int kerningOffset = (prevChar && glyph.kerning.count(prevChar)) ? glyph.kerning[prevChar] : 0;
                x += static_cast<float>(kerningOffset) * scale;

                // float       xpos = x + static_cast<float>(glyph.bearingX) * scale*y_position_scale;
                // float       ypos = y - static_cast<float>(glyph.bearingY) * scale*y_position_scale;
                float       xpos = x;
                float       ypos = y;
                const float w    = glyph.w * ATLAS_WIDTH * scale;
                const float h    = glyph.h * ATLAS_HEIGHT * scale;
                float       u0 = glyph.x, v0 = glyph.y;
                float       u1 = glyph.x + glyph.w, v1 = glyph.y + glyph.h;

                vertices.emplace_back(xpos, ypos + h, u0, v1);
                vertices.emplace_back(xpos + w, ypos, u1, v0);
                vertices.emplace_back(xpos, ypos, u0, v0);

                vertices.emplace_back(xpos, ypos + h, u0, v1);
                vertices.emplace_back(xpos + w, ypos + h, u1, v1);
                vertices.emplace_back(xpos + w, ypos, u1, v0);

                x += static_cast<float>(glyph.advanceX) * scale;
                prevChar = c;
            }

            g->beginShape(TRIANGLES);
            for (const auto v: vertices) {
                g->vertex(v.position.x,
                          v.position.y,
                          v.position.z,
                          v.tex_coord.x,
                          v.tex_coord.y);
            }
            g->endShape();

            g->unbind_texture();
        }

        /* ----------------------------------------------------------------------------------------------------- */
        /* ----------------------------------------------------------------------------------------------------- */
        /* ----------------------------------------------------------------------------------------------------- */
        /* ----------------------------------------------------------------------------------------------------- */
        /* ----------------------------------------------------------------------------------------------------- */
        /* ----------------------------------------------------------------------------------------------------- */

        explicit PFontSDL(const std::string& font_file, const float size) : font(nullptr) {

            /* Initialize the TTF library */
            if (!TTF_initialized) {
                if (!TTF_Init()) {
                    error("Couldn't initialize TTF: ", SDL_GetError());
                    return;
                }
                TTF_initialized = true;
            }

            console("font_file: ", font_file);
            ptsize = size;
            font   = TTF_OpenFont(font_file.c_str(), ptsize);

            if (font == nullptr) {
                error("Couldn't load ", ptsize, " pt font from '", font_file, "' ", SDL_GetError());
                return;
            }

            if (TTF_FontIsFixedWidth(font)) {
                console("font is fixed width");
            }

            TTF_SetFontKerning(font, true);
            TTF_SetFontStyle(font, renderstyle);

            create_atlas();

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
            // const auto   message = "Hamburgefonts";
            const auto   message = "AVTAWaToVAWeYoyo Hamburgefonts";
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
        GLfloat texcoord[4]{};
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
            texcoord[0] = 0.0f;                                                     /* Min X */
            texcoord[1] = 0.0f;                                                     /* Min Y */
            texcoord[2] = static_cast<GLfloat>(surface->w) / static_cast<float>(w); /* Max X */
            texcoord[3] = static_cast<GLfloat>(surface->h) / static_cast<float>(h); /* Max Y */

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