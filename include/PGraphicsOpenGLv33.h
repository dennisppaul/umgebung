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
#include "UFont.h"

namespace umgebung {

    class PGraphicsOpenGLv33 final : public PGraphicsOpenGL {
    public:
        explicit PGraphicsOpenGLv33(bool render_to_offscreen);

        void IMPL_background(float a, float b, float c, float d) override;
        void IMPL_bind_texture(int bind_texture_id) override;
        void IMPL_set_texture(PImage* img) override;

        /* --- OpenGL 3.3 specific implementation of shared methods --- */

        void render_framebuffer_to_screen(bool use_blit = false) override;
        void emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, bool line_strip_closed) override;
        void emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) override;

        void        restore_matrices() override {}
        void        reset_matrices() override;
        void        prepare_frame() override;
        void        setup_fbo() override;
        void        finish_fbo() override {}
        void        endDraw() override;
        void        init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;
        void        hint(uint16_t property) override;
        void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
        void        download_texture(PImage* img) override;
        void        debug_text(const std::string& text, float x, float y) override; // TODO move to PGraphics ( use glBeginShape() )
        std::string name() override { return "PGraphicsOpenGLv33"; }
        void        mesh(PMesh* mesh_shape) override;
        void        shader(PShader* shader) override;
        PShader*    loadShader(const std::string& vertex_code, const std::string& fragment_code, const std::string& geometry_code = "") override;

    private:
        struct RenderBatch {
            int    start_index;
            int    num_vertices;
            GLuint texture_id;

            RenderBatch(const int start, const int count, const GLuint texID)
                : start_index(start), num_vertices(count), texture_id(texID) {}
        };

        struct VertexBufferData {
            GLuint              VAO{0};
            GLuint              VBO{0};
            std::vector<Vertex> vertices{};
            const uint32_t      num_vertices;
            explicit VertexBufferData(const uint32_t vertex_count) : num_vertices(vertex_count) {
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
        uint8_t                   render_mode{RENDER_MODE_IMMEDIATE};
        GLuint                    texture_id_solid_color{};
        GLuint                    texture_id_current{};
        UFont                     debug_font;
        VertexBufferData          vertex_buffer_data{VBO_BUFFER_CHUNK_SIZE};
        // TODO revive for buffered mode
        std::vector<RenderBatch> renderBatches;

        /* --- OpenGL ( 2.0 + 3.3 ) specific methods --- */

        // TODO move OGL methods to PGraphicsOpenGL
        bool OGL_generate_and_upload_image_as_texture(PImage* image, bool generate_texture_mipmapped); // TODO replace `init()` in PImage constructor with `upload_texture(...)`
        void OGL_tranform_model_matrix_and_render_vertex_buffer(VertexBufferData& vertex_buffer, GLenum mode, std::vector<Vertex>& shape_vertices) const;

        /* --- OpenGL 3.3 specific methods --- */

        static void OGL3_resize_vertex_buffer(size_t buffer_size_bytes);
        static void OGL3_init_vertex_buffer(VertexBufferData& vertex_buffer);
        void        OGL3_create_solid_color_texture();
        static void OGL3_render_vertex_buffer(VertexBufferData& vertex_buffer, GLenum primitive_mode, const std::vector<Vertex>& shape_vertices);
    };
} // namespace umgebung
