/*
 * Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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

namespace umfeld {
    class PGraphicsOpenGLv20 final : public PGraphicsOpenGL {
    public:
        explicit PGraphicsOpenGLv20(bool render_to_offscreen);

        /* --- OpenGL 2.0 specific implementation of shared methods --- */

        void emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, bool line_strip_closed) override;
        void emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) override;

        void IMPL_background(float a, float b, float c, float d) override;
        void IMPL_bind_texture(int bind_texture_id) override;
        void IMPL_set_texture(PImage* img) override;

        void render_framebuffer_to_screen(bool use_blit = false) override;
        bool read_framebuffer(std::vector<unsigned char>& pixels) override;
        void store_fbo_state() override;
        void restore_fbo_state() override;
        void bind_fbo() override;
        void finish_fbo() override;

        void upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
        void download_texture(PImage* img) override;

        void beginDraw() override;
        void endDraw() override;

        void reset_mvp_matrices() override;
        void restore_mvp_matrices() override;

        void        init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;
        std::string name() override { return "PGraphicsOpenGLv22"; }

        /* --- standard drawing functions --- */

        void camera() override;
        void camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) override;
        void frustum(float left, float right, float bottom, float top, float near, float far) override;
        void ortho(float left, float right, float bottom, float top, float near, float far) override;
        void perspective(float fovy, float aspect, float near, float far) override;

        void    strokeWeight(float weight) override;
        void    rect(float x, float y, float width, float height) override;
        void    ellipse(float x, float y, float width, float height) override;
        void    circle(float x, float y, float diameter) override;
        void    line(float x1, float y1, float x2, float y2) override;
        void    line(float x1, float y1, float z1, float x2, float y2, float y3) override;
        void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) override;
        void    bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
        void    bezier(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4) override;
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
        void    mesh(VertexBuffer* mesh_shape) override { /* TODO implement */ console("TODO implement mesh"); }
        void    debug_text(const std::string& text, float x, float y) override; // TODO move to PGraphics ( use glBeginShape() )

    private:
        bool                enabled_texture_in_shape{false};
        std::vector<Vertex> outline_vertices;
        GLint               previously_bound_FBO{0};
    };
} // namespace umfeld