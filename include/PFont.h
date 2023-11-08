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

#ifndef DISABLE_GRAPHICS

#include <FTGL/ftgl.h>

#endif

#include "Umgebung.h"

class PFont {
public:
    PFont(const char *file, float size) {
#ifndef DISABLE_GRAPHICS
        font = new FTTextureFont(file);
        if (font->Error()) {
            std::cerr << "+++ error initializing font" << std::endl;
            delete font;
            return;
        }
        font->FaceSize((int) size);
#endif // DISABLE_GRAPHICS
    }

    void size(float size) {
#ifndef DISABLE_GRAPHICS
        if (font == nullptr) return;
        font->FaceSize((int) size);
#endif
    }

    void draw(const char *text, float x, float y) {
#ifndef DISABLE_GRAPHICS
        if (font == nullptr) return;
        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(1, -1, 1);
        font->Render(text, -1, FTPoint(0, 0));
        glPopMatrix();
#endif
    }

private:
#ifndef DISABLE_GRAPHICS
    FTTextureFont *font = nullptr;
#endif
};