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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PGraphicsOpenGL.h"
#include "UFont.h"

namespace umgebung {

    class PGraphicsOpenGLv33 final : public PGraphicsOpenGL {
    public:
        void IMPL_background(float a, float b, float c, float d) override;
        void IMPL_bind_texture(int bind_texture_id) override;
        void IMPL_set_texture(PImage* img) override;

        explicit PGraphicsOpenGLv33(bool render_to_offscreen);

        void        restore_matrices() override {}
        void        reset_matrices() override;
        void        prepare_frame() override;
        void        setup_fbo() override;
        void        finish_fbo() override {}
        void        endDraw() override;
        void        render_framebuffer_to_screen(bool use_blit = false) override;
        void        init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;
        void        hint(uint16_t property) override;
        void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
        void        download_texture(PImage* img) override;
        std::string name() override { return "PGraphicsOpenGLv33"; }
        void        debug_text(const std::string& text, float x, float y) override;
        void        emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, bool line_strip_closed) override;
        void        emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) override;

    private:
        struct RenderBatch {
            int    start_index;
            int    num_vertices;
            GLuint texture_id;

            RenderBatch(const int start, const int count, const GLuint texID)
                : start_index(start), num_vertices(count), texture_id(texID) {}
        };

        struct PrimitiveVertexBuffer {
            GLuint              VAO{0};
            GLuint              VBO{0};
            std::vector<Vertex> vertices{};
            const uint32_t      num_vertices;
            explicit PrimitiveVertexBuffer(const uint32_t vertex_count) : num_vertices(vertex_count) {
                vertices.resize(vertex_count);
            }
            bool uninitialized() const {
                return VAO == 0 || VBO == 0;
            }
        };

        static constexpr bool     RENDER_POINT_AS_CIRCLE                 = true;
        static constexpr bool     RENDER_PRIMITVES_AS_SHAPES             = true;
        static constexpr uint8_t  NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV = 9;
        static constexpr uint8_t  NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA  = 7;
        static constexpr uint32_t VBO_BUFFER_CHUNK_SIZE                  = 1024 * 1024; // 1MB
        static constexpr uint8_t  RENDER_MODE_IMMEDIATE                  = 0;
        static constexpr uint8_t  RENDER_MODE_BUFFERED                   = 1;
        static const char*        vertex_shader_source_texture();
        static const char*        fragment_shader_source_texture();
        static const char*        vertex_shader_source_simple();
        static const char*        fragment_shader_source_simple();
        uint8_t                   render_mode{RENDER_MODE_IMMEDIATE};
        GLuint                    fill_shader_program{};
        GLuint                    fill_VAO_xyz_rgba_uv{};
        GLuint                    fill_VBO_xyz_rgba_uv{};
        std::vector<float>        fill_vertices_xyz_rgba_uv;
        uint32_t                  fill_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)
        GLuint                    stroke_shader_program{};
        GLuint                    stroke_VAO_xyz_rgba{};
        GLuint                    stroke_VBO_xyz_rgba{};
        std::vector<float>        stroke_vertices_xyz_rgba;
        uint32_t                  stroke_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)
        GLuint                    texture_id_solid_color{};
        GLuint                    texture_id_current{};
        std::vector<RenderBatch>  renderBatches;
        bool                      render_lines_as_quads{true};
        UFont                     debug_font;

        /* --- RENDER_MODE_IMMEDIATE (IM) --- */

        // TODO what about textures?!?
        PrimitiveVertexBuffer IM_primitive_line{2};
        PrimitiveVertexBuffer IM_primitive_rect_stroke{5};
        PrimitiveVertexBuffer IM_primitive_rect_fill{4};
        PrimitiveVertexBuffer IM_primitive_shape{VBO_BUFFER_CHUNK_SIZE};

        void IM_render_point(float x1, float y1, float z1);
        void IM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void IM_render_end_shape(bool close_shape);
        void OGL_tranform_model_matrix_and_render_vertex_buffer(PrimitiveVertexBuffer& primitive,
                                                                GLenum                 mode,
                                                                std::vector<Vertex>&   shape_vertices) const;

        /* --- RENDER_MODE_RETAINED (RM) --- */

        void RM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void RM_render_ellipse(float x, float y, float width, float height);

        void RM_flush_stroke();
        void RM_flush_fill();
        void fill_resize_buffer(uint32_t newSize);
        void init_stroke_vertice_buffers();
        void init_fill_vertice_buffers();

        void RM_add_quad_as_triangles(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 v3, const glm::vec4 color) {
            // triangle #1
            add_transformed_fill_vertex_xyz_rgba_uv(v0, color);
            add_transformed_fill_vertex_xyz_rgba_uv(v1, color);
            add_transformed_fill_vertex_xyz_rgba_uv(v3, color);
            // triangle #2
            add_transformed_fill_vertex_xyz_rgba_uv(v3, color);
            add_transformed_fill_vertex_xyz_rgba_uv(v1, color);
            add_transformed_fill_vertex_xyz_rgba_uv(v2, color);
        }

        void add_transformed_fill_vertex_xyz_rgba_uv(const glm::vec3& position, const glm::vec4& color, float u = 0.0f, float v = 0.0f);
        void RM_add_texture_id_to_render_batch(const std::vector<float>& vertices, int num_vertices, GLuint batch_texture_id);
        void RM_render_line_strip_as_connected_quads(std::vector<glm::vec3>& points, const glm::vec4& color, bool close_shape);
        void RM_render_line_strip_as_quad_segments(const std::vector<glm::vec3>& points, const glm::vec4& color, bool close_shape, bool round_corners);
        void RM_render_ellipse_filled(float x, float y, float width, float height, int detail, const glm::vec4& color);

        // TODO remove these:
        void add_fill_vertex_xyz_rgba_uv(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords);
        void add_fill_vertex_xyz_rgba_uv_raw(glm::vec3 position, glm::vec4 color, glm::vec2 tex_coords);
        void add_fill_vertex_xyz_rgba_uv(float x, float y, float z,
                                         float r, float g, float b, float a = 1.0f,
                                         float u = 0.0f, float v = 0.0f);
        void add_stroke_vertex_xyz_rgba(float x, float y, float z,
                                        float r, float g, float b, float a = 1.0f);
        /* --- SHARED --- */

        static void   OGL_render_vertex_buffer(PrimitiveVertexBuffer& vertex_buffer, GLenum primitive_mode, const std::vector<Vertex>& shape_vertices);
        static void   OGL3_resize_vertex_buffer(size_t buffer_size_bytes);
        static void   OGL3_init_vertex_buffer(PrimitiveVertexBuffer& primitive);

        // void          PGRAPHICS_triangulate_line_strip_vertex(const std::vector<Vertex>& line_strip, bool close_shape, std::vector<Vertex>& line_vertices) const;
        void          create_solid_color_texture();
        bool          SHARED_generate_and_upload_image_as_texture(PImage* image, bool generate_texture_mipmapped); // TODO replace `init()` in PImage constructor with `upload_texture(...)`
        static GLuint SHARED_build_shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        static void   SHARED_checkShaderCompileStatus(GLuint shader);
        static void   SHARED_checkProgramLinkStatus(GLuint program);
    };
} // namespace umgebung
