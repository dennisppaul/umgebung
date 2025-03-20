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
#include <GL/glew.h>

#include "PGraphicsOpenGL.h"
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

PGraphicsOpenGLv20::PGraphicsOpenGLv20(const bool render_to_offscreen) : PImage(0, 0, 0) {
    this->render_to_offscreen = render_to_offscreen;
}

void PGraphicsOpenGLv20::strokeWeight(const float weight) {
    stroke_weight = weight;
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
    if (color_fill.active) {
        glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(x, y);
        glTexCoord2f(1, 0);
        glVertex2f(x + width, y);
        glTexCoord2f(1, 1);
        glVertex2f(x + width, y + height);
        glTexCoord2f(0, 1);
        glVertex2f(x, y + height);
        glEnd();
    }
    if (color_stroke.active) {
        glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
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
    if (color_fill.active) {
        glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        draw_ellipse(GL_TRIANGLE_FAN, ellipse_detail, x, y, width, height);
    }

    if (color_stroke.active) {
        glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        draw_ellipse(GL_LINE_LOOP, ellipse_detail, x, y, width, height);
    }
}

void PGraphicsOpenGLv20::circle(const float x, const float y, const float diameter) {
    ellipse(x, y, diameter, diameter);
}

void PGraphicsOpenGLv20::line(const float x1, const float y1, const float x2, const float y2) {
    if (!color_stroke.active) {
        return;
    }
    glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void PGraphicsOpenGLv20::line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
    if (!color_stroke.active) {
        return;
    }
    glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void PGraphicsOpenGLv20::triangle(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, const float x3, const float y3, const float z3) {
    if (!color_stroke.active) {
        glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        glBegin(GL_TRIANGLES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);
        glEnd();
        return;
    }
    if (!color_fill.active) {
        glColor4f(color_fill.r, color_fill.g, color_fill.b, color_stroke.a);
        glBegin(GL_TRIANGLES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);
        glEnd();
        return;
    }
}

void PGraphicsOpenGLv20::bezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4) {
    if (!color_stroke.active) {
        return;
    }
    glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);

    const int   segments = bezier_detail;
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
    if (!color_stroke.active) {
        return;
    }
    glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);

    const int   segments = bezier_detail;
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
    bezier_detail = detail;
}

void PGraphicsOpenGLv20::pointSize(const float size) {
    point_size = size;
}

void PGraphicsOpenGLv20::point(const float x, const float y, const float z) {
    if (!color_stroke.active) {
        return;
    }

    // TODO maybe implement point by rect
    glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);

    //    glPushMatrix();
    //    translate(x - point_size * 0.5, y - point_size * 0.5, z);
    //    rect(0, 0, point_size, point_size);
    //    glPopMatrix();

    const float tmp_point_size = std::max(std::min(point_size, open_gl_capabilities.point_size_max), open_gl_capabilities.point_size_min);

    glPointSize(tmp_point_size); // @development when using antialiasing a point size of 1 produces a semitransparent point
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void PGraphicsOpenGLv20::beginShape(const int shape) {
    // TODO collect vertices for outline
    outline_vertices.clear();

    shape_has_begun = true;

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
    if (shape_has_begun) {
        glEnd();
        shape_has_begun = false;
    }
    if (enabled_texture_in_shape) {
        glDisable(GL_TEXTURE_2D);
        enabled_texture_in_shape = false;
    }

    /* --- stroke --- */

    glLineWidth(stroke_weight * pixel_density);
    if (!outline_vertices.empty()) {
        if (close_shape) {
            outline_vertices.push_back(outline_vertices[0]);
        }
        glBegin(GL_LINE_STRIP);
        for (const auto v: outline_vertices) {
            glColor4f(v.color.r, v.color.g, v.color.b, v.color.a);
            glVertex3f(v.position.x, v.position.y, v.position.z);
        }
        glEnd();
        outline_vertices.clear();
    }
}

void PGraphicsOpenGLv20::vertex(const float x, const float y, const float z) {
    if (!shape_has_begun) {
        console("`vertex()` should only be called between `beginShape()` and `endShape()`");
        return;
    }
    if (color_fill.active) {
        glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        glVertex3f(x, y, z);
    }

    if (color_stroke.active) {
        outline_vertices.emplace_back(x, y, z,
                                      color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a,
                                      0, 0);
    }
}


void PGraphicsOpenGLv20::vertex(const float x, const float y, const float z, const float u, const float v) {
    if (!shape_has_begun) {
        console("`vertex()` should only be called between `beginShape()` and `endShape()`");
        return;
    }
    if (color_fill.active) {
        glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        glTexCoord2f(u, v);
        glVertex3f(x, y, z);
    }

    if (color_stroke.active) {
        outline_vertices.emplace_back(x, y, z,
                                      color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a,
                                      u, v);
    }
}

/* font */

PFont* PGraphicsOpenGLv20::loadFont(const std::string& file, const float size) {
    auto* font = new PFont(file.c_str(), size, static_cast<float>(pixel_density));
    return font;
}

void PGraphicsOpenGLv20::textFont(PFont* font) {
    current_font = font;
}

void PGraphicsOpenGLv20::textSize(const float size) {
    if (current_font == nullptr) {
        return;
    }
    current_font->textSize(size);
}

void PGraphicsOpenGLv20::text_str(const std::string& text, const float x, const float y, const float z) {
    if (current_font == nullptr) {
        return;
    }
    if (!color_fill.active) {
        return;
    }

#ifndef DISABLE_GRAPHICS
    glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
    current_font->draw(this, text, x, y, z);
#endif // DISABLE_GRAPHICS
}

float PGraphicsOpenGLv20::textWidth(const std::string& text) {
    if (current_font == nullptr) {
        return 0;
    }

#ifndef DISABLE_GRAPHICS
    return current_font->textWidth(text.c_str());
#endif // DISABLE_GRAPHICS
}

void PGraphicsOpenGLv20::pixelDensity(const int density) {
    static bool emitted_warning = false;
    if (!emitted_warning && init_properties_locked) {
        warning("`pixelDensity()` should not be set after context is created. use `retina_support` in settings instead.");
        emitted_warning = true;
    }
    pixel_density = density;
}

void PGraphicsOpenGLv20::text(const char* value, const float x, const float y, const float z) {
    text_str(value, x, y, z);
}

PImage* PGraphicsOpenGLv20::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}


static bool OGL_generate_and_upload_image_as_texture(PImage* image, const bool generate_texture_mipmapped) {
    // TODO merge with method in PGraphicsOpenGLv33
    if (image->pixels == nullptr) {
        error("Failed to upload texture because pixels are null");
        return false;
    }

    GLuint mTextureID;
    glGenTextures(1, &mTextureID);

    if (mTextureID == 0) {
        error("Failed to generate texture ID");
        return false;
    }

    image->texture_id = static_cast<int>(mTextureID);
    glBindTexture(GL_TEXTURE_2D, image->texture_id);

    // Set texture parameters
    if (generate_texture_mipmapped) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Load image data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 static_cast<GLint>(image->width),
                 static_cast<GLint>(image->height),
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE,
                 image->pixels);
    if (generate_texture_mipmapped) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    return true;
}

void PGraphicsOpenGLv20::image(PImage* img, const float x, const float y, float w, float h) {
#ifndef DISABLE_GRAPHICS
    if (!color_fill.active) {
        return;
    }

    if (img == nullptr) {
        error("image(...) / image is null");
        return;
    }

    if (w < 0) {
        w = img->width;
    }
    if (h < 0) {
        h = img->height;
    }

    // TODO move this to own method and share with `texture()`
    if (img->texture_id == TEXTURE_NOT_GENERATED) {
        OGL_generate_and_upload_image_as_texture(img, true);
        if (img->texture_id == TEXTURE_NOT_GENERATED) {
            error("image cannot create texture.");
            return;
        }
        console("PGraphicsOpenGLv20::image // uploaded texture image to GPU: ", img->texture_id);
    }

    const uint8_t tmp_rect_mode          = rect_mode;
    const uint8_t tmp_texture_id_current = texture_id_current; // TODO

    glEnable(GL_TEXTURE_2D);
    glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
    glBindTexture(GL_TEXTURE_2D, img->texture_id);

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

    if (tmp_texture_id_current != img->texture_id) {
        glBindTexture(GL_TEXTURE_2D, tmp_texture_id_current);
    }
    rect_mode = tmp_rect_mode;

#endif // DISABLE_GRAPHICS
}

void PGraphicsOpenGLv20::image(PImage* img, const float x, const float y) {
    image(img, x, y, img->width, img->height);
}

void PGraphicsOpenGLv20::texture(PImage* img) {
    if (img == nullptr) {
        IMPL_bind_texture(TEXTURE_NONE);
        return;
    }

    if (shape_has_begun) {
        console("`texture()` can only be called right before `beginShape(...)`. ( note, this is different from the original processing )");
        return;
    }

    // TODO move this to own method and share with `image()`
    if (img->texture_id == TEXTURE_NOT_GENERATED) {
        OGL_generate_and_upload_image_as_texture(img, true);
        if (img->texture_id == TEXTURE_NOT_GENERATED) {
            error("image cannot create texture.");
            return;
        }
        console("PGraphicsOpenGLv20::texture // uploaded texture image to GPU: ", img->texture_id);
    }

    enabled_texture_in_shape = true;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, img->texture_id);
}

void PGraphicsOpenGLv20::popMatrix() {
    PGraphics::popMatrix();
    glPopMatrix();
}

void PGraphicsOpenGLv20::pushMatrix() {
    PGraphics::pushMatrix();
    glPushMatrix();
}

void PGraphicsOpenGLv20::translate(const float x, const float y, const float z) {
    PGraphics::translate(x, y, z);
    glTranslatef(x, y, z);
}

void PGraphicsOpenGLv20::rotateX(const float angle) {
    PGraphics::rotateX(angle);
    glRotatef(degrees(angle), 1.0f, 0.0f, 0.0f);
}

void PGraphicsOpenGLv20::rotateY(const float angle) {
    PGraphics::rotateY(angle);
    glRotatef(degrees(angle), 0.0f, 1.0f, 0.0f);
}

void PGraphicsOpenGLv20::rotateZ(const float angle) {
    PGraphics::rotateZ(angle);
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphicsOpenGLv20::rotate(const float angle) {
    PGraphics::rotate(angle);
    glRotatef(degrees(angle), 0.0f, 0.0f, 1.0f);
}

void PGraphicsOpenGLv20::rotate(const float angle, const float x, const float y, const float z) {
    PGraphics::rotate(angle, x, y, z);
    glRotatef(degrees(angle), x, y, z);
}

void PGraphicsOpenGLv20::scale(const float x) {
    PGraphics::scale(x);
    glScalef(x, x, x);
}

void PGraphicsOpenGLv20::scale(const float x, const float y) {
    PGraphics::scale(x, y);
    glScalef(x, y, 1.0f);
}

void PGraphicsOpenGLv20::scale(const float x, const float y, const float z) {
    PGraphics::scale(x, y, z);
    glScalef(x, y, z);
}

void PGraphicsOpenGLv20::init(uint32_t* pixels, const int width, const int height, int format, bool generate_mipmap) {
    this->width        = width;
    this->height       = height;
    framebuffer.width  = width;
    framebuffer.height = height;
    if (render_to_offscreen) {
        glGenFramebuffers(1, &framebuffer.id);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
        glGenTextures(1, &framebuffer.texture_id);
        glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture_id, 0);
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
}

void PGraphicsOpenGLv20::hint(const uint16_t property) {
    // TODO @MERGE
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

void PGraphicsOpenGLv20::upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) {
    error("`upload_texture` not implemented ( might be called from `PImage`, `Capture` ,or `Movie` )");
}

void PGraphicsOpenGLv20::download_texture(PImage* img) {
    error("`download_texture` not implemented ( might be called from `PImage`, `Capture` ,or `Movie` )");
}

#ifndef DISABLE_GRAPHICS
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
