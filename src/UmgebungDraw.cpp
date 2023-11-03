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
static float fPointSize = 1;

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


void pointSize(float point_size) {
    fPointSize = point_size;
}

void point(float x, float y, float z) {
    if (!stroke_color.active) return;
    glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);

//    glPushMatrix();
//    translate(x - fPointSize * 0.5, y - fPointSize * 0.5, z);
//    rect(0, 0, fPointSize, fPointSize);
//    glPopMatrix();

    glPointSize(fPointSize); // @development when using antialiasing a point size of 1 produces a semitransparent point
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void beginShape(int shape) {
    if (!fill_color.active) return;
    glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);

    int mShape;
    switch (shape) {
        case TRIANGLES:
            mShape = GL_TRIANGLES;
            break;
        case TRIANGLE_STRIP:
            mShape = GL_TRIANGLE_STRIP;
            break;
        case TRIANGLE_FAN:
            mShape = GL_TRIANGLE_FAN;
            break;
        case QUADS:
            mShape = GL_QUADS;
            break;
        case QUAD_STRIP:
            mShape = GL_QUAD_STRIP;
            break;
        case POLYGON:
            mShape = GL_POLYGON;
            break;
        case POINTS:
            mShape = GL_POINTS;
            break;
        case LINES:
            mShape = GL_LINES;
            break;
        case LINE_STRIP:
            mShape = GL_LINE_STRIP;
            break;
        default:
            mShape = GL_TRIANGLES;
    }
    glBegin(mShape);
}

void endShape() {
    glEnd();
}

void vertex(float x, float y, float z) {
    glVertex3f(x, y, z);
}


/* font */

PFont *loadFont(const char *file, float size) {
    auto *font = new PFont(file, size);
    return font;
}

void textFont(PFont *font) {
    fCurrentFont = font;
}

void textSize(float size) {
    if (fCurrentFont == nullptr) return;
    fCurrentFont->size(size);
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

void popMatrix() {
    glPopMatrix();
}

void pushMatrix() {
    glPushMatrix();
}

void translate(float x, float y, float z) {
    glTranslatef(x, y, z);
}

void rotateX(float angle) {
    glRotatef(degrees(angle), 1.0f, 0.0f, 0.0f);
}

void rotateY(float angle) {
    glRotatef(degrees(angle), 0.0f, 1.0f, 0.0f);
}

void rotateZ(float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void rotate(float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void rotate(float angle, float x, float y, float z) {
    glRotatef(degrees(angle), x, y, z);
}

void scale(float x) {
    glScalef(x, x, x);
}

void scale(float x, float y) {
    glScalef(x, y, 1.0f);
}

void scale(float x, float y, float z) {
    glScalef(x, y, z);
}