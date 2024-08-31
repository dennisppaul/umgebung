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

#include <vector>

#include "PGraphics.h"
#include "PFont.h"

using namespace umgebung;

// TODO look into OpenGL 3 e.g https://github.com/opengl-tutorials/ogl/

PGraphics::PGraphics() : width(0), height(0), ellipseVBO(0), ellipseSegments(0), bufferInitialized(false) {
#ifdef PGRAPHICS_USE_VBO
    setupEllipseBuffer(ELLIPSE_NUM_SEGMENTS);
#endif // PGRAPHICS_USE_VBO
}

PGraphics::~PGraphics() {
#ifdef PGRAPHICS_USE_VBO
    if (bufferInitialized) {
        glDeleteBuffers(1, &ellipseVBO);
    }
#endif // PGRAPHICS_USE_VBO
}

void PGraphics::stroke(float r, float g, float b, float a) {
    stroke_color.r      = r;
    stroke_color.g      = g;
    stroke_color.b      = b;
    stroke_color.a      = a;
    stroke_color.active = true;
}

void PGraphics::stroke(float brightness, float a) {
    stroke_color.r      = brightness;
    stroke_color.g      = brightness;
    stroke_color.b      = brightness;
    stroke_color.a      = a;
    stroke_color.active = true;
}

void PGraphics::stroke(float a) {
    stroke(a, a, a);
}

void PGraphics::noStroke() {
    stroke_color.active = false;
}

void PGraphics::fill(float r, float g, float b, float a) {
    fill_color.r      = r;
    fill_color.g      = g;
    fill_color.b      = b;
    fill_color.a      = a;
    fill_color.active = true;
}

void PGraphics::fill(float brightness, float a) {
    fill_color.r      = brightness;
    fill_color.g      = brightness;
    fill_color.b      = brightness;
    fill_color.a      = a;
    fill_color.active = true;
}

void PGraphics::fill(float a) {
    fill(a, a, a);
}

void PGraphics::noFill() {
    fill_color.active = false;
}

#ifndef DISABLE_GRAPHICS

void PGraphics::background(float a, float b, float c, float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PGraphics::background(float a) {
    background(a, a, a);
}

void PGraphics::rect(float x, float y, float _width, float _height) {
    if (fill_color.active) {
        glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + _width, y);
        glVertex2f(x + _width, y + _height);
        glVertex2f(x, y + _height);
        glEnd();
    }
    if (stroke_color.active) {
        glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + _width, y);
        glVertex2f(x + _width, y + _height);
        glVertex2f(x, y + _height);
        glEnd();
    }
}

#ifdef PGRAPHICS_USE_VBO
void PGraphics::setupEllipseBuffer(int num_segments) {
    ellipseSegments = num_segments;
    std::vector<GLfloat> ellipseVertices((num_segments + 1) * 2); // x and y for each vertex

    float theta = 2 * 3.1415926 / float(num_segments);
    float c     = cosf(theta); // precalculate the sine and cosine
    float s     = sinf(theta);
    float t;

    float x1 = 1.0f; // start at angle = 0
    float y1 = 0.0f;

    for (int i = 0; i < num_segments + 1; i++) {
        ellipseVertices[2 * i]     = x1;
        ellipseVertices[2 * i + 1] = y1;
        // apply rotation matrix
        t  = x1;
        x1 = c * x1 - s * y1;
        y1 = s * t + c * y1;
    }

    glGenBuffers(1, &ellipseVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);
    glBufferData(GL_ARRAY_BUFFER, ellipseVertices.size() * sizeof(GLfloat), ellipseVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    bufferInitialized = true;
}

void PGraphics::ellipse(float x, float y, float _width, float _height) {
    if (!bufferInitialized) {
        return; // Buffer not initialized
    }

    float hw = _width / 2.0f;  // Half width
    float hh = _height / 2.0f; // Half height

    if (fill_color.active) {
        glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, (void*) 0);

        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(hw, hh, 1.0f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, ellipseSegments + 1);
        glPopMatrix();

        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (stroke_color.active) {
        glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, (void*) 0);

        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(hw, hh, 1.0f);
        glDrawArrays(GL_LINE_LOOP, 0, ellipseSegments + 1);
        glPopMatrix();

        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
#else
void PGraphics::ellipse(float x, float y, float _width, float _height) {
    const int   num_segments = 36; // Number of segments to approximate the ellipse
    const float theta        = 2 * 3.1415926 / float(num_segments);
    const float c            = cosf(theta); // Precalculate the sine and cosine
    const float s            = sinf(theta);
    float       t;

    const float hw = _width / 2.0f; // Half width
    // const float hh = _height / 2.0f; // Half height

    float x1 = hw; // We start at angle = 0
    float y1 = 0;

    if (fill_color.active) {
        glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center of the ellipse
        for (int i = 0; i < num_segments + 1; i++) {
            glVertex2f(x + x1, y + y1);
            // Apply the rotation matrix
            t  = x1;
            x1 = c * x1 - s * y1;
            y1 = s * t + c * y1;
        }
        glEnd();
    }

    if (stroke_color.active) {
        glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        glBegin(GL_LINE_LOOP);
        x1 = hw; // We start at angle = 0
        y1 = 0;
        for (int i = 0; i < num_segments + 1; i++) {
            glVertex2f(x + x1, y + y1);
            // Apply the rotation matrix
            t  = x1;
            x1 = c * x1 - s * y1;
            y1 = s * t + c * y1;
        }
        glEnd();
    }
}
#endif // PGRAPHICS_USE_VBO

void PGraphics::circle(float x, float y, float diameter) {
    ellipse(x, y, diameter, diameter);
}

void PGraphics::line(float x1, float y1, float x2, float y2) {
    if (!stroke_color.active) return;
    glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}


void PGraphics::pointSize(float point_size) {
    fPointSize = point_size;
}

void PGraphics::point(float x, float y, float z) {
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

void PGraphics::beginShape(int shape) {
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

void PGraphics::endShape() {
    glEnd();
}

void PGraphics::vertex(float x, float y, float z) {
    glVertex3f(x, y, z);
}

#endif // DISABLE_GRAPHICS

/* font */

PFont* PGraphics::loadFont(std::string file, float size) {
    auto* font = new PFont(file.c_str(), size);
    return font;
}

void PGraphics::textFont(PFont* font) {
    fCurrentFont = font;
}

void PGraphics::textSize(float size) {
    if (fCurrentFont == nullptr) return;
    fCurrentFont->size(size);
}

void PGraphics::text(const std::string& text, float x, float y, float z) {
    if (fCurrentFont == nullptr) return;
    if (!fill_color.active) return;

#ifndef DISABLE_GRAPHICS
    glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
    fCurrentFont->draw(text.c_str(), x, y, z);
#endif // DISABLE_GRAPHICS
}

template<typename T>
void PGraphics::text(const T& value, float x, float y, float z) {
    std::ostringstream ss;
    ss << value;
    text(ss.str(), x, y, z);
}

PImage* PGraphics::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}

void PGraphics::image(PImage* img, float x, float y, float w, float h) {
#ifndef DISABLE_GRAPHICS
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
#endif // DISABLE_GRAPHICS
}

void PGraphics::image(PImage* img, float x, float y) {
    image(img, x, y, img->width, img->height);
}

#ifndef DISABLE_GRAPHICS

void PGraphics::popMatrix() {
    glPopMatrix();
}

void PGraphics::pushMatrix() {
    glPushMatrix();
}

void PGraphics::translate(float x, float y, float z) {
    glTranslatef(x, y, z);
}

void PGraphics::rotateX(float angle) {
    glRotatef(degrees(angle), 1.0f, 0.0f, 0.0f);
}

void PGraphics::rotateY(float angle) {
    glRotatef(degrees(angle), 0.0f, 1.0f, 0.0f);
}

void PGraphics::rotateZ(float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphics::rotate(float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphics::rotate(float angle, float x, float y, float z) {
    glRotatef(degrees(angle), x, y, z);
}

void PGraphics::scale(float x) {
    glScalef(x, x, x);
}

void PGraphics::scale(float x, float y) {
    glScalef(x, y, 1.0f);
}

void PGraphics::scale(float x, float y, float z) {
    glScalef(x, y, z);
}

#else  // DISABLE_GRAPHICS
void PGraphics::popMatrix() {
}

void PGraphics::pushMatrix() {
}

void PGraphics::translate(float x, float y, float z) {
}

void PGraphics::rotateX(float angle) {
}

void PGraphics::rotateY(float angle) {
}

void PGraphics::rotateZ(float angle) {
}

void PGraphics::rotate(float angle) {
}

void PGraphics::rotate(float angle, float x, float y, float z) {
}

void PGraphics::scale(float x) {
}

void PGraphics::scale(float x, float y) {
}

void PGraphics::scale(float x, float y, float z) {
}

void PGraphics::background(float a, float b, float c, float d) {
}

void PGraphics::background(float a) {
}

void PGraphics::rect(float x, float y, float _width, float _height) {
}

void PGraphics::line(float x1, float y1, float x2, float y2) {
}


void PGraphics::pointSize(float point_size) {
}

void PGraphics::point(float x, float y, float z) {
}

void PGraphics::beginShape(int shape) {
}

void PGraphics::endShape() {
}

void PGraphics::vertex(float x, float y, float z) {
}
#endif // DISABLE_GRAPHICS
