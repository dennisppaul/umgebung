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

#include "UmgebungDraw.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static PFont *fCurrentFont = nullptr;

struct {
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1;
    bool active = false;
} fill_color, stroke_color;

void stroke(float r, float g, float b, float a) {
    stroke_color.r = r;
    stroke_color.g = g;
    stroke_color.b = b;
    stroke_color.a = a;
    stroke_color.active = true;
}

void stroke(float a) {
    stroke(a, a, a);
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

void fill(float a) {
    fill(a, a, a);
}

void noFill() {
    fill_color.active = false;
}

void background(float a, float b, float c, float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT);
}

void background(float a) {
    background(a, a, a);
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
    if (!stroke_color.active) return;
    glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

/* font */

PFont *loadFont(const char *file, float size) {
    auto *font = new PFont(file, size);
    return font;
}

void textFont(PFont *font) {
    fCurrentFont = font;
}

void text(const std::string &text, float x, float y) {
    if (fCurrentFont == nullptr) return;
    if (!fill_color.active) return;

    glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
    fCurrentFont->draw(text.c_str(), x, y);
}

PImage *loadImage(const std::string &filename) {
    auto *img = new PImage(filename);
    return img;
}

void image(PImage *img, float x, float y, float w, float h) {
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
    img->bind();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x, y);

    glTexCoord2f(1, 0);
    glVertex2f(x + w, y);

    glTexCoord2f(1, 1);
    glVertex2f(x + w, y + h);

    glTexCoord2f(0, 1);
    glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void image(PImage *img, float x, float y) {
    image(img, x, y, img->width, img->height);
}
