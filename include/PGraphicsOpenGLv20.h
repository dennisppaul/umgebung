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

#pragma once

#include "PGraphicsOpenGL.h"

namespace umgebung {
    class PGraphicsOpenGLv20 final : public PGraphicsOpenGL {
        // TODO clean this up … move methods to implementation file
    public:
        void reset_matrices() override {
            PGraphics::reset_matrices();

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(0, framebuffer.width, 0, framebuffer.height, -depth_range, depth_range);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            /** flip y axis */
            glScalef(1, -1, 1);
            glTranslatef(0, -height, 0);

            glViewport(0, 0, framebuffer.width, framebuffer.height);
        }

        void restore_matrices() override {
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }

        void prepare_frame() override {
            set_default_graphics_state();
        }

        void setup_fbo() override {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix();
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
        }

        void finish_fbo() override {
            glPopMatrix();
            glPopAttrib();
        }

        void render_framebuffer_to_screen(bool use_blit = false) override {
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
            glVertex2f(static_cast<float>(framebuffer.width), 0);
            glTexCoord2f(1.0, 1.0);
            glVertex2f(static_cast<float>(framebuffer.width),
                       static_cast<float>(framebuffer.height));
            glTexCoord2f(0.0, 1.0);
            glVertex2f(0, static_cast<float>(framebuffer.height));
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glPopAttrib();
        }

        explicit PGraphicsOpenGLv20(bool render_to_offscreen);

        void    init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;
        void    strokeWeight(float weight) override;
        void    background(float a, float b, float c, float d = 1.0f) override;
        void    background(float a) override;
        void    rect(float x, float y, float width, float height) override;
        void    ellipse(float x, float y, float width, float height) override;
        void    circle(float x, float y, float radius) override;
        void    line(float x1, float y1, float x2, float y2) override;
        void    line(float x1, float y1, float z1, float x2, float y2, float y3) override;
        void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) override;
        void    bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
        void    bezier(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4) override;
        void    bezierDetail(int detail) override;
        void    pointSize(float size) override;
        void    point(float x, float y, float z = 0.0f) override;
        void    beginShape(int shape = POLYGON) override;
        void    endShape(bool close_shape = false) override;
        void    vertex(float x, float y, float z = 0.0f) override;
        void    vertex(float x, float y, float z, float u, float v) override;
        PFont*  loadFont(const std::string& file, float size) override;
        void    textFont(PFont* font) override;
        void    textSize(float size) override;
        void    text(const char* value, float x, float y, float z = 0.0f) override;
        float   textWidth(const std::string& text) override;
        PImage* loadImage(const std::string& filename) override;
        void    image(PImage* img, float x, float y, float w, float h) override;
        void    image(PImage* img, float x, float y) override;
        void    texture(PImage* img) override;
        void    popMatrix() override;
        void    pushMatrix() override;
        void    translate(float x, float y, float z = 0.0f) override;
        void    rotateX(float angle) override;
        void    rotateY(float angle) override;
        void    rotateZ(float angle) override;
        void    rotate(float angle) override;
        void    rotate(float angle, float x, float y, float z) override;
        void    scale(float x) override;
        void    scale(float x, float y) override;
        void    scale(float x, float y, float z) override;
        void    pixelDensity(int density) override;
        void    hint(uint16_t property) override;
        void    text_str(const std::string& text, float x, float y, float z = 0.0f) override;
        void    mesh(PMesh* mesh_shape) override { /* TODO implement */ }

        /* --- additional methods --- */

        void emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, bool line_strip_closed) override {}
        void emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) override {}

        void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
        void        download_texture(PImage* img) override;
        std::string name() override { return "PGraphicsOpenGLv22"; }

    private:
        bool                enabled_texture_in_shape = false;
        std::vector<Vertex> outline_vertices;
    };
} // namespace umgebung