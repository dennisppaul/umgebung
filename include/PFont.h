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

// TODO implement
//    ## Loading & Displaying
//    createFont() :: Dynamically converts a font to the format used by Processing
//    loadFont() :: Loads a font into a variable of type PFont
//    textFont() :: Sets the current font that will be drawn with the text() function
//    text() :: Draws text to the screen
//    ## Metrics
//    textAscent() :: Returns ascent of the current font at its current size
//    textDescent() :: Returns descent of the current font at its current size
//    ## Attributes
//    textAlign() :: Sets the current alignment for drawing text
//    textLeading() :: Sets the spacing between lines of text in units of pixels
//    textMode() :: Sets the way text draws to the screen
//    //textSize() :: Sets the current font size
//    //textWidth() :: Calculates and returns the width of any character or text string

// #define PFONT_DEBUG_FONT
// #define PFONT_INCLUDE_OPENGL

#include <string>
#include <codecvt>

#include <algorithm>
#include <vector>
#include <unordered_map>

#ifdef PFONT_INCLUDE_OPENGL
#include <GL/glew.h>
#endif // PFONT_INCLUDE_OPENGL

#include "harfbuzz/hb.h"
#include "harfbuzz/hb-ft.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "UmgebungFunctionsAdditional.h"
#include "PImage.h"

namespace umgebung {
    struct TexturedQuad {
        float x0, y0, u0, v0; // Top-left
        float x1, y1, u1, v1; // Top-right
        float x2, y2, u2, v2; // Bottom-right
        float x3, y3, u3, v3; // Bottom-left

        TexturedQuad(const float _x0, const float _y0, const float _u0, const float _v0,
                     const float _x1, const float _y1, const float _u1, const float _v1,
                     const float _x2, const float _y2, const float _u2, const float _v2,
                     const float _x3, const float _y3, const float _u3, const float _v3)
            : x0(_x0), y0(_y0), u0(_u0), v0(_v0),
              x1(_x1), y1(_y1), u1(_u1), v1(_v1),
              x2(_x2), y2(_y2), u2(_u2), v2(_v2),
              x3(_x3), y3(_y3), u3(_u3), v3(_v3) {}
    };

    class PFont final : public PImage {
        const std::string character_atlas_default = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()[]{}-_=+;:'\",<.>/?`~";

    public:
        explicit PFont(const std::string& font_filepath,
                       const int          font_size,
                       const float        pixelDensity = 1) : font_size(font_size) {
            const std::string character_atlas = character_atlas_default;
            (void) pixelDensity; // TODO implement pixel density

            /* init freetype and font struct */
            const char* filepath_c = font_filepath.c_str();
            FT_Init_FreeType(&freetype);
            font         = new FontData();
            font->buffer = hb_buffer_create();
            FT_New_Face(freetype, filepath_c, 0, &font->face);
            FT_Set_Pixel_Sizes(font->face, 0, font_size);
            font->hb_font = hb_ft_font_create(font->face, nullptr);

            create_font_atlas(*font, character_atlas);

            console("PFont / implement pixel density");

            // texture_id = create_font_texture(*font); // NOTE this is done in PGraphics
            width  = static_cast<float>(font->atlas_width);
            height = static_cast<float>(font->atlas_height);
            format = 4;
            pixels = new uint32_t[static_cast<int>(width * height)];
            copy_atlas_to_rgba(*font, reinterpret_cast<unsigned char*>(pixels));

            console("PFont     :  created atlas");
            console("size      : ", width, "×", height);
            console("texture id: ", texture_id == TEXTURE_NOT_GENERATED ? "(NOT YET GENERATED)" : to_string(texture_id));
#ifdef PFONT_DEBUG_FONT
            DEBUG_save_font_atlas(*font, font_filepath + "--font_atlas.png");
            DEBUG_save_text(*font, "AVTAWaToVAWeYoyo Hamburgefonts", font_filepath + "--text.png");
#endif //PFONT_DEBUG_FONT
        }

        static constexpr int atlas_pixel_width       = 512;
        static constexpr int atlas_character_padding = 2;
        const int            font_size;

        PImage* create_image(const std::string& text) const {
            error("PImage / implement `create_image`: ", text);
            return nullptr;
        }

        void draw(PGraphics* g, const std::string& text, const float x, const float y, const float z = 0) {
            if (font == nullptr) {
                return;
            }
            if (g == nullptr) {
                return;
            }

            generate_text_quads(*font, text, text_quads);

            g->pushMatrix();
            g->translate(x, y, z);
            g->texture(this);
            g->beginShape(TRIANGLES);
            for (const auto q: text_quads) {
                g->vertex(q.x0, q.y0, 0, q.u0, q.v0);
                g->vertex(q.x1, q.y1, 0, q.u1, q.v1);
                g->vertex(q.x2, q.y2, 0, q.u2, q.v2);

                g->vertex(q.x3, q.y3, 0, q.u3, q.v3);
                g->vertex(q.x0, q.y0, 0, q.u0, q.v0);
                g->vertex(q.x2, q.y2, 0, q.u2, q.v2);
            }
            g->endShape(CLOSE);
            g->popMatrix();
        }

        float textWidth(const char* text) const { // TODO move to std::string
            (void) text;
            error("PFont implement `textWidth`");
            return 0.0f;
        }

        void textSize(const float textSize) const {
            (void) textSize;
            error("PFont implement `textSize`");
        }

        ~PFont() override {
            hb_buffer_destroy(font->buffer);
            hb_font_destroy(font->hb_font);
            FT_Done_Face(font->face);
            delete font;
            FT_Done_FreeType(freetype);
            delete pixels;
        }

    private:
        struct Glyph {
            std::vector<unsigned char> bitmap;
            int                        width{};
            int                        height{};
            int                        left{};
            int                        top{};
            int                        advance{};
            int                        atlas_x{};
            int                        atlas_y{};
        };

        struct FontData {
            std::unordered_map<uint32_t, Glyph> glyphs;
            int                                 ascent{};
            int                                 descent{};
            int                                 line_gap{};
            int                                 atlas_width{};
            int                                 atlas_height{};
            std::vector<unsigned char>          atlas;
            FT_Face                             face{nullptr};
            hb_font_t*                          hb_font{nullptr};
            hb_buffer_t*                        buffer{nullptr};
        };

        std::vector<TexturedQuad> text_quads;
        // GLuint                    font_texture{0};
        FontData*  font{nullptr};
        FT_Library freetype{nullptr};

#ifdef PFONT_DEBUG_FONT
        void DEBUG_save_font_atlas(const FontData& font, const std::string& output_path) const;
        void DEBUG_save_text(const FontData& font, const char* text, const std::string& outputfile) const;
#endif //PFONT_DEBUG_FONT

        // static std::u16string utf8_to_utf16(std::string& utf8) {
        //     std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        //     return convert.from_bytes(utf8);
        // }

        void create_font_atlas(FontData& font, const std::string& characters_in_atlas) const {
            if (font.face == nullptr || font.hb_font == nullptr) {
                error("font data not intizialized");
                return;
            }

            font.ascent   = static_cast<int>(font.face->size->metrics.ascender >> 6);
            font.descent  = static_cast<int>(-font.face->size->metrics.descender >> 6);
            font.line_gap = static_cast<int>(font.face->size->metrics.height >> 6);

            hb_buffer_clear_contents(font.buffer);
            // std::u16string s16 = utf8_to_utf16(characters_in_atlas);
            // std::vector<uint16_t> utf16_vec(s16.begin(), s16.end());
            // utf16_vec.push_back(0);  // Add null terminator
            // const uint16_t* raw_utf16 = utf16_vec.data();
            // hb_buffer_add_utf16(font.buffer, raw_utf16, -1, 0, -1);
            hb_buffer_add_utf8(font.buffer, characters_in_atlas.c_str(), -1, 0, -1);
            hb_buffer_set_direction(font.buffer, HB_DIRECTION_LTR);
            hb_buffer_set_script(font.buffer, HB_SCRIPT_LATIN);
            hb_buffer_set_language(font.buffer, hb_language_from_string("en", 2));

            hb_shape(font.hb_font, font.buffer, nullptr, 0);

            unsigned int           glyph_count;
            const hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(font.buffer, &glyph_count);

            int           current_x      = 0;
            int           current_y      = 0;
            int           max_row_height = 0;
            constexpr int atlas_width    = atlas_pixel_width;
            int           atlas_height   = 0;

            for (unsigned int i = 0; i < glyph_count; i++) {
                FT_Load_Glyph(font.face, glyph_info[i].codepoint, FT_LOAD_RENDER);
                const FT_GlyphSlot glyph = font.face->glyph;

                if (font.glyphs.find(glyph_info[i].codepoint) == font.glyphs.end()) {
                    Glyph g;
                    g.width   = static_cast<int>(glyph->bitmap.width);
                    g.height  = static_cast<int>(glyph->bitmap.rows);
                    g.left    = glyph->bitmap_left;
                    g.top     = glyph->bitmap_top;
                    g.advance = static_cast<int>(glyph->advance.x) >> 6;

                    // Ensure correct copying of bitmap buffer
                    if (glyph->bitmap.buffer) {
                        g.bitmap.assign(glyph->bitmap.buffer, glyph->bitmap.buffer + (g.width * g.height));
                    } else {
                        g.bitmap.assign(g.width * g.height, 0);
                    }

                    if (current_x + g.width + atlas_character_padding > atlas_width) {
                        current_x = 0;
                        current_y += max_row_height + atlas_character_padding;
                        max_row_height = 0;
                    }

                    g.atlas_x = current_x;
                    g.atlas_y = current_y;

                    current_x += g.width + atlas_character_padding;
                    max_row_height = std::max(max_row_height, g.height);

                    font.glyphs[glyph_info[i].codepoint] = g;
                }
            }

            atlas_height      = current_y + max_row_height; // Fix last row height
            font.atlas_width  = atlas_width;
            font.atlas_height = atlas_height;
            font.atlas.resize(atlas_width * atlas_height, 0);

            for (const auto& pair: font.glyphs) {
                const Glyph& g = pair.second;
                for (int y = 0; y < g.height; y++) {
                    for (int x = 0; x < g.width; x++) {
                        const int atlas_x                           = g.atlas_x + x;
                        const int atlas_y                           = g.atlas_y + y;
                        font.atlas[atlas_y * atlas_width + atlas_x] = g.bitmap[y * g.width + x];
                    }
                }
            }
        }

        void copy_atlas_to_rgba(const FontData& font, unsigned char* atlas_rgba) const {
            for (int y = 0; y < font.atlas_height; y++) {
                for (int x = 0; x < font.atlas_width; x++) {
                    const unsigned char val = font.atlas[y * font.atlas_width + x];
                    const int           idx = (y * font.atlas_width + x) * 4;
                    // Store grayscale value into RGBA format (transparent text on white fond)
                    atlas_rgba[idx + 0] = 255; // R
                    atlas_rgba[idx + 1] = 255; // G
                    atlas_rgba[idx + 2] = 255; // B
                    atlas_rgba[idx + 3] = val; // A (fully opaque)
                }
            }
        }

#ifdef PFONT_INCLUDE_OPENGL

        // TODO move to `UmgebungConstantsOpenGL.h`
        static constexpr GLint UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE    = GL_UNSIGNED_INT_8_8_8_8_REV;
        static constexpr GLint UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT = GL_RGBA;

        /**
         * @deprecated this is handle by PGraphics
         * @param font
         * @return
         */
        GLuint create_font_texture(const FontData& font) const {
            // TODO this does happen in OpenGL context ... as for PImage
            std::vector<unsigned char> atlas_rgba(font.atlas_width * font.atlas_height * 4, 255);
            for (int y = 0; y < font.atlas_height; y++) {
                for (int x = 0; x < font.atlas_width; x++) {
                    const unsigned char val = font.atlas[y * font.atlas_width + x];
                    const int           idx = (y * font.atlas_width + x) * 4;
                    // Store grayscale value into RGBA format (transparent text on white fond)
                    atlas_rgba[idx + 0] = 255; // R
                    atlas_rgba[idx + 1] = 255; // G
                    atlas_rgba[idx + 2] = 255; // B
                    atlas_rgba[idx + 3] = val; // A (fully opaque)
                }
            }

            GLuint texture_id;
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);

            // Use linear filtering for smooth text rendering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Clamp to edges to avoid bleeding artifacts
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Upload the atlas (single-channel grayscale)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                         font.atlas_width, font.atlas_height,
                         0,
                         UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                         UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE,
                         // font.atlas.data());
                         atlas_rgba.data());

            // Unbind for safety
            glBindTexture(GL_TEXTURE_2D, 0);

            return texture_id;
        }
#endif // PFONT_INCLUDE_OPENGL

        static void generate_text_quads(const FontData&            font,
                                        const std::string&         text,
                                        std::vector<TexturedQuad>& quads) {
            quads.clear();
            hb_buffer_clear_contents(font.buffer);

            hb_buffer_add_utf8(font.buffer, text.c_str(), -1, 0, -1);
            hb_buffer_set_direction(font.buffer, HB_DIRECTION_LTR);
            hb_buffer_set_script(font.buffer, HB_SCRIPT_LATIN);
            hb_buffer_set_language(font.buffer, hb_language_from_string("en", 2));

            hb_shape(font.hb_font, font.buffer, nullptr, 0);

            unsigned int               glyph_count;
            const hb_glyph_info_t*     glyph_info = hb_buffer_get_glyph_infos(font.buffer, &glyph_count);
            const hb_glyph_position_t* glyph_pos  = hb_buffer_get_glyph_positions(font.buffer, &glyph_count);

            // Dynamically decide whether to preallocate
            if (text.length() > 100) { // Only preallocate if the text is long
                size_t estimated_quads = 0;
                for (unsigned int i = 0; i < glyph_count; i++) {
                    uint32_t glyph_id = glyph_info[i].codepoint;
                    if (glyph_id != ' ' && font.glyphs.find(glyph_id) != font.glyphs.end()) {
                        estimated_quads++; // Only count valid glyphs (not spaces)
                    }
                }
                quads.reserve(estimated_quads);
            }

            float      x = 0.0f;
            const auto y = static_cast<float>(font.ascent); // Baseline position

            for (unsigned int i = 0; i < glyph_count; i++) {
                uint32_t glyph_id = glyph_info[i].codepoint;

                auto it = font.glyphs.find(glyph_id);
                if (it == font.glyphs.end()) {
                    // Handle spaces explicitly
                    if (glyph_id == ' ') {
                        x += static_cast<float>(glyph_pos[i].x_advance >> 6); // Move forward for spaces
                    }
                    continue; // Skip unsupported glyphs
                }

                const Glyph& g = it->second;

                float      x_pos = x + static_cast<float>(g.left + (glyph_pos[i].x_offset >> 6));
                float      y_pos = y - static_cast<float>(g.top + (glyph_pos[i].y_offset >> 6));
                const auto w     = static_cast<float>(g.width);
                const auto h     = static_cast<float>(g.height);

                // Compute texture coordinates
                float u0 = static_cast<float>(g.atlas_x) / static_cast<float>(font.atlas_width);
                float v0 = static_cast<float>(g.atlas_y) / static_cast<float>(font.atlas_height);
                float u1 = static_cast<float>(g.atlas_x + g.width) / static_cast<float>(font.atlas_width);
                float v1 = static_cast<float>(g.atlas_y + g.height) / static_cast<float>(font.atlas_height);

                // Add textured quad
                quads.emplace_back(
                    x_pos, y_pos, u0, v0,         // Top-left
                    x_pos + w, y_pos, u1, v0,     // Top-right
                    x_pos + w, y_pos + h, u1, v1, // Bottom-right
                    x_pos, y_pos + h, u0, v1      // Bottom-left
                );

                x += static_cast<float>(glyph_pos[i].x_advance >> 6); // Move forward
            }
        }
    };
} // namespace umgebung
