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
        void    circle(float x, float y, float diameter) override;
        void    ellipseDetail(int detail) override;
        void    line(float x1, float y1, float x2, float y2) override;
        void    line(float x1, float y1, float z1, float x2, float y2, float z2) override;
        void    linse(float x1, float y1, float x2, float y2);
        void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) override;
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
        void    beginDraw() override;
        void    endDraw() override;
        void    bind() override;
        void    init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;

        /* --- additional methods --- */

        // TODO replace `init()` in PImage constructor with `upload_texture(...)`
        static bool upload_texture(PImage* image, bool generate_texture_mipmapped);

        void prepare_frame() {
            if (render_mode == RENDER_MODE_IMMEDIATE) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glUseProgram(stroke_shader_program);

                // Upload matrices
                const GLint projLoc = glGetUniformLocation(stroke_shader_program, "uProjection");
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection3D)); // or projection2D

                const GLint viewLoc = glGetUniformLocation(stroke_shader_program, "uViewMatrix");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

                const GLint matrixLoc = glGetUniformLocation(stroke_shader_program, "uModelMatrix");
                glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix_shader));
            }
        }

        void flush() override {
            if (render_mode == RENDER_MODE_RETAINED) {
                flush_fill();
                flush_stroke();
                return;
            }
            if (render_mode == RENDER_MODE_IMMEDIATE) {
                /* TODO this should happen at the beginning of the frame */
                prepare_frame();
                return;
            }
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

        struct Vertex {
            glm::vec3 postition;
            glm::vec4 color;
            glm::vec2 texture;
        };

        static constexpr uint8_t RENDER_MODE_IMMEDIATE = 0;
        static constexpr uint8_t RENDER_MODE_RETAINED  = 1;
        uint8_t                  render_mode           = RENDER_MODE_IMMEDIATE;

        static constexpr uint8_t RENDER_LINE_AS_QUADS_SEGMENTS                    = 0;
        static constexpr uint8_t RENDER_LINE_AS_QUADS_SEGMENTS_WITH_ROUND_CORNERS = 1;
        static constexpr uint8_t RENDER_LINE_AS_QUADS_WITH_POINTY_CORNERS         = 2;
        uint8_t                  line_mode                                        = RENDER_LINE_AS_QUADS_SEGMENTS;

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

        GLuint                   texture_id_solid_color_default{};
        std::vector<RenderBatch> renderBatches;
        glm::mat4                model_matrix_client;
        glm::mat4                model_matrix_shader;
        std::vector<glm::mat4>   matrixStack;
        float                    aspectRatio;
        glm::mat4                projection2D{};
        glm::mat4                projection3D{};
        glm::mat4                viewMatrix{};
        float                    fStrokeWeight{1};
        int                      fBezierDetail{20};
        int                      fEllipseDetail{32};
        int                      fPreviousFBO{};
        bool                     render_lines_as_quads{true};
        std::vector<glm::vec3>   shape_stroke_vertex_cache{VBO_BUFFER_CHUNK_SIZE}; // TODO maybe add color?
        std::vector<Vertex>      shape_fill_vertex_cache{VBO_BUFFER_CHUNK_SIZE};
        int                      shape_mode_cache = POLYGON;

        // static constexpr int ELLIPSE_NUM_SEGMENTS = 32;
        //        PFont* fCurrentFont           = nullptr;
        //        float  fPointSize             = 1;
        //        float  fStrokeWeight          = 1;
        //        bool   fEnabledTextureInShape = false;
        //        bool   fShapeBegun            = false;
        //        int    fPixelDensity          = 1;

        static const char* vertex_shader_source_texture();
        static const char* fragment_shader_source_texture();
        static const char* vertex_shader_source_simple();
        static const char* fragment_shader_source_simple();

        /* --- RENDER_MODE_IMMEDIATE (IM) --- */

        struct IM_primitive {
            const uint32_t num_vertices;
            GLuint         VAO{0};
            GLuint         VBO{0};
            explicit IM_primitive(const uint32_t vertices) : num_vertices(vertices) {}
            bool uninitialized() const {
                return VAO == 0 || VBO == 0;
            }
        };

        IM_primitive IM_primitive_line{6};
        IM_primitive IM_primitive_shape{VBO_BUFFER_CHUNK_SIZE};

        std::vector<Vertex> convertQuadsToTriangles(const std::vector<Vertex>& quads) const;
        std::vector<Vertex> convertPolygonToTriangleFan(const std::vector<Vertex>& polygon) const;

        void IM_init_primitive(IM_primitive& primitive) const;
        void IM_render_point(float x1, float y1, float z1);
        void IM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void IM_render_begin_shape(int shape);
        void IM_render_end_shape(bool close_shape);
        void IM_render_shape(IM_primitive& primitive, GLenum mode, const std::vector<Vertex>& shape_fill_vertices) const;

        // ... triangle ( + textured ), quad ( + textured ), circle, etcetera

        /* --- RENDER_MODE_RETAINED (RM) --- */

        void flush_stroke();
        void flush_fill();
        void fill_resize_buffer(uint32_t newSize);
        void init_stroke_vertice_buffers();
        void init_fill_vertice_buffers();
        void createDummyTexture();

        void add_transformed_fill_vertex_xyz_rgba_uv(const glm::vec3& position, const glm::vec4& color, float u = 0.0f, float v = 0.0f);
        void add_texture_id_to_render_batch(const std::vector<float>& vertices, int num_vertices, GLuint texture_id);
        void to_screen_space(glm::vec3& world_position) const;
        void render_line_strip_as_connected_quads(std::vector<glm::vec3>& points, const glm::vec4& color, bool close_shape);
        void render_line_strip_as_quad_segments(const std::vector<glm::vec3>& points, const glm::vec4& color, bool close_shape, bool round_corners);
        void draw_filled_ellipse(float x, float y, float width, float height, int detail, const glm::vec4& color);

        // TODO remove these:
        void          add_fill_vertex_xyz_rgba_uv(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords);
        void          add_fill_vertex_xyz_rgba_uv_raw(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords);
        void          add_fill_vertex_xyz_rgba_uv(float x, float y, float z,
                                                  float r, float g, float b, float a = 1.0f,
                                                  float u = 0.0f, float v = 0.0f);
        void          add_stroke_vertex_xyz_rgba(float x, float y, float z,
                                                 float r, float g, float b, float a = 1.0f);
        static GLuint build_shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        static void   checkShaderCompileStatus(GLuint shader);
        static void   checkProgramLinkStatus(GLuint program);
        static void   printMatrix(const glm::mat4& matrix);
    };
} // namespace umgebung
