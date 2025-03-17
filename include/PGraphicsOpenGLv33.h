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
        void rect(float x, float y, float width, float height) override;

        // TODO move to implementation file i.e `.cpp`

        void setup_matrices() override {
            reset_matrices();
        }

        void restore_matrices() override {
            // No explicit matrix stack in modern OpenGL
        }

        void reset_matrices() override {
            PGraphics::reset_matrices();

            const float viewport_width  = framebuffer_width;
            const float viewport_height = framebuffer_height;
            glViewport(0, 0, static_cast<GLint>(viewport_width), static_cast<GLint>(viewport_height));
        }

        void prepare_frame() {
            set_default_graphics_state();

            if (render_mode == RENDER_MODE_IMMEDIATE) {
                glUseProgram(fill_shader_program);

                // Upload matrices
                const GLint projLoc = glGetUniformLocation(fill_shader_program, "uProjection");
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D));

                const GLint viewLoc = glGetUniformLocation(fill_shader_program, "uViewMatrix");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

                const GLint modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

                texture_id_current = 0;
                IMPL_bind_texture(texture_id_solid_color);
            }
        }

        void beginDraw() override {
            if (render_to_offscreen) {
                store_current_fbo();
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
            }
            prepare_frame();
            reset_matrices();
        }

        void endDraw() override {
            if (render_mode == RENDER_MODE_BUFFERED) {
                RM_flush_fill();
                RM_flush_stroke();
            }

            if (render_to_offscreen) {
                restore_previous_fbo();
            }
        }

        // Modern OpenGL framebuffer rendering method
        void render_framebuffer_to_screen(bool use_blit = false) override {
            if (use_blit) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.id);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                glBlitFramebuffer(0, 0, framebuffer.width, framebuffer.height,
                                  0, 0, framebuffer.width, framebuffer.height,
                                  GL_COLOR_BUFFER_BIT, GL_LINEAR); // TODO maybe GL_NEAREST is enough
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            } else {
                error("`render_framebuffer_to_screen` need to implement this ... maybe reuse existing shader");
                // glBindFramebuffer(GL_FRAMEBUFFER, 0);
                // glDisable(GL_DEPTH_TEST);
                // glDisable(GL_BLEND);
                //
                // glUseProgram(shaderProgram);
                // glBindVertexArray(screenVAO);
                //
                // bind_framebuffer_texture();
                // glUniform1i(glGetUniformLocation(shaderProgram, "screenTexture"), 0);
                //
                // glDrawArrays(GL_TRIANGLES, 0, 6);
                //
                // glBindVertexArray(0);
                // glUseProgram(0);
            }
        }

    public:
        explicit PGraphicsOpenGLv33(bool render_to_offscreen);

        void   init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override;
        void   strokeWeight(float weight) override;
        void   background(float a, float b, float c, float d = 1.0f) override;
        void   background(float a) override;
        void   triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) override; // NOTE: done
        void   point(float x, float y, float z = 0.0f) override;                                                            // NOTE: done
        void   pointSize(float size) override;
        void   beginShape(int shape = POLYGON) override;                     // NOTE: done
        void   endShape(bool close_shape = false) override;                  // NOTE: done
        void   vertex(float x, float y, float z = 0.0f) override;            // NOTE: done
        void   vertex(float x, float y, float z, float u, float v) override; // NOTE: done
        PFont* loadFont(const std::string& file, float size) override;
        void   textFont(PFont* font) override;
        void   textSize(float size) override;
        void   text(const char* value, float x, float y, float z = 0.0f) override;
        float  textWidth(const std::string& text) override;
        void   image(PImage* img, float x, float y, float w, float h) override; // NOTE: done
        void   image(PImage* img, float x, float y) override;                   // NOTE: done
        void   texture(PImage* img) override;                                   // NOTE: done
        void   pixelDensity(int density) override;
        void   hint(uint16_t property) override;
        void   text_str(const std::string& text, float x, float y, float z = 0.0f) override;

        /* --- additional methods --- */

        void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
        void        download_texture(PImage* img) override;
        std::string name() override { return "PGraphicsOpenGLv33"; }
        void        debug_text(const std::string& text, float x, float y) override;
        UFont       debug_font;

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
        uint8_t                   render_mode                            = RENDER_MODE_IMMEDIATE;
        static const char*        vertex_shader_source_texture();
        static const char*        fragment_shader_source_texture();
        static const char*        vertex_shader_source_simple();
        static const char*        fragment_shader_source_simple();
        // TODO @RENDER_MODE_RETAINED replace all `add_vertex...` methods with just
        //     fill ( rendered as `GL_TRIANGLES` ):
        //     - `RM_fill_add_vertex(Vertex v)`
        //     - `RM_fill_add_triangle(Vertex v1, Vertex v2, Vertex v3)`
        //     stroke ( rendered as `GL_LINES` ):
        //     - `RM_fill_add_vertex(Vertex v)`
        //     - `RM_fill_add_line(Vertex v1, Vertex v2)`
        uint8_t render_line_mode = RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_MITER;
        // TODO create *vertex buffer client* struct for this
        // TODO remove these and replace them with `PrimitiveVertexArray`:
        GLuint                   fill_shader_program{};
        GLuint                   fill_VAO_xyz_rgba_uv{};
        GLuint                   fill_VBO_xyz_rgba_uv{};
        std::vector<float>       fill_vertices_xyz_rgba_uv;
        uint32_t                 fill_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)
        GLuint                   stroke_shader_program{};
        GLuint                   stroke_VAO_xyz_rgba{};
        GLuint                   stroke_VBO_xyz_rgba{};
        std::vector<float>       stroke_vertices_xyz_rgba;
        uint32_t                 stroke_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)
        GLuint                   texture_id_solid_color{};
        GLuint                   texture_id_current{};
        std::vector<RenderBatch> renderBatches;
        bool                     render_lines_as_quads{true};
        std::vector<glm::vec3>   shape_stroke_vertex_cache_vec3_DEPRECATED{VBO_BUFFER_CHUNK_SIZE}; // TODO remove this
        std::vector<Vertex>      shape_stroke_vertex_buffer{VBO_BUFFER_CHUNK_SIZE};
        std::vector<Vertex>      shape_fill_vertex_buffer{VBO_BUFFER_CHUNK_SIZE};
        int                      shape_mode_cache{POLYGON};

        // TODO check if all vertex attributes are copied i.e color and tex_coords
        // TODO move to Geometry class
        std::vector<Vertex>        convertQuadStripToQuads(const std::vector<Vertex>& quadStrip) const;
        std::vector<Vertex>        convertPointsToTriangles(const std::vector<Vertex>& points, float size) const;
        static std::vector<Vertex> convertQuadsToTriangles(const std::vector<Vertex>& quads);
        static std::vector<Vertex> convertPolygonToTriangleFan(const std::vector<Vertex>& polygon);
        std::vector<Vertex>        convertTriangleFanToTriangles(const std::vector<Vertex>& fan) const;
        std::vector<Vertex>        convertTriangleStripToTriangles(const std::vector<Vertex>& strip) const;

        void emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, bool line_strip_closed);
        void emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices);

        /* --- RENDER_MODE_IMMEDIATE (IM) --- */

        // TODO what about textures?!?
        PrimitiveVertexBuffer IM_primitive_line{2};
        PrimitiveVertexBuffer IM_primitive_rect_stroke{5};
        PrimitiveVertexBuffer IM_primitive_rect_fill{4};
        PrimitiveVertexBuffer IM_primitive_shape{VBO_BUFFER_CHUNK_SIZE};

        void IM_render_point(float x1, float y1, float z1);
        void IM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void IM_render_rect(float x, float y, float width, float height);
        void IM_render_end_shape(bool close_shape);
        void OGL_tranform_model_matrix_and_render_vertex_buffer(PrimitiveVertexBuffer& primitive,
                                                                GLenum                 mode,
                                                                std::vector<Vertex>&   shape_vertices) const;

        /* --- RENDER_MODE_RETAINED (RM) --- */

        void RM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void RM_render_rect(float x, float y, float width, float height);
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

        void          PGRAPHICS_triangulate_line_strip_vertex(const std::vector<Vertex>& line_strip, bool close_shape, std::vector<Vertex>& line_vertices) const;
        void          create_solid_color_texture();
        void          IMPL_bind_texture(int bind_texture_id) override;
        bool          SHARED_generate_and_upload_image_as_texture(PImage* image, bool generate_texture_mipmapped); // TODO replace `init()` in PImage constructor with `upload_texture(...)`
        static void   OGL3_resize_vertex_buffer(size_t buffer_size_bytes);
        static void   OGL_render_vertex_buffer(PrimitiveVertexBuffer& vertex_buffer, GLenum primitive_mode, const std::vector<Vertex>& shape_vertices);
        static void   OGL3_init_vertex_buffer(PrimitiveVertexBuffer& primitive);
        static GLuint SHARED_build_shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        static void   SHARED_checkShaderCompileStatus(GLuint shader);
        static void   SHARED_checkProgramLinkStatus(GLuint program);
    };
} // namespace umgebung
