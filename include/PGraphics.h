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

// TODO implement primitives
//     arc() Draws an arc in the display window
//     square() Draws a square to the screen
//     sphereDetail() Controls the detail used to render a sphere by adjusting the number of vertices of the sphere mesh

#include <sstream>
#include <glm/glm.hpp>

#include "UmgebungConstants.h"
#include "PImage.h"
#include "Vertex.h"
#include "Triangulator.h"

namespace umgebung {
    class PFont;

    class PGraphics : public virtual PImage {
    public:
        struct FrameBufferObject {
            unsigned int id{};
            unsigned int texture_id{};
            int          width{};
            int          height{};
        };

        FrameBufferObject framebuffer{};
        bool              render_to_offscreen{true};
        float             depth_range = 10000.0f;

        PGraphics();
        ~PGraphics() override = default;

        /* --- implementation specific methods --- */

        virtual void IMPL_background(float a, float b, float c, float d) {} // NOTE this needs to clear the color buffer and depth buffer
        virtual void IMPL_bind_texture(int bind_texture_id) {}              // TODO on closer insepction this is only used in PGraphicsOpenGL
        virtual void IMPL_set_texture(PImage* img) {}

        virtual void render_framebuffer_to_screen(bool use_blit = false) {} // TODO this should probably go to PGraphicsOpenGL

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
        virtual void beginDraw()                                                                                    = 0;
        virtual void endDraw()                                                                                      = 0;

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

        virtual void    bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        virtual void    bezier(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4);
        virtual void    bezierDetail(int detail);
        virtual void    ellipse(float x, float y, float width, float height);
        virtual void    ellipseMode(int mode);
        virtual void    ellipseDetail(int detail);
        virtual void    circle(float x, float y, float diameter);
        virtual void    image(PImage* img, float x, float y, float w, float h);
        virtual void    image(PImage* img, float x, float y);
        virtual void    texture(PImage* img = nullptr);
        virtual PImage* loadImage(const std::string& filename);
        virtual void    line(float x1, float y1, float z1, float x2, float y2, float z2);
        virtual void    line(float x1, float y1, float x2, float y2);
        virtual void    point(float x, float y, float z = 0.0f);
        virtual void    pointSize(float size);
        virtual void    quad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4);
        virtual void    rect(float x, float y, float width, float height);
        virtual void    rectMode(int mode);
        virtual void    square(const float x, const float y, const float extent) { rect(x, y, extent, extent); }
        virtual void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
        virtual void    textFont(PFont* font);
        virtual void    textSize(float size);
        virtual void    text(const char* value, float x, float y, float z = 0.0f);
        virtual float   textWidth(const std::string& text);
        virtual PFont*  loadFont(const std::string& file, float size);
        virtual void    box(float width, float height, float depth);
        virtual void    box(const float size) { box(size, size, size); }
        virtual void    sphere(float width, float height, float depth);
        virtual void    sphere(const float size) { sphere(size, size, size); }
        virtual void    vertex(float x, float y, float z, float u, float v);
        virtual void    vertex(float x, float y, float z = 0.0f);
        virtual void    beginShape(int shape = POLYGON);
        void            process_collected_fill_vertices();
        void            process_collected_stroke_vertices(bool close_shape);
        virtual void    endShape(bool close_shape = false);
        // virtual void    lights()                                                                                           = 0;

        /* --- additional --- */

        virtual std::string name() { return "PGraphics"; }
        virtual void        reset_matrices();
        virtual void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) {}
        virtual void        download_texture(PImage* img) {}
        virtual void        lock_init_properties(const bool lock_properties) { init_properties_locked = lock_properties; }
        virtual void        hint(uint16_t property);
        virtual void        pixelDensity(int density);
        virtual void        text_str(const std::string& text, float x, float y, float z = 0.0f); // TODO maybe make this private?
        virtual void        debug_text(const std::string& text, float x, float y) {}             // TODO implement this in PGraphics
        void                to_screen_space(glm::vec3& world_position) const;                    // NOTE: convert from model space to screen space
        void                to_world_space(glm::vec3& model_position) const;                     // NOTE: convert from model space to works space
        void                linse(const float x1, const float y1, const float x2, const float y2) { line(x1, y1, x2, y2); }
        int                 getPixelDensity() const { return pixel_density; }
        void                stroke_mode(const int line_render_mode) { this->line_render_mode = line_render_mode; }
        void                stroke_properties(float stroke_join_round_resolution, float stroke_cap_round_resolution, float stroke_join_miter_max_angle);
        void                triangulate_line_strip_vertex(const std::vector<Vertex>& line_strip, bool close_shape, std::vector<Vertex>& line_vertices) const;

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

        const float                      DEFAULT_FOV            = 2.0f * atan(0.5f); // = 53.1301f;
        static constexpr uint16_t        ELLIPSE_DETAIL_MIN     = 3;
        static constexpr uint16_t        ELLIPSE_DETAIL_DEFAULT = 36;
        bool                             init_properties_locked{false};
        PFont*                           current_font{nullptr};
        ColorState                       color_stroke{};
        ColorState                       color_fill{};
        uint8_t                          rect_mode{CORNER};
        uint8_t                          ellipse_mode{CENTER};
        int                              ellipse_detail{0};
        std::vector<glm::vec2>           ellipse_points_LUT{};
        float                            point_size{1};
        float                            stroke_weight{1};
        int                              bezier_detail{20};
        uint8_t                          pixel_density{1};
        int                              texture_id_current{};
        bool                             shape_has_begun{false};
        int                              polygon_triangulation_strategy{POLYGON_TRIANGULATION_BETTER};
        int                              line_render_mode{STROKE_RENDER_MODE_TRIANGULATE};
        int                              point_render_mode{POINT_RENDER_MODE_TRIANGULATE};
        int                              stroke_join_mode{BEVEL_FAST};
        int                              stroke_cap_mode{PROJECT};
        float                            stroke_join_round_resolution{glm::radians(20.0f)}; // TODO maybe make these configurable
        float                            stroke_cap_round_resolution{glm::radians(20.0f)};  // 20° resolution i.e 18 segment for whole circle
        float                            stroke_join_miter_max_angle{165.0f};
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
        glm::mat4                        model_matrix{};
        glm::mat4                        view_matrix{};
        glm::mat4                        projection_matrix_2D{};
        glm::mat4                        projection_matrix_3D{};
        std::vector<glm::mat4>           model_matrix_stack{};
        bool                             model_matrix_dirty{false};

        // TODO clean this up:

        void push_texture_id() {
            last_bound_texture_id_cache = texture_id_current;
        }

        void pop_texture_id() {
            IMPL_bind_texture(last_bound_texture_id_cache);
            last_bound_texture_id_cache = TEXTURE_NONE;
        }

        void vertex_vec(const glm::vec3& position, const glm::vec2& tex_coords) {
            vertex(position.x, position.y, position.z, tex_coords.x, tex_coords.y);
        }

        static glm::vec4 as_vec4(const ColorState& color) {
            return {color.r, color.g, color.b, color.a};
        }

        void push_color_state(const ColorState& current, std::vector<ColorState>& stack) const {
            stack.push_back(current);
        }

        void pop_color_state(ColorState& current, std::vector<ColorState>& stack) const {
            if (!stack.empty()) {
                current = stack.back();
                stack.pop_back();
            }
        }

        void resize_ellipse_points_LUT();
    };
} // namespace umgebung