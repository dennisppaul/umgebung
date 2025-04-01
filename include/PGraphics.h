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

#include <sstream>
#include <glm/glm.hpp>

#include "UmgebungConstants.h"
#include "UmgebungFunctionsAdditional.h"
#include "PImage.h"
#include "Vertex.h"
#include "Triangulator.h"
#include "UFont.h"

namespace umgebung {
    class PFont;
    class VertexBuffer;
    class PShader;

    class PGraphics : public virtual PImage {
    public:
        struct FrameBufferObject {
            unsigned int id{};
            unsigned int texture_id{};
            int          width{};
            int          height{};
            bool         msaa{false};
        };

        FrameBufferObject framebuffer{};
        bool              render_to_offscreen{true};
        float             depth_range = 10000.0f;

        PGraphics();
        ~PGraphics() override = default;

        /* --- implementation specific methods --- */

        virtual void IMPL_background(float a, float b, float c, float d) = 0; // NOTE this needs to clear the color buffer and depth buffer
        virtual void IMPL_bind_texture(int bind_texture_id)              = 0;
        virtual void IMPL_set_texture(PImage* img)                       = 0;

        virtual void render_framebuffer_to_screen(bool use_blit) {} // TODO this should probably go to PGraphicsOpenGL
        virtual bool read_framebuffer(std::vector<unsigned char>& pixels) { return false; }

        /* --- implementation specific methods ( pure virtual ) --- */

        /**
         * @brief method should emit the fill vertices to the rendering backend. recording, collecting,
         *        and transforming vertices needs to happen here. any drawing should use this method.
         * @param triangle_vertices
         */
        virtual void emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) = 0;
        /**
         * @brief method should emit the stroke vertices to the rendering backend. recording, collecting,
         *        and transforming vertices needs to happen here. any drawing should use this method.
         * @param line_strip_vertices
         * @param line_strip_closed
         */
        virtual void emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, bool line_strip_closed) = 0;
        virtual void beginDraw();
        virtual void endDraw();
        virtual void reset_mvp_matrices();
        virtual void restore_mvp_matrices();

        /* --- implemented in base class PGraphics --- */

        virtual void popMatrix();
        virtual void pushMatrix();
        virtual void resetMatrix();
        virtual void printMatrix(const glm::mat4& matrix);
        virtual void printMatrix();
        virtual void translate(float x, float y, float z = 0.0f);
        virtual void rotateX(float angle);
        virtual void rotateY(float angle);
        virtual void rotateZ(float angle);
        virtual void rotate(float angle);
        virtual void rotate(float angle, float x, float y, float z);
        virtual void scale(float x);
        virtual void scale(float x, float y);
        virtual void scale(float x, float y, float z);

        virtual void background(PImage* img);
        virtual void background(float a, float b, float c, float d = 1.0f);
        virtual void background(float a);
        virtual void fill(float r, float g, float b, float alpha = 1.0f);
        virtual void fill(float gray, float alpha = 1.0f);
        virtual void fill_color(uint32_t c);
        virtual void noFill();
        virtual void stroke(float r, float g, float b, float alpha = 1.0f);
        virtual void stroke(float gray, float alpha);
        virtual void stroke(float a);
        virtual void stroke_color(uint32_t c);
        virtual void noStroke();
        virtual void strokeWeight(float weight);
        virtual void strokeJoin(int join);
        virtual void strokeCap(int cap);

        // ## Shape

        // ### 2d Primitives

        virtual void arc(float x, float y, float w, float h, float start, float stop, int mode = OPEN);
        virtual void circle(float x, float y, float diameter);
        virtual void ellipse(float a, float b, float c, float d);
        virtual void line(float x1, float y1, float z1, float x2, float y2, float z2);
        virtual void line(float x1, float y1, float x2, float y2);
        virtual void point(float x, float y, float z = 0.0f);
        virtual void quad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4);
        virtual void rect(float x, float y, float width, float height);
        virtual void square(const float x, const float y, const float extent) { rect(x, y, extent, extent); }
        virtual void triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

        // ### Vertex

        virtual void beginShape(int shape = POLYGON);
        virtual void endShape(bool close_shape = false);
        virtual void vertex(float x, float y, float z = 0.0f);
        virtual void vertex(float x, float y, float z, float u, float v);

        // ## Structure

        virtual void pushStyle();
        virtual void popStyle();


        virtual void     bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        virtual void     bezier(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4);
        virtual void     bezierDetail(int detail);
        virtual void     curve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        virtual void     curve(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4);
        virtual void     curveDetail(int detail);
        virtual void     curveTightness(float tightness);
        virtual void     arcDetail(int detail);
        virtual void     ellipseMode(int mode);
        virtual void     ellipseDetail(int detail);
        virtual void     image(PImage* img, float x, float y, float w, float h);
        virtual void     image(PImage* img, float x, float y);
        virtual void     texture(PImage* img = nullptr);
        virtual PImage*  loadImage(const std::string& filename);
        virtual void     pointSize(float size);
        virtual void     rectMode(int mode);
        virtual void     textFont(PFont* font);
        virtual void     textSize(float size);
        virtual void     text(const char* value, float x, float y, float z = 0.0f);
        virtual float    textWidth(const std::string& text);
        virtual void     textAlign(int alignX, int alignY = BASELINE);
        virtual float    textAscent();
        virtual float    textDescent();
        virtual void     textLeading(float leading);
        virtual PFont*   loadFont(const std::string& file, float size);
        virtual void     box(float width, float height, float depth);
        virtual void     box(const float size) { box(size, size, size); }
        virtual void     sphere(float width, float height, float depth);
        virtual void     sphere(const float size) { sphere(size, size, size); }
        void             process_collected_fill_vertices();
        void             process_collected_stroke_vertices(bool close_shape);
        virtual void     shader(PShader* shader) {} // TODO maybe not implement them like this
        virtual PShader* loadShader(const std::string& vertex_code, const std::string& fragment_code, const std::string& geometry_code = "") { return nullptr; };
        virtual void     resetShader() {}
        virtual void     normal(float x, float y, float z, float w = 0);
        virtual void     blendMode(int mode) {}
        // virtual void     beginCamera();
        // virtual void     endCamera();
        virtual void camera();
        virtual void camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
        virtual void frustum(float left, float right, float bottom, float top, float near, float far);
        virtual void ortho(float left, float right, float bottom, float top, float near, float far);
        virtual void perspective(float fovy, float aspect, float near, float far);
        virtual void printCamera();
        virtual void printProjection();
        // virtual void    lights()                                                                                           = 0;

        /* --- additional --- */

        virtual void        mesh(VertexBuffer* mesh_shape) {}
        virtual void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) {}
        virtual void        download_texture(PImage* img) {}
        virtual void        lock_init_properties(const bool lock_properties) { init_properties_locked = lock_properties; }
        virtual void        hint(uint16_t property);
        virtual void        pixelDensity(int density);
        virtual void        text_str(const std::string& text, float x, float y, float z = 0.0f); // TODO maybe make this private?
        virtual void        debug_text(const std::string& text, float x, float y) {}             // TODO implement this in PGraphics
        void                to_screen_space(glm::vec3& model_position) const;                    // NOTE: convert from model space to screen space
        void                to_world_space(glm::vec3& model_position) const;                     // NOTE: convert from model space to works space
        void                linse(const float x1, const float y1, const float x2, const float y2) { line(x1, y1, x2, y2); }
        int                 getPixelDensity() const { return pixel_density; }
        void                stroke_mode(const int line_render_mode) { this->line_render_mode = line_render_mode; }
        void                stroke_properties(float stroke_join_round_resolution, float stroke_cap_round_resolution, float stroke_join_miter_max_angle);
        void                triangulate_line_strip_vertex(const std::vector<Vertex>& line_strip, bool close_shape, std::vector<Vertex>& line_vertices) const;
        virtual void        set_default_graphics_state() {}
        void                set_render_mode(const int render_mode) { this->render_mode = render_mode; }
        virtual std::string name() { return "PGraphics"; }

        template<typename T>
        void text(const T& value, const float x, const float y, const float z = 0.0f) {
            std::ostringstream ss;
            ss << value;
            text_str(ss.str(), x, y, z);
        }

        static std::vector<Vertex> triangulate_faster(const std::vector<Vertex>& vertices);
        static std::vector<Vertex> triangulate_better_quality(const std::vector<Vertex>& vertices);
        static std::vector<Vertex> triangulate_good(const std::vector<Vertex>& vertices);

    protected:
        struct ColorState : glm::vec4 {
            bool active = false;
        };

        struct StyleState {
            ColorState stroke;
            ColorState fill;
            float      strokeWeight;
            // TODO add style values like tint, blend mode, etc.
        };

        // const float                      DEFAULT_FOV            = 2.0f * atan(0.5f); // = 53.1301f; // P5 :: tan(PI*30.0 / 180.0);
        static constexpr uint16_t        ELLIPSE_DETAIL_MIN     = 3;
        static constexpr uint16_t        ELLIPSE_DETAIL_DEFAULT = 36;
        static constexpr uint16_t        ARC_DETAIL_DEFAULT     = 36;
        std::stack<StyleState>           style_stack;
        bool                             init_properties_locked{false};
        PFont*                           current_font{nullptr};
        ColorState                       color_stroke{};
        ColorState                       color_fill{};
        int                              rect_mode{CORNER};
        int                              ellipse_mode{CENTER};
        int                              ellipse_detail{0};
        int                              arc_detail{ARC_DETAIL_DEFAULT};
        std::vector<glm::vec2>           ellipse_points_LUT{};
        float                            point_size{1};
        float                            stroke_weight{1};
        int                              bezier_detail{20};
        int                              curve_detail{20};
        float                            curve_tightness{0.0f};
        uint8_t                          pixel_density{1};
        int                              texture_id_current{TEXTURE_NONE};
        bool                             shape_has_begun{false};
        int                              polygon_triangulation_strategy{POLYGON_TRIANGULATION_BETTER};
        int                              line_render_mode{STROKE_RENDER_MODE_TRIANGULATE_2D};
        int                              point_render_mode{POINT_RENDER_MODE_TRIANGULATE};
        int                              stroke_join_mode{BEVEL_FAST};
        int                              stroke_cap_mode{PROJECT};
        float                            stroke_join_round_resolution{glm::radians(20.0f)}; // TODO maybe make these configurable
        float                            stroke_cap_round_resolution{glm::radians(20.0f)};  // 20° resolution i.e 18 segment for whole circle
        float                            stroke_join_miter_max_angle{163.0f};
        inline static const Triangulator triangulator{};
        std::vector<ColorState>          color_stroke_stack{};
        std::vector<ColorState>          color_fill_stack{};
        std::vector<glm::vec3>           box_vertices_LUT{};
        std::vector<glm::vec3>           sphere_vertices_LUT{};
        int                              shape_mode_cache{POLYGON};
        static constexpr uint32_t        VBO_BUFFER_CHUNK_SIZE{1024 * 1024}; // 1MB
        std::vector<Vertex>              shape_stroke_vertex_buffer{VBO_BUFFER_CHUNK_SIZE};
        std::vector<Vertex>              shape_fill_vertex_buffer{VBO_BUFFER_CHUNK_SIZE};
        int                              last_bound_texture_id_cache{TEXTURE_NONE};
        bool                             model_matrix_dirty{false};
        glm::vec4                        current_normal{Vertex::DEFAULT_NORMAL};
        glm::mat4                        temp_view_matrix{};
        glm::mat4                        temp_projection_matrix{};
        int                              render_mode{RENDER_MODE_IMMEDIATE};
        UFont                            debug_font;
        bool                             in_camera_block{false};
        PShader*                         current_shader{nullptr};

    public:
        PShader*               default_shader{nullptr};
        glm::mat4              model_matrix{};
        glm::mat4              view_matrix{};
        glm::mat4              projection_matrix{};
        std::vector<glm::mat4> model_matrix_stack{};

    protected:
        bool texture_id_pushed{false};
        void push_texture_id() {
            if (!texture_id_pushed) {
                texture_id_pushed           = true;
                last_bound_texture_id_cache = texture_id_current;
            } else {
                warning("unbalanced texture id *push*/pop");
            }
        }

        void pop_texture_id() {
            if (texture_id_pushed) {
                texture_id_pushed = false;
                IMPL_bind_texture(last_bound_texture_id_cache);
                last_bound_texture_id_cache = TEXTURE_NONE;
            } else {
                warning("unbalanced texture id push/*pop*");
            }
        }

        void vertex_vec(const glm::vec3& position, const glm::vec2& tex_coords) {
            vertex(position.x, position.y, position.z, tex_coords.x, tex_coords.y);
        }

        static glm::vec4 as_vec4(const ColorState& color) {
            return {color.r, color.g, color.b, color.a};
        }

        static void push_color_state(const ColorState& current, std::vector<ColorState>& stack) {
            stack.push_back(current);
        }

        static void pop_color_state(ColorState& current, std::vector<ColorState>& stack) {
            if (!stack.empty()) {
                current = stack.back();
                stack.pop_back();
            }
        }

        void resize_ellipse_points_LUT();
    };
} // namespace umgebung