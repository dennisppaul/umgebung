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

#include <vector>
#include <type_traits>
#include <GL/glew.h>

#include "PGraphicsOpenGLv20.h"
#include "PFont.h"
#include "UmgebungFunctions.h"
#include "UmgebungFunctionsAdditional.h"

// TODO not sure if this is the best way to handle this
static_assert(std::is_same_v<GLuint, unsigned int>,
              "`GLuint` is not an `unsigned int`. change the FBO types in `PGraphics.h` and remove this line.");
static_assert(std::is_same_v<GLfloat, float>,
              "`GLfloat` is not a `float`. change float types e.g in `PShape.h` and remove this line.");

using namespace umgebung;

// TODO look into OpenGL 3 e.g https://github.com/opengl-tutorials/ogl/

PGraphicsOpenGLv20::PGraphicsOpenGLv20() : PImage(0, 0, 0) {
}

void PGraphicsOpenGLv20::strokeWeight(const float weight) {
    fStrokeWeight = weight;
    glLineWidth(weight);
}

void PGraphicsOpenGLv20::background(const float a, const float b, const float c, const float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PGraphicsOpenGLv20::background(const float a) {
    background(a, a, a);
}

void PGraphicsOpenGLv20::rect(const float x, const float y, const float width, const float height) {
    if (fill_state.active) {
        glColor4f(fill_state.r, fill_state.g, fill_state.b, fill_state.a);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }
    if (stroke_state.active) {
        glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }
}

static void draw_ellipse(const GLenum shape,
                         const int    num_segments,
                         const float  x,
                         const float  y,
                         const float  width,
                         const float  height) {
    const float radiusX = width / 2.0f;
    const float radiusY = height / 2.0f;
    const float centerX = x;
    const float centerY = y;
    glBegin(shape);

    if (shape == GL_TRIANGLE_FAN) {
        glVertex2f(centerX, centerY);
    }

    for (int i = 0; i <= num_segments; ++i) {
        const float theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(num_segments);
        const float dx    = radiusX * cosf(theta);
        const float dy    = radiusY * sinf(theta);
        glVertex2f(centerX + dx, centerY + dy);
    }
    glEnd();
}

void PGraphicsOpenGLv20::ellipse(const float x, const float y, const float width, const float height) {
    if (fill_state.active) {
        glColor4f(fill_state.r, fill_state.g, fill_state.b, fill_state.a);
        draw_ellipse(GL_TRIANGLE_FAN, fEllipseDetail, x, y, width, height);
    }

    if (stroke_state.active) {
        glColor4f(fill_state.r, fill_state.g, fill_state.b, fill_state.a);
        draw_ellipse(GL_LINE_LOOP, fEllipseDetail, x, y, width, height);
    }
}

void PGraphicsOpenGLv20::circle(const float x, const float y, const float diameter) {
    ellipse(x, y, diameter, diameter);
}

void PGraphicsOpenGLv20::ellipseDetail(const int detail) {
    fEllipseDetail = detail;
}

void PGraphicsOpenGLv20::line(const float x1, const float y1, const float x2, const float y2) {
    if (!stroke_state.active) {
        return;
    }
    glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void PGraphicsOpenGLv20::line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
    if (!stroke_state.active) {
        return;
    }
    glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void PGraphicsOpenGLv20::triangle(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, const float x3, const float y3, const float z3) {
    if (!stroke_state.active) {
        glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);
        glBegin(GL_TRIANGLES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);
        glEnd();
        return;
    }
    if (!fill_state.active) {
        glColor4f(fill_state.r, fill_state.g, fill_state.b, stroke_state.a);
        glBegin(GL_TRIANGLES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);
        glEnd();
        return;
    }
}

void PGraphicsOpenGLv20::bezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4) {
    if (!stroke_state.active) {
        return;
    }
    glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);

    const int   segments = fBezierDetail;
    const float step     = 1.0f / static_cast<float>(segments);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) * step;
        const float u = 1.0f - t;

        const float b0 = u * u * u;
        const float b1 = 3 * u * u * t;
        const float b2 = 3 * u * t * t;
        const float b3 = t * t * t;

        const float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        const float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;

        glVertex2f(x, y);
    }
    glEnd();
}

void PGraphicsOpenGLv20::bezier(const float x1, const float y1, const float z1,
                                const float x2, const float y2, const float z2,
                                const float x3, const float y3, const float z3,
                                const float x4, const float y4, const float z4) {
    if (!stroke_state.active) {
        return;
    }
    glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);

    const int   segments = fBezierDetail;
    const float step     = 1.0f / static_cast<float>(segments);

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) * step;
        const float u = 1.0f - t;

        const float b0 = u * u * u;
        const float b1 = 3 * u * u * t;
        const float b2 = 3 * u * t * t;
        const float b3 = t * t * t;

        const float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        const float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;
        const float z = b0 * z1 + b1 * z2 + b2 * z3 + b3 * z4;

        glVertex3f(x, y, z);
    }
    glEnd();
}

void PGraphicsOpenGLv20::bezierDetail(const int detail) {
    fBezierDetail = detail;
}

void PGraphicsOpenGLv20::pointSize(const float point_size) {
    fPointSize = point_size;
}

void PGraphicsOpenGLv20::point(const float x, const float y, const float z) {
    if (!stroke_state.active) {
        return;
    }
    glColor4f(stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);

    //    glPushMatrix();
    //    translate(x - fPointSize * 0.5, y - fPointSize * 0.5, z);
    //    rect(0, 0, fPointSize, fPointSize);
    //    glPopMatrix();

    glPointSize(fPointSize); // @development when using antialiasing a point size of 1 produces a semitransparent point
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void PGraphicsOpenGLv20::beginShape(const int shape) {
    if (!fill_state.active) {
        // TODO why not stroke shapes?
        return;
    }
    fShapeBegun = true;
    if (fEnabledTextureInShape) {
    }
    glColor4f(fill_state.r, fill_state.g, fill_state.b, fill_state.a);

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

void PGraphicsOpenGLv20::endShape(bool close_shape) {
    glEnd();
    if (fEnabledTextureInShape) {
        glDisable(GL_TEXTURE_2D);
        fEnabledTextureInShape = false;
    }
    fShapeBegun = false;
}

void PGraphicsOpenGLv20::vertex(const float x, const float y, const float z) {
    glVertex3f(x, y, z);
}


void PGraphicsOpenGLv20::vertex(const float x, const float y, const float z, const float u, const float v) {
    glTexCoord2f(u, v);
    glVertex3f(x, y, z);
}

/* font */

PFont* PGraphicsOpenGLv20::loadFont(const std::string& file, const float size) {
    auto* font = new PFont(file.c_str(), size, static_cast<float>(fPixelDensity));
    return font;
}

void PGraphicsOpenGLv20::textFont(PFont* font) {
    fCurrentFont = font;
}

void PGraphicsOpenGLv20::textSize(const float size) {
    if (fCurrentFont == nullptr) {
        return;
    }
    fCurrentFont->size(size);
}

void PGraphicsOpenGLv20::text_str(const std::string& text, const float x, const float y, const float z) {
    if (fCurrentFont == nullptr) {
        return;
    }
    if (!fill_state.active) {
        return;
    }

#ifndef DISABLE_GRAPHICS
    glColor4f(fill_state.r, fill_state.g, fill_state.b, fill_state.a);
    fCurrentFont->draw(text.c_str(), x, y, z);
#endif // DISABLE_GRAPHICS
}

float PGraphicsOpenGLv20::textWidth(const std::string& text) {
    if (fCurrentFont == nullptr) {
        return 0;
    }

#ifndef DISABLE_GRAPHICS
    return fCurrentFont->textWidth(text.c_str());
#endif // DISABLE_GRAPHICS
}

void PGraphicsOpenGLv20::pixelDensity(const int value) {
    if (value > 0 && value <= 3) {
        fPixelDensity = value;
    } else {
        std::cerr << "PixelDensity can only be between 1 and 3." << std::endl;
    }
}

void PGraphicsOpenGLv20::text(const char* value, const float x, const float y, const float z) {
    text_str(value, x, y, z);
}

PImage* PGraphicsOpenGLv20::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}

void PGraphicsOpenGLv20::image(PImage* img, const float x, const float y, const float w, const float h) {
#ifndef DISABLE_GRAPHICS
    glEnable(GL_TEXTURE_2D);
    glColor4f(fill_state.r, fill_state.g, fill_state.b, fill_state.a);
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

void PGraphicsOpenGLv20::image(PImage* img, const float x, const float y) {
    image(img, x, y, img->width, img->height);
}

void PGraphicsOpenGLv20::texture(PImage* img) {
#ifndef DISABLE_GRAPHICS
    if (fShapeBegun) {
        std::cerr << "texture must be set before `beginShape()`" << std::endl;
        return;
    }
    fEnabledTextureInShape = true;
    glEnable(GL_TEXTURE_2D);
    img->bind();
#endif // DISABLE_GRAPHICS
}

#ifndef DISABLE_GRAPHICS

void PGraphicsOpenGLv20::popMatrix() {
    glPopMatrix();
}

void PGraphicsOpenGLv20::pushMatrix() {
    glPushMatrix();
}

void PGraphicsOpenGLv20::translate(const float x, const float y, const float z) {
    glTranslatef(x, y, z);
}

void PGraphicsOpenGLv20::rotateX(const float angle) {
    glRotatef(degrees(angle), 1.0f, 0.0f, 0.0f);
}

void PGraphicsOpenGLv20::rotateY(const float angle) {
    glRotatef(degrees(angle), 0.0f, 1.0f, 0.0f);
}

void PGraphicsOpenGLv20::rotateZ(const float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphicsOpenGLv20::rotate(const float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphicsOpenGLv20::rotate(const float angle, const float x, const float y, const float z) {
    glRotatef(degrees(angle), x, y, z);
}

void PGraphicsOpenGLv20::scale(const float x) {
    glScalef(x, x, x);
}

void PGraphicsOpenGLv20::scale(const float x, const float y) {
    glScalef(x, y, 1.0f);
}

void PGraphicsOpenGLv20::scale(const float x, const float y, const float z) {
    glScalef(x, y, z);
}

void PGraphicsOpenGLv20::beginDraw() {
#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    /* save state */
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fPreviousFBO);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // bind the FBO for offscreen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    glViewport(0, 0, framebuffer.width, framebuffer.height);

    /* setup projection and modelview matrices */

    glMatrixMode(GL_PROJECTION);
    // save the current projection matrix
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, framebuffer.width, 0, framebuffer.height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    // save the current modelview matrix
    glPushMatrix();
    glLoadIdentity();
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER
}

void PGraphicsOpenGLv20::endDraw() {
#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    // restore projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    /* restore state */
    glBindFramebuffer(GL_FRAMEBUFFER, fPreviousFBO); // Restore the previously bound FBO
    glPopMatrix();
    glPopAttrib();
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER
}

void PGraphicsOpenGLv20::bind() {
#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER
}

void PGraphicsOpenGLv20::init(uint32_t* pixels, const int width, const int height, int format, bool generate_mipmap) {
    this->width        = width;
    this->height       = height;
    framebuffer.width  = width;
    framebuffer.height = height;
#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    glGenFramebuffers(1, &framebuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    glGenTextures(1, &framebuffer.texture);
    glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 framebuffer.width,
                 framebuffer.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Handle framebuffer incomplete error
        std::cerr << "ERROR Framebuffer is not complete!" << std::endl;
    }
    glViewport(0, 0, framebuffer.width, framebuffer.height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER
}

void PGraphicsOpenGLv20::hint(const uint16_t property) {
    switch (property) {
        case ENABLE_SMOOTH_LINES:
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            break;
        case DISABLE_SMOOTH_LINES:
            glDisable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
            break;
        default:
            break;
    }
}

#else // DISABLE_GRAPHICS
void PGraphicsOpenGLv20::popMatrix() {
}

void PGraphicsOpenGLv20::pushMatrix() {
}

void PGraphicsOpenGLv20::translate(float x, float y, float z) {
}

void PGraphicsOpenGLv20::rotateX(float angle) {
}

void PGraphicsOpenGLv20::rotateY(float angle) {
}

void PGraphicsOpenGLv20::rotateZ(float angle) {
}

void PGraphicsOpenGLv20::rotate(float angle) {
}

void PGraphicsOpenGLv20::rotate(float angle, float x, float y, float z) {
}

void PGraphicsOpenGLv20::scale(float x) {
}

void PGraphicsOpenGLv20::scale(float x, float y) {
}

void PGraphicsOpenGLv20::scale(float x, float y, float z) {
}

void PGraphicsOpenGLv20::background(float a, float b, float c, float d) {
}

void PGraphicsOpenGLv20::background(float a) {
}

void PGraphicsOpenGLv20::rect(float x, float y, float _width, float _height) const {
}

void PGraphicsOpenGLv20::line(float x1, float y1, float x2, float y2) const {
}

void PGraphicsOpenGLv20::bezier(float x1, float y1,
                                float x2, float y2,
                                float x3, float y3,
                                float x4, float y4) const {}

void PGraphicsOpenGLv20::bezier(float x1, float y1, float z1,
                                float x2, float y2, float z2,
                                float x3, float y3, float z3,
                                float x4, float y4, float z4) const {}

void PGraphicsOpenGLv20::bezierDetail(int detail) {}
void PGraphicsOpenGLv20::pointSize(float point_size) {}
void PGraphicsOpenGLv20::point(float x, float y, float z) const {}
void PGraphicsOpenGLv20::beginShape(int shape) {}
void PGraphicsOpenGLv20::endShape() {}
void PGraphicsOpenGLv20::vertex(float x, float y, float z) {}
void PGraphicsOpenGLv20::vertex(float x, float y, float z, float u, float v) {}
void PGraphicsOpenGLv20::strokeWeight(float weight) {}
void PGraphicsOpenGLv20::hint(const uint16_t property) {}

#endif // DISABLE_GRAPHICS
