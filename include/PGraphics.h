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

// TODO implement color modes and ranges i.e 0–255 or 0–1 and HSB or RGB ...

/*
// TODO implement 2D Primitives
arc() Draws an arc in the display window
circle() Draws a circle to the screen
ellipse() Draws an ellipse (oval) in the display window
line() Draws a line (a direct path between two points) to the screen  // 3D
point() Draws a point, a coordinate in space at the dimension of one pixel // 3D
quad() A quad is a quadrilateral, a four sided polygon
rect() Draws a rectangle to the screen
square() Draws a square to the screen
triangle() A triangle is a plane created by connecting three points
// TODO implement 3D Primitives
box() A box is an extruded rectangle
sphereDetail() Controls the detail used to render a sphere by adjusting the number of vertices of the sphere mesh
sphere() A sphere is a hollow ball made from tessellated triangles
*/

#ifndef DISABLE_GRAPHICS
#endif // DISABLE_GRAPHICS

#include <sstream>

#include "UmgebungConstants.h"
#include "PImage.h"

#define PGRAPHICS_RENDER_INTO_FRAMEBUFFER

namespace umgebung {

    class PFont;

    class PGraphics : public virtual PImage {
    public:
        static constexpr uint16_t ENABLE_SMOOTH_LINES  = 0;
        static constexpr uint16_t DISABLE_SMOOTH_LINES = 1;

        PGraphics();
        ~PGraphics() override = default;

        /* --- implemented in base class PGraphics --- */

        virtual void fill(float r, float g, float b, float alpha = 1.0f);
        virtual void fill(float gray, float alpha = 1.0f);
        virtual void fill_color(uint32_t c);
        virtual void noFill();
        virtual void stroke(float r, float g, float b, float alpha = 1.0f);
        virtual void stroke(float gray, float alpha);
        virtual void stroke(float a);
        virtual void stroke_color(uint32_t c);
        virtual void noStroke();

        /* --- additional --- */
        virtual void reset_matrices() {} // TODO this should be part of a `beginFrame()` and/or could be handled in `beginDraw()`
        virtual void flush() {}          // TODO this should be renamed to `endFrame()` and could maybe be handle in `endDraw()`

        /* --- interface --- */

        // TODO implement 3D version
        // virtual void    line(float x1, float y1, float z1, float x2, float y2, float z2)    = 0;

        virtual void    strokeWeight(float weight)                                                                         = 0;
        virtual void    background(float a, float b, float c, float d = 1.0f)                                              = 0;
        virtual void    background(float a)                                                                                = 0;
        virtual void    rect(float x, float y, float width, float height)                                                  = 0;
        virtual void    ellipse(float x, float y, float width, float height)                                               = 0;
        virtual void    circle(float x, float y, float radius)                                                             = 0;
        virtual void    ellipseDetail(int detail)                                                                          = 0;
        virtual void    line(float x1, float y1, float x2, float y2)                                                       = 0;
        virtual void    line(float x1, float y1, float z1, float x2, float y2, float z2)                                   = 0;
        virtual void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) = 0;
        virtual void    bezier(float x1, float y1, float x2, float y2,
                               float x3, float y3, float x4, float y4)                                                     = 0;
        virtual void    bezier(float x1, float y1, float z1, float x2, float y2, float z2,
                               float x3, float y3, float z3, float x4, float y4, float z4)                                 = 0;
        virtual void    bezierDetail(int detail)                                                                           = 0;
        virtual void    pointSize(float point_size)                                                                        = 0;
        virtual void    point(float x, float y, float z = 0.0f)                                                            = 0;
        virtual void    beginShape(int shape = POLYGON)                                                                    = 0;
        virtual void    endShape(bool close_shape = false)                                                                 = 0;
        virtual void    vertex(float x, float y, float z = 0.0f)                                                           = 0;
        virtual void    vertex(float x, float y, float z, float u, float v)                                                = 0;
        virtual PFont*  loadFont(const std::string& file, float size)                                                      = 0;
        virtual void    textFont(PFont* font)                                                                              = 0;
        virtual void    textSize(float size)                                                                               = 0;
        virtual void    text(const char* value, float x, float y, float z = 0.0f)                                          = 0;
        virtual float   textWidth(const std::string& text)                                                                 = 0;
        virtual PImage* loadImage(const std::string& filename)                                                             = 0;
        virtual void    image(PImage* img, float x, float y, float w, float h)                                             = 0;
        virtual void    image(PImage* img, float x, float y)                                                               = 0;
        virtual void    texture(PImage* img)                                                                               = 0;
        virtual void    popMatrix()                                                                                        = 0;
        virtual void    pushMatrix()                                                                                       = 0;
        virtual void    translate(float x, float y, float z = 0.0f)                                                        = 0;
        virtual void    rotateX(float angle)                                                                               = 0;
        virtual void    rotateY(float angle)                                                                               = 0;
        virtual void    rotateZ(float angle)                                                                               = 0;
        virtual void    rotate(float angle)                                                                                = 0;
        virtual void    rotate(float angle, float x, float y, float z)                                                     = 0;
        virtual void    scale(float x)                                                                                     = 0;
        virtual void    scale(float x, float y)                                                                            = 0;
        virtual void    scale(float x, float y, float z)                                                                   = 0;
        virtual void    pixelDensity(int density)                                                                          = 0;
        virtual void    hint(uint16_t property)                                                                            = 0;
        virtual void    beginDraw()                                                                                        = 0;
        virtual void    endDraw()                                                                                          = 0;
        virtual void    text_str(const std::string& text, float x, float y, float z = 0.0f)                                = 0; // TODO maybe make this private?

        template<typename T>
        void text(const T& value, const float x, const float y, const float z = 0.0f) {
            std::ostringstream ss;
            ss << value;
            text_str(ss.str(), x, y, z);
        }

        struct FBO {
            unsigned int id{};
            unsigned int texture{};
            int          width{};
            int          height{};
        };

        FBO framebuffer{};

    protected:
        struct Color {
            float r      = 0;
            float g      = 0;
            float b      = 0;
            float a      = 1;
            bool  active = false;
        };
        // TODO maybe better store rgba as glm::vec4
        Color fill_state{};
        Color stroke_state{};
    };
} // namespace umgebung