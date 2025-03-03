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
        void    line(float x1, float y1, float x2, float y2) override;
        void    line(float x1, float y1, float z1, float x2, float y2, float z2) override;
        void    linse(float x1, float y1, float x2, float y2);
        void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) override;
        void    bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
        void    bezier(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4) override;
        void    bezierDetail(int detail) override;
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

        void prepare_frame();

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
        struct RenderBatch {
            int    start_index;
            int    num_vertices;
            GLuint texture_id;

            RenderBatch(const int start, const int count, const GLuint texID)
                : start_index(start), num_vertices(count), texture_id(texID) {}
        };

        struct Vertex {
            glm::vec3 position;
            glm::vec4 color;
            glm::vec2 tex_coord;
            Vertex(const float x, const float y, const float z,
                   const float r, const float g, const float b, const float a,
                   const float u, const float v)
                : position(x, y, z),
                  color(r, g, b, a),
                  tex_coord(u, v) {}
            Vertex(const glm::vec3& position,
                   const glm::vec4& color,
                   const glm::vec2& tex_coord)
                : position(position),
                  color(color),
                  tex_coord(tex_coord) {}
            Vertex() : position(), color(), tex_coord() {}
            // Vertex() : position(0, 0, 0), color(0, 0, 0, 0), tex_coord(0, 0) {}
        };

        static constexpr GLint    UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE    = GL_UNSIGNED_INT_8_8_8_8_REV;
        static constexpr GLint    UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT = GL_RGBA;
        static constexpr uint8_t  NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV = 9;
        static constexpr uint8_t  NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA  = 7;
        static constexpr uint32_t VBO_BUFFER_CHUNK_SIZE                  = 1024 * 1024;       // 1MB
        const float               DEFAULT_FOV                            = 2.0f * atan(0.5f); // = 53.1301f;
        static constexpr uint8_t  RENDER_MODE_IMMEDIATE                  = 0;
        static constexpr uint8_t  RENDER_MODE_RETAINED                   = 1;
        uint8_t                   render_mode                            = RENDER_MODE_IMMEDIATE;
        // TODO @RENDER_MODE_RETAINED replace all `add_vertex...` methods with just
        //     fill ( rendered as `GL_TRIANGLES` ):
        //     - `RM_fill_add_vertex(Vertex v)`
        //     - `RM_fill_add_triangle(Vertex v1, Vertex v2, Vertex v3)`
        //     stroke ( rendered as `GL_LINES` ):
        //     - `RM_fill_add_vertex(Vertex v)`
        //     - `RM_fill_add_line(Vertex v1, Vertex v2)`

        static constexpr uint8_t RENDER_LINE_AS_QUADS_SEGMENTS                    = 0;
        static constexpr uint8_t RENDER_LINE_AS_QUADS_SEGMENTS_WITH_ROUND_CORNERS = 1;
        static constexpr uint8_t RENDER_LINE_AS_QUADS_WITH_POINTY_CORNERS         = 2;
        uint8_t                  render_line_mode                                 = RENDER_LINE_AS_QUADS_SEGMENTS_WITH_ROUND_CORNERS;

        // TODO create *vertex buffer client* struct for this
        // TODO remove these and replace them with `PrimitiveVertexArray`:
        GLuint             fill_shader_program{};
        GLuint             fill_VAO_xyz_rgba_uv{};
        GLuint             fill_VBO_xyz_rgba_uv{};
        std::vector<float> fill_vertices_xyz_rgba_uv;
        uint32_t           fill_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)
        GLuint             stroke_shader_program{};
        GLuint             stroke_VAO_xyz_rgba{};
        GLuint             stroke_VBO_xyz_rgba{};
        std::vector<float> stroke_vertices_xyz_rgba;
        uint32_t           stroke_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)
        GLuint             texture_id_solid_color{};
        GLuint             texture_id_current{};

// #define USE_UNORDERED_MAP
#ifdef USE_UNORDERED_MAP
        std::unordered_map<GLuint, std::vector<Vertex>> render_vertex_batches;
        for (const auto& [texture_id, vertices]: render_vertex_batches) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
        }
        see https : //chatgpt.com/share/67c564b6-c144-8004-a3c3-2252c952d1eb
#else
        std::vector<RenderBatch> renderBatches;
#endif

                    glm::mat4  model_matrix_client;
        glm::mat4              model_matrix_shader;
        std::vector<glm::mat4> model_matrix_stack;
        glm::mat4              projection_matrix_2D{};
        glm::mat4              projection_matrix_3D{};
        glm::mat4              view_matrix{};
        float                  stroke_weight{1};
        int                    bezier_detail{20};
        int                    previous_FBO{};
        bool                   render_lines_as_quads{true};
        std::vector<glm::vec3> shape_stroke_vertex_cache_vec3_DEPRECATED{VBO_BUFFER_CHUNK_SIZE}; // TODO remove this
        std::vector<Vertex>    shape_stroke_vertex_cache{VBO_BUFFER_CHUNK_SIZE};
        std::vector<Vertex>    shape_fill_vertex_cache{VBO_BUFFER_CHUNK_SIZE};
        int                    shape_mode_cache{POLYGON};
        bool                   shape_has_begun{false};

        // static constexpr int ELLIPSE_NUM_SEGMENTS = 32;
        //        PFont* fCurrentFont           = nullptr;
        //        float  fStrokeWeight          = 1;
        //        bool   fEnabledTextureInShape = false;
        //        bool   fShapeBegun            = false;
        //        int    fPixelDensity          = 1;

        static const char* vertex_shader_source_texture();
        static const char* fragment_shader_source_texture();
        static const char* vertex_shader_source_simple();
        static const char* fragment_shader_source_simple();

        /* --- RENDER_MODE_IMMEDIATE (IM) --- */

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

        // TODO what about textures?!?
        PrimitiveVertexBuffer IM_primitive_line{2};
        PrimitiveVertexBuffer IM_primitive_rect_stroke{5};
        PrimitiveVertexBuffer IM_primitive_rect_fill{4};
        PrimitiveVertexBuffer IM_primitive_shape{VBO_BUFFER_CHUNK_SIZE};

        static std::vector<Vertex> convertQuadsToTriangles(const std::vector<Vertex>& quads);
        static std::vector<Vertex> convertPolygonToTriangleFan(const std::vector<Vertex>& polygon);
        std::vector<Vertex>        triangulateConcavePolygon(const std::vector<Vertex>& polygon) const;

        void IM_render_point(float x1, float y1, float z1);
        void IM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void IM_render_rect(float x, float y, float width, float height);
        void IM_render_ellipse(float x, float y, float width, float height);

        void IM_render_end_shape(bool close_shape);
        void IM_render_vertex_buffer(PrimitiveVertexBuffer& primitive, GLenum mode, std::vector<Vertex>& shape_vertices) const;

        // ... triangle ( + textured ), quad ( + textured ), circle, etcetera

        /* --- RENDER_MODE_RETAINED (RM) --- */

        void RM_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
        void RM_render_rect(float x, float y, float width, float height);
        void RM_render_ellipse(float x, float y, float width, float height);

        void flush_stroke();
        void flush_fill();
        void fill_resize_buffer(uint32_t newSize);
        void init_stroke_vertice_buffers();
        void init_fill_vertice_buffers();
        void create_solid_color_texture();

        void add_transformed_fill_vertex_xyz_rgba_uv(const glm::vec3& position, const glm::vec4& color, float u = 0.0f, float v = 0.0f);
        void RM_add_texture_id_to_render_batch(const std::vector<float>& vertices, int num_vertices, GLuint batch_texture_id);
        void to_screen_space(glm::vec3& world_position) const;
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

        void          SHARED_bind_texture(GLuint bind_texture_id);
        static void          SHARED_resize_vertex_buffer(size_t buffer_size_bytes) ;
        static void          SHARED_render_vertex_buffer(PrimitiveVertexBuffer& vertex_buffer, GLenum primitive_mode, const std::vector<Vertex>& shape_vertices) ;
        static void          SHARED_init_vertex_buffer(PrimitiveVertexBuffer& primitive) ;
        static void   printMatrix(const glm::mat4& matrix);
        static GLuint SHARED_build_shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        static void   SHARED_checkShaderCompileStatus(GLuint shader);
        static void   SHARED_checkProgramLinkStatus(GLuint program);
        static bool   SHARED_upload_image_as_texture(PImage* image, bool generate_texture_mipmapped); // TODO replace `init()` in PImage constructor with `upload_texture(...)`
    };
} // namespace umgebung
