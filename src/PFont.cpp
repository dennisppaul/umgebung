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

#include "Umgebung.h"
#include "PFont.h"

using namespace umgebung;

#ifdef PFONT_DEBUG_FONT
#define STB_IMAGE_WRITE_IMPLEMENTATION // TOOD why does this cause a duplicate symbol error?
#include "stb_image_write.h"
#endif //PFONT_DEBUG_FONT

#ifdef PFONT_DEBUG_FONT
void PFont::DEBUG_save_font_atlas(const FontData& font, const std::string& output_path) const {
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

void PFont::DEBUG_save_text(const FontData&    font,
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
#endif //PFONT_DEBUG_FONT