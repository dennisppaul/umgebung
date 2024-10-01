/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2023 Dennis P Paul.
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
#define PGRAPHICS_RENDER_INTO_FRAMEBUFFER

#include <sstream>
#include <GL/glew.h>

#endif // DISABLE_GRAPHICS

#include "UmgebungConstants.h"
#include "PImage.h"

// #define PGRAPHICS_USE_VBO

namespace umgebung {

    class PImage;
    class PFont;

    class PGraphics : public virtual PImage {
    public:
        PGraphics();

        void    stroke(float r, float g, float b, float a = 1.0);
        void    stroke(float brightness, float a);
        void    stroke(float a);
        void    stroke(uint32_t c);
        void    noStroke();
        void    strokeWeight(float weight);
        void    fill(float r, float g, float b, float a = 1.0);
        void    fill(float brightness, float a);
        void    fill(float a);
        void    fill(uint32_t c);
        void    noFill();
        void    background(float a, float b, float c, float d = 1.0);
        void    background(float a);
        void    rect(float x, float y, float width, float height) const;
        void    ellipse(float x, float y, float width, float height) const;
        void    circle(float x, float y, float radius) const;
        void    ellipseDetail(int detail);
        void    line(float x1, float y1, float x2, float y2) const;
        void    bezier(float x1, float y1,
                       float x2, float y2,
                       float x3, float y3,
                       float x4, float y4) const;
        void    bezier(float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float x4, float y4, float z4) const;
        void    bezierDetail(int detail);
        void    pointSize(float point_size);
        void    point(float x, float y, float z = 0.0) const;
        void    beginShape(int shape = POLYGON);
        void    endShape();
        void    vertex(float x, float y, float z = 0.0);
        void    vertex(float x, float y, float z, float u, float v);
        PFont*  loadFont(const std::string& file, float size); // @development maybe use smart pointers here
        void    textFont(PFont* font);
        void    textSize(float size) const;
        void    text(const char* value, float x, float y, float z = 0.0f) const;
        float   textWidth(const std::string& text);
        PImage* loadImage(const std::string& filename);
        void    image(const PImage* img, float x, float y, float w, float h) const;
        void    image(PImage* img, float x, float y);
        void    texture(const PImage* img);
        void    popMatrix();
        void    pushMatrix();
        void    translate(float x, float y, float z = 0);
        void    rotateX(float angle);
        void    rotateY(float angle);
        void    rotateZ(float angle);
        void    rotate(float angle);
        void    rotate(float angle, float x, float y, float z);
        void    scale(float x);
        void    scale(float x, float y);
        void    scale(float x, float y, float z);

        template<typename T>
        void text(const T& value, const float x, const float y, const float z = 0.0f) const {
            std::ostringstream ss;
            ss << value;
            text_str(ss.str(), x, y, z);
        }

        ~PGraphics() override;

    private:
        PFont* fCurrentFont           = nullptr;
        float  fPointSize             = 1;
        bool   fEnabledTextureInShape = false;
        bool   fShapeBegun            = false;
        int    fEllipseDetail         = 32;
        int    fBezierDetail          = 20;

        struct {
            float r      = 0;
            float g      = 0;
            float b      = 0;
            float a      = 1;
            bool  active = false;
        } fill_color, stroke_color;

        static constexpr int ELLIPSE_NUM_SEGMENTS = 32;

        void text_str(const std::string& text, float x, float y, float z = 0.0f) const;

#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    private:
        GLuint fbo{}, fbo_texture{};
        int    fbo_width{};
        int    fbo_height{};
        GLint  fPreviousFBO = 0;

    public:
        void beginDraw();
        void endDraw() const;
        void bind() const override;
        void init(uint32_t* pixels, int width, int height, int format) override;
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER

#ifdef PGRAPHICS_USE_VBO
        GLuint ellipseVBO;
        int    ellipseSegments;
        bool   bufferInitialized;
        void   setupEllipseBuffer(int num_segments);
#endif // PGRAPHICS_USE_VBO
    };
} // namespace umgebung