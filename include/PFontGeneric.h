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

#include <string>
#include <locale>
#include <codecvt>

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>

#include "harfbuzz/hb.h"
#include "harfbuzz/hb-ft.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "UmgebungConstants.h"
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

    class PFontGeneric final : public PImage {
    public:
        explicit PFontGeneric(const std::string& font_filepath, const int font_size) : font_size(font_size) {
            const std::string character_atlas = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_=+[{]};:'\",<.>/?`~";
            init(font_filepath, character_atlas);

            // texture_id = create_font_texture(*font); // NOTE this is done in PGraphics
            width  = font->atlas_width;
            height = font->atlas_height;
            format = 4;
            pixels = new uint32_t[width * height];
            copy_atlas_to_rgba(*font, reinterpret_cast<unsigned char*>(pixels));

            console("created PFont atlas");
            console("size   : ", width, "×", height);
            console("texture: ", texture_id);

            save_font_atlas(*font, font_filepath + "--font_atlas.png");
            save_text(*font, "AVTAWaToVAWeYoyo Hamburgefonts", font_filepath + "--text.png");

            generate_text_quads(*font, "AVTAWaToVAWeYoyo Hamburgefonts", text_quads);
            console("generated quads: ", text_quads.size());
        }

        static constexpr int MAX_ATLAS_WIDTH = 512;

        const int font_size;
        const int atlas_character_padding = 2;

        void draw(PGraphics* g, const float x, const float y, const float z = 0) {
            generate_text_quads(*font, "AVTAWaToVAWeYoyo Hamburgefonts", text_quads);
            pushMatrix();

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

        void init(const std::string& font_filepath, const std::string& character_atlas) {
            const char* filepath_c = font_filepath.c_str();
            FT_Init_FreeType(&ft);
            font         = new FontData();
            font->buffer = hb_buffer_create();
            FT_New_Face(ft, filepath_c, 0, &font->face);
            FT_Set_Pixel_Sizes(font->face, 0, font_size);
            font->hb_font = hb_ft_font_create(font->face, nullptr);

            create_font_atlas(*font, character_atlas.c_str());
            font_texture = create_font_texture(*font);
        }

        ~PFontGeneric() override {
            hb_buffer_destroy(font->buffer);
            hb_font_destroy(font->hb_font);
            FT_Done_Face(font->face);
            delete font;
            FT_Done_FreeType(ft);
            delete pixels;
        }

    private:
        struct Glyph {
            std::vector<unsigned char> bitmap;
            int                        width;
            int                        height;
            int                        left;
            int                        top;
            int                        advance;
            int                        atlas_x;
            int                        atlas_y;
        };

        struct FontData {
            std::unordered_map<uint32_t, Glyph> glyphs;
            int                                 ascent;
            int                                 descent;
            int                                 line_gap;
            int                                 atlas_width;
            int                                 atlas_height;
            std::vector<unsigned char>          atlas;
            FT_Face                             face{nullptr};
            hb_font_t*                          hb_font{nullptr};
            hb_buffer_t*                        buffer{nullptr};
        };

        std::vector<TexturedQuad> text_quads;
        GLuint                    font_texture{0};
        FontData*                 font{nullptr};
        FT_Library                ft{nullptr};

        void save_font_atlas(const FontData& font, const std::string& output_path) const {
            // Convert the grayscale atlas to an RGBA image for better visibility
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

                    // // Store grayscale value into RGBA format (white text on black background)
                    // atlas_rgba[idx + 0] = val; // R
                    // atlas_rgba[idx + 1] = val; // G
                    // atlas_rgba[idx + 2] = val; // B
                    // atlas_rgba[idx + 3] = 255; // A (fully opaque)
                }
            }

            // Save as PNG
            stbi_write_png(output_path.c_str(), font.atlas_width, font.atlas_height, 4, atlas_rgba.data(), font.atlas_width * 4);
            console("Font atlas saved to: ", output_path);
        }

        // static std::u16string utf8_to_utf16(std::string& utf8) {
        //     std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        //     return convert.from_bytes(utf8);
        // }

        void create_font_atlas(FontData& font, std::string characters_in_atlas) const {
            if (font.face == nullptr || font.hb_font == nullptr) {
                error("font data not intizialized");
                return;
            }

            font.ascent   = font.face->size->metrics.ascender >> 6;
            font.descent  = -font.face->size->metrics.descender >> 6;
            font.line_gap = font.face->size->metrics.height >> 6;


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

            int       current_x      = 0;
            int       current_y      = 0;
            int       max_row_height = 0;
            const int atlas_width    = MAX_ATLAS_WIDTH;
            int       atlas_height   = 0;

            for (unsigned int i = 0; i < glyph_count; i++) {
                FT_Load_Glyph(font.face, glyph_info[i].codepoint, FT_LOAD_RENDER);
                const FT_GlyphSlot glyph = font.face->glyph;

                if (font.glyphs.find(glyph_info[i].codepoint) == font.glyphs.end()) {
                    Glyph g;
                    g.width   = glyph->bitmap.width;
                    g.height  = glyph->bitmap.rows;
                    g.left    = glyph->bitmap_left;
                    g.top     = glyph->bitmap_top;
                    g.advance = glyph->advance.x >> 6;

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

        // TODO move to `UmgebungConstantsOpenGL.h`
        static constexpr GLint UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE    = GL_UNSIGNED_INT_8_8_8_8_REV;
        static constexpr GLint UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT = GL_RGBA;

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

        /**
         * @deprecated this is handle by PGraphics
         * @param font
         * @return
         */
        GLuint create_font_texture(const FontData& font) const {
            // TODO this should happen in OpenGL context ... as for PImage
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

        void generate_text_quads(const FontData&            font,
                                 const char*                text,
                                 std::vector<TexturedQuad>& quads) const {
            quads.clear();
            hb_buffer_clear_contents(font.buffer);

            hb_buffer_add_utf8(font.buffer, text, -1, 0, -1);
            hb_buffer_set_direction(font.buffer, HB_DIRECTION_LTR);
            hb_buffer_set_script(font.buffer, HB_SCRIPT_LATIN);
            hb_buffer_set_language(font.buffer, hb_language_from_string("en", 2));

            hb_shape(font.hb_font, font.buffer, nullptr, 0);

            unsigned int               glyph_count;
            const hb_glyph_info_t*     glyph_info = hb_buffer_get_glyph_infos(font.buffer, &glyph_count);
            const hb_glyph_position_t* glyph_pos  = hb_buffer_get_glyph_positions(font.buffer, &glyph_count);

            // Dynamically decide whether to preallocate
            if (strlen(text) > 100) { // Only preallocate if the text is long
                size_t estimated_quads = 0;
                for (unsigned int i = 0; i < glyph_count; i++) {
                    uint32_t glyph_id = glyph_info[i].codepoint;
                    if (glyph_id != ' ' && font.glyphs.find(glyph_id) != font.glyphs.end()) {
                        estimated_quads++; // Only count valid glyphs (not spaces)
                    }
                }
                quads.reserve(estimated_quads);
            }

            float       x = 0.0f;
            const float y = static_cast<float>(font.ascent); // Baseline position

            for (unsigned int i = 0; i < glyph_count; i++) {
                uint32_t glyph_id = glyph_info[i].codepoint;

                auto it = font.glyphs.find(glyph_id);
                if (it == font.glyphs.end()) {
                    // Handle spaces explicitly
                    if (glyph_id == ' ') {
                        x += glyph_pos[i].x_advance >> 6; // Move forward for spaces
                    }
                    continue; // Skip unsupported glyphs
                }

                const Glyph& g = it->second;

                float       x_pos = x + g.left + (glyph_pos[i].x_offset >> 6);
                float       y_pos = y - g.top + (glyph_pos[i].y_offset >> 6);
                const float w     = static_cast<float>(g.width);
                const float h     = static_cast<float>(g.height);

                // Compute texture coordinates
                float u0 = static_cast<float>(g.atlas_x) / font.atlas_width;
                float v0 = static_cast<float>(g.atlas_y) / font.atlas_height;
                float u1 = static_cast<float>(g.atlas_x + g.width) / font.atlas_width;
                float v1 = static_cast<float>(g.atlas_y + g.height) / font.atlas_height;

                // Add textured quad
                quads.push_back({
                    x_pos, y_pos, u0, v0,         // Top-left
                    x_pos + w, y_pos, u1, v0,     // Top-right
                    x_pos + w, y_pos + h, u1, v1, // Bottom-right
                    x_pos, y_pos + h, u0, v1      // Bottom-left
                });

                x += glyph_pos[i].x_advance >> 6; // Move forward
            }
        }

        void save_text(const FontData&    font,
                       const char*        text,
                       const std::string& outputfile) const {

            hb_buffer_clear_contents(font.buffer);
            hb_buffer_add_utf8(font.buffer, text, -1, 0, -1);
            hb_buffer_set_direction(font.buffer, HB_DIRECTION_LTR);
            hb_buffer_set_script(font.buffer, HB_SCRIPT_LATIN);
            hb_buffer_set_language(font.buffer, hb_language_from_string("en", 2));

            // Use the pre-existing HarfBuzz font
            hb_shape(font.hb_font, font.buffer, nullptr, 0);

            unsigned int               glyph_count;
            const hb_glyph_info_t*     glyph_info = hb_buffer_get_glyph_infos(font.buffer, &glyph_count);
            const hb_glyph_position_t* glyph_pos  = hb_buffer_get_glyph_positions(font.buffer, &glyph_count);

            const int max_height      = font.ascent + font.descent;
            int       total_advance   = 0;
            int       trimmed_advance = 0; // Tracks actual needed width

            // Compute max width while ignoring missing glyphs
            for (unsigned int i = 0; i < glyph_count; i++) {
                uint32_t glyph_id = glyph_info[i].codepoint;
                if (glyph_id == ' ' || font.glyphs.find(glyph_id) != font.glyphs.end()) {
                    trimmed_advance += glyph_pos[i].x_advance >> 6; // Count only valid glyphs and spaces
                }
                total_advance += glyph_pos[i].x_advance >> 6; // Original width
            }

            total_advance = trimmed_advance; // Trim unused space

            std::vector<unsigned char> image;
            image.assign(total_advance * max_height, 0);

            int       x = 0;
            const int y = font.ascent; // Baseline position

            for (unsigned int i = 0; i < glyph_count; i++) {
                uint32_t glyph_id = glyph_info[i].codepoint;

                auto it = font.glyphs.find(glyph_id);
                if (it == font.glyphs.end()) {
                    if (glyph_id == ' ') {
                        x += glyph_pos[i].x_advance >> 6; // Move forward for spaces
                    }
                    continue;
                }

                const Glyph& g = it->second;

                const int x_pos = x + g.left + (glyph_pos[i].x_offset >> 6);
                const int y_pos = y - g.top + (glyph_pos[i].y_offset >> 6);

                for (int row = 0; row < g.height; row++) {
                    for (int col = 0; col < g.width; col++) {
                        const int atlas_x = g.atlas_x + col;
                        const int atlas_y = g.atlas_y + row;
                        const int img_x   = x_pos + col;
                        const int img_y   = y_pos + row;

                        if (img_x >= 0 && img_x < total_advance && img_y >= 0 && img_y < max_height) {
                            if (atlas_x >= 0 && atlas_x < font.atlas_width &&
                                atlas_y >= 0 && atlas_y < font.atlas_height) {
                                unsigned char val                    = font.atlas[atlas_y * font.atlas_width + atlas_x];
                                image[img_y * total_advance + img_x] = std::max(image[img_y * total_advance + img_x], val);
                            }
                        }
                    }
                }

                x += glyph_pos[i].x_advance >> 6; // Move forward
            }

            // Save image
            //     stbi_write_png(output_path, total_advance, max_height, 1, image.data(), total_advance);
            std::vector<unsigned char> image_rgba(image.size() * 4, 255);
            for (int i = 0; i < image.size(); ++i) {
                image_rgba[i * 4 + 0] = 255;
                image_rgba[i * 4 + 1] = 255;
                image_rgba[i * 4 + 2] = 255;
                image_rgba[i * 4 + 3] = image[i];
            }
            stbi_write_png(outputfile.c_str(), total_advance, max_height, 4, image_rgba.data(), total_advance * 4);
        }
    };
} // namespace umgebung
