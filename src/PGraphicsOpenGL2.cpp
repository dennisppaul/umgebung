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

#include <GL/glew.h>
#include <vector>
#include <type_traits>

#include "PGraphics.h"
#include "PFont.h"
#include "UmgebungFunctions.h"
#include "UmgebungFunctionsAdditional.h"

static_assert(std::is_same_v<GLuint, unsigned int>,
              "`GLuint` is not an `unsigned int`. change the FBO types in PGraphics.h and remove this line.");

using namespace umgebung;

// TODO look into OpenGL 3 e.g https://github.com/opengl-tutorials/ogl/
PGraphicsOpenGL2::PGraphicsOpenGL2() : PImage(0, 0, 0) {
#ifdef PGRAPHICS_USE_VBO
    ellipseVBO        = 0;
    ellipseSegments   = 0;
    bufferInitialized = false;
    setupEllipseBuffer(ELLIPSE_NUM_SEGMENTS);
#endif // PGRAPHICS_USE_VBO
    fill(1.0f);
    stroke(0.0f);
}

PGraphicsOpenGL2::~PGraphicsOpenGL2() {
#ifdef PGRAPHICS_USE_VBO
    if (bufferInitialized) {
        glDeleteBuffers(1, &ellipseVBO);
    }
#endif // PGRAPHICS_USE_VBO
}

void PGraphicsOpenGL2::stroke(const float r, const float g, const float b, const float alpha) {
    stroke_color.r      = r;
    stroke_color.g      = g;
    stroke_color.b      = b;
    stroke_color.a      = alpha;
    stroke_color.active = true;
}

void PGraphicsOpenGL2::stroke(const float gray, const float alpha) {
    stroke_color.r      = gray;
    stroke_color.g      = gray;
    stroke_color.b      = gray;
    stroke_color.a      = alpha;
    stroke_color.active = true;
}

void PGraphicsOpenGL2::stroke_i(const uint32_t c) {
    color_inv(c, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
    // stroke_color.r      = static_cast<float>(c >> 16 & 0xFF) / 255.0f;
    // stroke_color.g      = static_cast<float>(c >> 8 & 0xFF) / 255.0f;
    // stroke_color.b      = static_cast<float>(c & 0xFF) / 255.0f;
    // stroke_color.a      = static_cast<float>(c >> 24 & 0xFF) / 255.0f;
    stroke_color.active = true;
}

void PGraphicsOpenGL2::stroke(const float a) {
    stroke(a, a, a);
}

void PGraphicsOpenGL2::noStroke() {
    stroke_color.active = false;
}

void PGraphicsOpenGL2::strokeWeight(float weight) {
    glLineWidth(weight);
}

void PGraphicsOpenGL2::fill(const float r, const float g, const float b, const float alpha) {
    fill_color.r      = r;
    fill_color.g      = g;
    fill_color.b      = b;
    fill_color.a      = alpha;
    fill_color.active = true;
}

void PGraphicsOpenGL2::fill(const float gray, const float alpha) {
    fill(gray, gray, gray, alpha);
}

void PGraphicsOpenGL2::fill_i(const uint32_t c) {
    color_inv(c, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
    // fill_color.r      = static_cast<float>(c >> 16 & 0xFF) / 255.0f;
    // fill_color.g      = static_cast<float>(c >> 8 & 0xFF) / 255.0f;
    // fill_color.b      = static_cast<float>(c & 0xFF) / 255.0f;
    // fill_color.a      = static_cast<float>(c >> 24 & 0xFF) / 255.0f;
    fill_color.active = true;
}

void PGraphicsOpenGL2::noFill() {
    fill_color.active = false;
}

#ifndef DISABLE_GRAPHICS

void PGraphicsOpenGL2::background(const float a, const float b, const float c, const float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PGraphicsOpenGL2::background(const float a) {
    background(a, a, a);
}

void PGraphicsOpenGL2::rect(const float x, const float y, const float width, const float height) const {
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

void PGraphicsOpenGL2::ellipse(const float x, const float y, const float width, const float height) const {
    if (fill_color.active) {
        glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        draw_ellipse(GL_TRIANGLE_FAN, fEllipseDetail, x, y, width, height);
    }

    if (stroke_color.active) {
        glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        draw_ellipse(GL_LINE_LOOP, fEllipseDetail, x, y, width, height);
    }
}
#endif // PGRAPHICS_USE_VBO

void PGraphicsOpenGL2::circle(const float x, const float y, const float diameter) const {
    ellipse(x, y, diameter, diameter);
}

void PGraphicsOpenGL2::ellipseDetail(const int detail) {
    fEllipseDetail = detail;
}

void PGraphicsOpenGL2::line(const float x1, const float y1, const float x2, const float y2) const {
    if (!stroke_color.active) {
        return;
    }
    glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void PGraphicsOpenGL2::bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) const {
    if (!stroke_color.active) {
        return;
    }
    glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);

    const int   segments = fBezierDetail;
    const float step     = 1.0f / segments;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        const float t = i * step;
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

void PGraphicsOpenGL2::bezier(float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float x4, float y4, float z4) const {
    if (!stroke_color.active) {
        return;
    }
    glColor4f(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);

    const int   segments = fBezierDetail;
    const float step     = 1.0f / segments;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float t = i * step;
        float u = 1.0f - t;

        float b0 = u * u * u;
        float b1 = 3 * u * u * t;
        float b2 = 3 * u * t * t;
        float b3 = t * t * t;

        float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;
        float z = b0 * z1 + b1 * z2 + b2 * z3 + b3 * z4;

        glVertex3f(x, y, z);
    }
    glEnd();
}

void PGraphicsOpenGL2::bezierDetail(int detail) {
    fBezierDetail = detail;
}

void PGraphicsOpenGL2::pointSize(const float point_size) {
    fPointSize = point_size;
}

void PGraphicsOpenGL2::point(const float x, const float y, const float z) const {
    if (!stroke_color.active) {
        return;
    }
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

void PGraphicsOpenGL2::beginShape(const int shape) {
    if (!fill_color.active) {
        return;
    }
    fShapeBegun = true;
    if (fEnabledTextureInShape) {
    }
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

void PGraphicsOpenGL2::endShape() {
    glEnd();
    if (fEnabledTextureInShape) {
        glDisable(GL_TEXTURE_2D);
        fEnabledTextureInShape = false;
    }
    fShapeBegun = false;
}

void PGraphicsOpenGL2::vertex(const float x, const float y, const float z) {
    glVertex3f(x, y, z);
}


void PGraphicsOpenGL2::vertex(const float x, const float y, const float z, const float u, const float v) {
    glTexCoord2f(u, v);
    glVertex3f(x, y, z);
}

#endif // DISABLE_GRAPHICS

/* font */

PFont* PGraphicsOpenGL2::loadFont(const std::string& file, const float size) {
    auto* font = new PFont(file.c_str(), size, fPixelDensity);
    return font;
}

void PGraphicsOpenGL2::textFont(PFont* font) {
    fCurrentFont = font;
}

void PGraphicsOpenGL2::textSize(const float size) const {
    if (fCurrentFont == nullptr) {
        return;
    }
    fCurrentFont->size(size);
}

void PGraphicsOpenGL2::text_str(const std::string& text, const float x, const float y, const float z) const {
    if (fCurrentFont == nullptr) {
        return;
    }
    if (!fill_color.active) {
        return;
    }

#ifndef DISABLE_GRAPHICS
    glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
    fCurrentFont->draw(text.c_str(), x, y, z);
#endif // DISABLE_GRAPHICS
}

float PGraphicsOpenGL2::textWidth(const std::string& text) const {
    if (fCurrentFont == nullptr) {
        return 0;
    }

#ifndef DISABLE_GRAPHICS
    return fCurrentFont->textWidth(text.c_str());
#endif // DISABLE_GRAPHICS
}

void PGraphicsOpenGL2::pixelDensity(const int value) {
    if (value > 0 && value <= 3) {
        fPixelDensity = value;
    } else {
        std::cerr << "PixelDensity can only be between 1 and 3." << std::endl;
    }
}

void PGraphicsOpenGL2::text(const char* value, const float x, const float y, const float z) const {
    text_str(value, x, y, z);
}

PImage* PGraphicsOpenGL2::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}

void PGraphicsOpenGL2::image(const PImage* img, const float x, const float y, const float w, const float h) const {
#ifndef DISABLE_GRAPHICS
    glEnable(GL_TEXTURE_2D);
    glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
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

void PGraphicsOpenGL2::image(PImage* img, const float x, const float y) {
    image(img, x, y, img->width, img->height);
}

void PGraphicsOpenGL2::texture(const PImage* img) {
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

void PGraphicsOpenGL2::popMatrix() {
    glPopMatrix();
}

void PGraphicsOpenGL2::pushMatrix() {
    glPushMatrix();
}

void PGraphicsOpenGL2::translate(const float x, const float y, const float z) {
    glTranslatef(x, y, z);
}

void PGraphicsOpenGL2::rotateX(const float angle) {
    glRotatef(degrees(angle), 1.0f, 0.0f, 0.0f);
}

void PGraphicsOpenGL2::rotateY(const float angle) {
    glRotatef(degrees(angle), 0.0f, 1.0f, 0.0f);
}

void PGraphicsOpenGL2::rotateZ(const float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphicsOpenGL2::rotate(const float angle) {
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphicsOpenGL2::rotate(const float angle, const float x, const float y, const float z) {
    glRotatef(degrees(angle), x, y, z);
}

void PGraphicsOpenGL2::scale(const float x) {
    glScalef(x, x, x);
}

void PGraphicsOpenGL2::scale(const float x, const float y) {
    glScalef(x, y, 1.0f);
}

void PGraphicsOpenGL2::scale(const float x, const float y, const float z) {
    glScalef(x, y, z);
}

#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
void PGraphicsOpenGL2::beginDraw() {
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
}

void PGraphicsOpenGL2::endDraw() const {
    // restore projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    /* restore state */
    glBindFramebuffer(GL_FRAMEBUFFER, fPreviousFBO); // Restore the previously bound FBO
    glPopMatrix();
    glPopAttrib();
}

void PGraphicsOpenGL2::bind() const {
    glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
}

void PGraphicsOpenGL2::init(uint32_t* pixels, const int width, const int height, int format) {
    this->width        = width;
    this->height       = height;
    framebuffer.width  = width;
    framebuffer.height = height;
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
}
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER
void PGraphicsOpenGL2::hint(const uint16_t property) {
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

void PGraphics::rect(float x, float y, float _width, float _height) const {
}

void PGraphics::line(float x1, float y1, float x2, float y2) const {
}

void PGraphics::bezier(float x1, float y1,
                       float x2, float y2,
                       float x3, float y3,
                       float x4, float y4) const {}

void PGraphics::bezier(float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float x4, float y4, float z4) const {}

void PGraphics::bezierDetail(int detail) {}
void PGraphics::pointSize(float point_size) {}
void PGraphics::point(float x, float y, float z) const {}
void PGraphics::beginShape(int shape) {}
void PGraphics::endShape() {}
void PGraphics::vertex(float x, float y, float z) {}
void PGraphics::vertex(float x, float y, float z, float u, float v) {}
void PGraphics::strokeWeight(float weight) {}
void PGraphics::hint(const uint16_t property) {}

#endif // DISABLE_GRAPHICS
