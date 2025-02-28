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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PGraphics.h"

namespace umgebung {

    class PGraphicsOpenGLv33 final : public PGraphics {
    public:
        PGraphicsOpenGLv33();

        void    strokeWeight(float weight) override;
        void    background(float a, float b, float c, float d = 1.0f) override;
        void    background(float a) override;
        void    rect(float x, float y, float width, float height) override;
        void    ellipse(float x, float y, float width, float height) override;
        void    circle(float x, float y, float radius) override;
        void    ellipseDetail(int detail) override;
        void    line(float x1, float y1, float x2, float y2) override;
        void    line(float x1, float y1, float z1, float x2, float y2, float z2) override;
        void    linse(float x1, float y1, float x2, float y2);
        void    bezier(float x1, float y1,
                       float x2, float y2,
                       float x3, float y3,
                       float x4, float y4) override;
        void    bezier(float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float x4, float y4, float z4) override;
        void    bezierDetail(int detail) override;
        void    pointSize(float point_size) override;
        void    point(float x, float y, float z = 0.0f) override;
        void    beginShape(int shape = POLYGON) override;
        void    endShape() override;
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
        void    beginDraw() override;
        void    endDraw() override;
        void    bind() override;
        void    init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;

        /* --- additional methods --- */

        // TODO replace `init()` in PImage constructor with `upload_texture(...)`
        static bool upload_texture(PImage* image, bool generate_texture_mipmapped);

        void flush() override {
            flush_fill();
            flush_stroke();
        }

        void reset_matrices() override;

    private:
        const uint8_t  NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV = 9;
        const uint8_t  NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA  = 7;
        const uint32_t VBO_BUFFER_CHUNK_SIZE                  = 1024 * 1024;       // 1MB
        const float    DEFAULT_FOV                            = 2.0f * atan(0.5f); // = 53.1301f;

        struct RenderBatch {
            int    start_index;
            int    num_vertices;
            GLuint texture_id;

            RenderBatch(const int start, const int count, const GLuint texID)
                : start_index(start), num_vertices(count), texture_id(texID) {}
        };

        // TOOD create *vertex buffer client* struct for this
        GLuint             fill_shader_program{};
        GLuint             fill_VAO_xyz_rgba_uv = 0;
        GLuint             fill_VBO_xyz_rgba_uv = 0;
        std::vector<float> fill_vertices_xyz_rgba_uv;
        uint32_t           fill_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)

        GLuint             stroke_shader_program{};
        GLuint             stroke_VAO_xyz_rgba{};
        GLuint             stroke_VBO_xyz_rgba = 0;
        std::vector<float> stroke_vertices_xyz_rgba;
        uint32_t           stroke_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)

        GLuint                   dummyTexture{};
        std::vector<RenderBatch> renderBatches;
        glm::mat4                currentMatrix;
        std::vector<glm::mat4>   matrixStack;
        float                    aspectRatio;
        glm::mat4                projection2D{};
        glm::mat4                projection3D{};
        glm::mat4                viewMatrix{};
        float                    fStrokeWeight{1};
        int                      fBezierDetail{20};
        int                      fPreviousFBO{};
        bool                     render_lines_as_quads{true};

        // static constexpr int ELLIPSE_NUM_SEGMENTS = 32;
        //        PFont* fCurrentFont           = nullptr;
        //        float  fPointSize             = 1;
        //        float  fStrokeWeight          = 1;
        //        bool   fEnabledTextureInShape = false;
        //        bool   fShapeBegun            = false;
        //        int    fEllipseDetail         = 32;
        //        int    fPixelDensity          = 1;

        static const char* vertex_shader_source_texture();
        static const char* fragment_shader_source_texture();
        static const char* vertex_shader_source_simple();
        static const char* fragment_shader_source_simple();

        void flush_stroke();
        void flush_fill();
        void fill_resize_buffer(uint32_t newSize);
        void init_stroke_vertice_buffers();
        void init_fill_vertice_buffers();
        void createDummyTexture();

        void add_transformed_fill_vertex_xyz_rgba_uv(const glm::vec3& position, const glm::vec4& color, float u = 0.0f, float v = 0.0f);
        void add_texture_id_to_render_batch(const std::vector<float>& vertices, int num_vertices, GLuint texture_id);
        void to_screen_space(glm::vec3& world_position) const;

        // TODO remove these:
        void add_fill_vertex_xyz_rgba_uv(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords);
        void add_fill_vertex_xyz_rgba_uv_raw(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords);
        void add_fill_vertex_xyz_rgba_uv(float x, float y, float z,
                                         float r, float g, float b, float a = 1.0f,
                                         float u = 0.0f, float v = 0.0f);
        void add_stroke_vertex_xyz_rgba(float x, float y, float z,
                                        float r, float g, float b, float a = 1.0f);
        // void add_quad_line(const glm::vec3& p1,
        //                    const glm::vec3& p2,
        //                    const glm::vec4& color,
        //                    float            width,
        //                    const glm::mat4& mvp);

        static GLuint build_shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        static void   checkShaderCompileStatus(GLuint shader);
        static void   checkProgramLinkStatus(GLuint program);
        static void   printMatrix(const glm::mat4& matrix);
    };
} // namespace umgebung