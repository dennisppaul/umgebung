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

#include <iostream>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>

class PFont {
public:
    PFont(const char *file, float size) {
        font = new FTTextureFont(file);
        if (font->Error()) {
            std::cerr << "+++ error initializing font" << std::endl;
            delete font;
            return;
        }
        font->FaceSize((int) size);
    }

    void draw(const char *text, float x, float y) {
        if (font == nullptr) return;
        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(1, -1, 1);
        font->Render(text, -1, FTPoint(0, 0));
        glPopMatrix();
    }

private:
    FTTextureFont *font = nullptr;

};