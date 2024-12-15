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

// TODO implement
//    ## Loading & Displaying
//    createFont() :: Dynamically converts a font to the format used by Processing
//    //loadFont() :: Loads a font into a variable of type PFont
//    //textFont() :: Sets the current font that will be drawn with the text() function
//    //text() :: Draws text to the screen
//    ## Metrics
//    textAscent() :: Returns ascent of the current font at its current size
//    textDescent() :: Returns descent of the current font at its current size
//    ## Attributes
//    textAlign() :: Sets the current alignment for drawing text
//    textLeading() :: Sets the spacing between lines of text in units of pixels
//    textMode() :: Sets the way text draws to the screen
//    //textSize() :: Sets the current font size
//    //textWidth() :: Calculates and returns the width of any character or text string


#ifndef DISABLE_GRAPHICS

#include <stdint.h>

#include <GL/glew.h>
#include <FTGL/ftgl.h>

#endif

#include "Umgebung.h"

namespace umgebung {

    class PFont {
    public:
        PFont(const char* file, float size, float pixelDensity = 1) {
#ifndef DISABLE_GRAPHICS
            font = new FTTextureFont(file);
            if (font->Error()) {
                std::cerr << "+++ error initializing font" << std::endl;
                delete font;
                return;
            }
            fPixelDensity = pixelDensity;
            font->FaceSize((int) size * fPixelDensity);
#endif // DISABLE_GRAPHICS
        }

        void size(float size) {
#ifndef DISABLE_GRAPHICS
            if (font == nullptr) {
                return;
            }
            font->FaceSize((int) size);
#endif
        }

        void draw(const char* text, float x, float y, float z) {
#ifndef DISABLE_GRAPHICS
            if (font == nullptr) {
                return;
            }
            glPushMatrix();
            glTranslatef(x, y, z);
            const float scaleFactor = 1.f / fPixelDensity;
            glScalef(scaleFactor, -scaleFactor, scaleFactor);
            font->Render(text, -1, FTPoint(0, 0));
            glPopMatrix();
#endif
        }

        float textWidth(const char* text) {
#ifndef DISABLE_GRAPHICS
            if (font == nullptr) {
                return 0;
            }

            return font->Advance(text, -1, FTPoint(0, 0)) / fPixelDensity;
#endif
        }

    private:
#ifndef DISABLE_GRAPHICS
        FTTextureFont* font          = nullptr;
        int            fPixelDensity = 1;
#endif
    };

} // namespace umgebung
