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
#include <glm/gtc/type_ptr.hpp>

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
    PGraphics::strokeWeight(weight);
    glLineWidth(weight);
}

void PGraphicsOpenGLv20::rect(const float x, const float y, const float width, const float height) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        // compute rectangle corners using glm::vec2
        glm::vec2 p1, p2;
        switch (rect_mode) {
            case CORNERS:
                p1 = {x, y};
                p2 = {width, height};
                break;
            case CENTER:
                p1 = {x - width * 0.5f, y - height * 0.5f};
                p2 = {x + width * 0.5f, y + height * 0.5f};
                break;
            case RADIUS:
                p1 = {x - width, y - height};
                p2 = {x + width, y + height};
                break;
            case CORNER:
            default:
                p1 = {x, y};
                p2 = {x + width, y + height};
                break;
        }
        // add rectMode
        if (color_fill.active) {
            glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(p1.x, p1.y);
            glTexCoord2f(1, 0);
            glVertex2f(p2.x, p1.y);
            glTexCoord2f(1, 1);
            glVertex2f(p2.x, p2.y);
            glTexCoord2f(0, 1);
            glVertex2f(p1.x, p2.y);
            glEnd();
        }
        if (color_stroke.active) {
            glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
            glBegin(GL_LINE_LOOP);
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p1.y);
            glVertex2f(p2.x, p2.y);
            glVertex2f(p1.x, p2.y);
            glEnd();
        }
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::rect(x, y, width, height);
        return;
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

void PGraphicsOpenGLv20::ellipse(const float x, const float y, const float w, const float h) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        float cx, cy, width, height;

        switch (ellipse_mode) {
            case CENTER:
                cx     = x;
                cy     = y;
                width  = w;
                height = h;
                break;
            case RADIUS:
                cx     = x;
                cy     = y;
                width  = w * 2.0f;
                height = h * 2.0f;
                break;
            case CORNER:
                cx     = x + w * 0.5f;
                cy     = y + h * 0.5f;
                width  = w;
                height = h;
                break;
            case CORNERS:
                cx     = (x + w) * 0.5f;
                cy     = (y + h) * 0.5f;
                width  = std::abs(w - x);
                height = std::abs(h - y);
                break;
            default:
                cx     = x;
                cy     = y;
                width  = w;
                height = h;
                break;
        }

        if (color_fill.active) {
            glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
            draw_ellipse(GL_TRIANGLE_FAN, ellipse_detail, cx, cy, width, height);
        }

        if (color_stroke.active) {
            glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a); // fix: use stroke color!
            draw_ellipse(GL_LINE_LOOP, ellipse_detail, cx, cy, width, height);
        }
        return;
    }

    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::ellipse(x, y, w, h); // uses the other version with full shape mode
        return;
    }
}

void PGraphicsOpenGLv20::circle(const float x, const float y, const float diameter) {
    ellipse(x, y, diameter, diameter);
}

void PGraphicsOpenGLv20::line(const float x1, const float y1, const float x2, const float y2) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        if (!color_stroke.active) {
            return;
        }
        glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::line(x1, y1, x2, y2);
        return;
    }
}

void PGraphicsOpenGLv20::line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        if (!color_stroke.active) {
            return;
        }
        glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glEnd();
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::line(x1, y1, z1, x2, y2, z2);
        return;
    }
}

void PGraphicsOpenGLv20::triangle(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, const float x3, const float y3, const float z3) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        if (!color_stroke.active) {
            glColor4f(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
            glBegin(GL_TRIANGLES);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
            glEnd();
        }
        if (!color_fill.active) {
            glColor4f(color_fill.r, color_fill.g, color_fill.b, color_stroke.a);
            glBegin(GL_TRIANGLES);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
            glEnd();
        }
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3);
        return;
    }
}

void PGraphicsOpenGLv20::bezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
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
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::bezier(x1, y1, x2, y2, x3, y3, x4, y4);
        return;
    }
}

void PGraphicsOpenGLv20::bezier(const float x1, const float y1, const float z1,
                                const float x2, const float y2, const float z2,
                                const float x3, const float y3, const float z3,
                                const float x4, const float y4, const float z4) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {

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
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::bezier(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
        return;
    }
}

void PGraphicsOpenGLv20::point(const float x, const float y, const float z) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
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
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::point(x, y, z);
        return;
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

void PGraphicsOpenGLv20::image(PImage* img, const float x, const float y, float w, float h) {
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

    // compute rectangle corners using glm::vec2
    glm::vec2 p1, p2;
    switch (rect_mode) {
        case CORNERS:
            p1 = {x, y};
            p2 = {w, h};
            break;
        case CENTER:
            p1 = {x - w * 0.5f, y - h * 0.5f};
            p2 = {x + w * 0.5f, y + h * 0.5f};
            break;
        case RADIUS:
            p1 = {x - w, y - h};
            p2 = {x + w, y + h};
            break;
        case CORNER:
        default:
            p1 = {x, y};
            p2 = {x + w, y + h};
            break;
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
    glBindTexture(GL_TEXTURE_2D, img->texture_id);
    glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(p1.x, p1.y);
    glTexCoord2f(1, 0);
    glVertex2f(p2.x, p1.y);
    glTexCoord2f(1, 1);
    glVertex2f(p2.x, p2.y);
    glTexCoord2f(0, 1);
    glVertex2f(p1.x, p2.y);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    if (tmp_texture_id_current != img->texture_id) {
        glBindTexture(GL_TEXTURE_2D, tmp_texture_id_current);
    }
    rect_mode = tmp_rect_mode;
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

// TODO could move this to a shared method in `PGraphics` and use beginShape(TRIANGLES)
void PGraphicsOpenGLv20::debug_text(const std::string& text, const float x, const float y) {
    const std::vector<Vertex> triangle_vertices = debug_font.generate(text, x, y, glm::vec4(color_fill));
    push_texture_id();
    glEnable(GL_TEXTURE_2D);
    IMPL_bind_texture(debug_font.textureID);
    glBegin(GL_TRIANGLES);
    for (const auto& vertex: triangle_vertices) {
        glColor4f(vertex.color.r, vertex.color.g, vertex.color.b, vertex.color.a);
        glTexCoord2f(vertex.tex_coord.x, vertex.tex_coord.y);
        glVertex2f(vertex.position.x, vertex.position.y);
        // OGL3_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_TRIANGLES, triangle_vertices);
    }
    glEnd();
    pop_texture_id();
    glDisable(GL_TEXTURE_2D);
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

void PGraphicsOpenGLv20::init(uint32_t* pixels,
                              const int width,
                              const int height,
                              int       format,
                              bool      generate_mipmap) {
    this->width        = width;
    this->height       = height;
    framebuffer.width  = width;
    framebuffer.height = height;

    if (render_to_offscreen) {
        console("creating offscreen buffer.");
        console("framebuffer: ", framebuffer.width, "×", framebuffer.height);
        console("graphics   : ", this->width, "×", this->height);

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
            std::cerr << "ERROR Framebuffer is not complete!" << std::endl;
        }
        glViewport(0, 0, framebuffer.width, framebuffer.height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        texture_id = framebuffer.texture_id; // TODO maybe get rid of one of the texture_id variables
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

void PGraphicsOpenGLv20::render_framebuffer_to_screen(bool use_blit) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    const float viewport_width  = framebuffer.width;
    const float viewport_height = framebuffer.height;
    const float ortho_width     = width;
    const float ortho_height    = height;

    glViewport(0, 0, viewport_width, viewport_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ortho_width, 0, ortho_height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    bind_framebuffer_texture();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(0, 0);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(ortho_width, 0);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(ortho_width, ortho_height);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(0, ortho_height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopAttrib();
}

bool PGraphicsOpenGLv20::read_framebuffer(std::vector<unsigned char>& pixels) {
    store_fbo_state();
    if (render_to_offscreen) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id); // non-MSAA FBO or default
    }
    const bool success = OGL_read_framebuffer(framebuffer, pixels);
    restore_fbo_state();
    return success;
}

void PGraphicsOpenGLv20::reset_mvp_matrices() {
    PGraphics::reset_mvp_matrices();

    // glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    // glLoadIdentity();
    // glOrtho(0, framebuffer.width, 0, framebuffer.height, -depth_range, depth_range);
    //
    // glMatrixMode(GL_MODELVIEW);
    // glPushMatrix();
    // glLoadIdentity();

    /* store MVP matrices */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection_matrix));

    // modelview matrix
    const glm::mat4 modelview = view_matrix * model_matrix;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(modelview));

    // /** flip y axis */
    // glScalef(1, -1, 1);
    // glTranslatef(0, -height, 0);
}

void PGraphicsOpenGLv20::restore_mvp_matrices() {
    PGraphics::restore_mvp_matrices();

    /* restore MVP matrices */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void PGraphicsOpenGLv20::beginDraw() {
    if (render_to_offscreen) {
        store_fbo_state();
    }
    PGraphicsOpenGL::beginDraw();
    if (!render_to_offscreen) {
        set_default_graphics_state();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -depth_range, depth_range);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glScalef(1, -1, 1);
        glTranslatef(0, -height, 0);
    }
}

void PGraphicsOpenGLv20::endDraw() {
    PGraphicsOpenGL::endDraw();
}

void PGraphicsOpenGLv20::bind_fbo() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
}

void PGraphicsOpenGLv20::finish_fbo() {
    glPopMatrix();
    glPopAttrib();
}

void PGraphicsOpenGLv20::store_fbo_state() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previously_bound_FBO);
}

void PGraphicsOpenGLv20::restore_fbo_state() {
    glBindFramebuffer(GL_FRAMEBUFFER, previously_bound_FBO);
}

void PGraphicsOpenGLv20::IMPL_background(const float a, const float b, const float c, const float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PGraphicsOpenGLv20::IMPL_bind_texture(const int bind_texture_id) {
    if (bind_texture_id != texture_id_current) {
        texture_id_current = bind_texture_id;
        glBindTexture(GL_TEXTURE_2D, texture_id_current); // NOTE this should be the only glBindTexture ( except for initializations )
    }
}

void PGraphicsOpenGLv20::IMPL_set_texture(PImage* img) {
    // if (img == nullptr) {
    //     IMPL_bind_texture(texture_id_solid_color);
    //     return;
    // }
    // NOTE identical >>>
    if (shape_has_begun) {
        console("`texture()` can only be called right before `beginShape(...)`. ( note, this is different from the original processing )");
        return;
    }

    // TODO move this to own method and share with `texture()`
    if (img->texture_id == TEXTURE_NOT_GENERATED) {
        OGL_generate_and_upload_image_as_texture(img, true);
        if (img->texture_id == TEXTURE_NOT_GENERATED) {
            error("image cannot create texture.");
            return;
        }
    }

    IMPL_bind_texture(img->texture_id);
    // TODO so this is interesting: we could leave the texture bound and require the client
    //      to unbind it with `texture_unbind()` or should `endShape()` always reset to
    //      `texture_id_solid_color` with `texture_unbind()`.
    // NOTE identical <<<
}

/* --- shape --- */

void PGraphicsOpenGLv20::beginShape(const int shape) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        outline_vertices.clear();
        shape_has_begun  = true;
        const int mShape = get_draw_mode(shape);
        glBegin(mShape);
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::beginShape(shape);
        return;
    }
}

void PGraphicsOpenGLv20::endShape(const bool close_shape) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
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
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::endShape(close_shape);
        return;
    }
}

void PGraphicsOpenGLv20::vertex(const float x, const float y, const float z) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {

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
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::vertex(x, y, z);
        return;
    }
}

void PGraphicsOpenGLv20::vertex(const float x, const float y, const float z, const float u, const float v) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
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
        return;
    }
    if (render_mode == RENDER_MODE_SHAPE) {
        PGraphics::vertex(x, y, z, u, v);
        return;
    }
}

static void emit_vertex(const Vertex& v) {
    glColor4f(v.color.r, v.color.g, v.color.b, v.color.a);
    glTexCoord2f(v.tex_coord.x, v.tex_coord.y);
    glNormal3f(v.normal.x, v.normal.y, v.normal.z);
    glVertex3f(v.position.x, v.position.y, v.position.z);
}

void PGraphicsOpenGLv20::emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, const bool line_strip_closed) {
    glBegin(GL_LINE_STRIP);
    for (const auto& v: line_strip_vertices) {
        emit_vertex(v);
    }
    if (line_strip_closed) {
        emit_vertex(line_strip_vertices[0]);
    }
    glEnd();
}

void PGraphicsOpenGLv20::emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) {
    glBegin(GL_TRIANGLES);
    for (const auto& v: triangle_vertices) {
        emit_vertex(v);
    }
    glEnd();
}

void PGraphicsOpenGLv20::camera(const float eyeX, const float eyeY, const float eyeZ,
                                const float centerX, const float centerY, const float centerZ,
                                const float upX, const float upY, const float upZ) {
    PGraphics::camera(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void PGraphicsOpenGLv20::camera() {
    PGraphics::camera();
}

void PGraphicsOpenGLv20::frustum(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    PGraphics::frustum(left, right, bottom, top, near, far);
}

void PGraphicsOpenGLv20::ortho(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    PGraphics::ortho(left, right, bottom, top, near, far);
}

void PGraphicsOpenGLv20::perspective(const float fovy, const float aspect, const float near, const float far) {
    PGraphics::perspective(fovy, aspect, near, far);
}
