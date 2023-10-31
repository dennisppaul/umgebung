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

#include <GLFW/glfw3.h>
#include "UmgebungDraw.h"

static struct {
    float r;
    float g;
    float b;
    float a;
    bool active;
} fill_color, stroke_color;

void stroke(float r, float g, float b, float a) {
    stroke_color.r = r;
    stroke_color.g = g;
    stroke_color.b = b;
    stroke_color.a = a;
    stroke_color.active = true;
}

void noStroke() {
    stroke_color.active = false;
}

void fill(float r, float g, float b, float a) {
    fill_color.r = r;
    fill_color.g = g;
    fill_color.b = b;
    fill_color.a = a;
    fill_color.active = true;
}

void noFill() {
    fill_color.active = false;
}

void background(float a, float b, float c, float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT);
}

void rect(float x, float y, float width, float height) {
    if (fill_color.active) {
        glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }
    if (stroke_color.active) {
        glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }
}

void line(float x1, float y1, float x2, float y2) {
    if (stroke_color.active) {
        glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }
}