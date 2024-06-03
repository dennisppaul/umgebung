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


#include <string>

#ifndef DISABLE_GRAPHICS

#include <GL/glew.h>

#endif // DISABLE_GRAPHICS

#include "UmgebungConstants.h"

// #define PGRAPHICS_USE_VBO

namespace umgebung {

    class PImage;

    class PFont;

    class PGraphics {
    public:
        int width  = 0;
        int height = 0;

        PGraphics();

        ~PGraphics();

        void stroke(float r, float g, float b, float a = 1.0);

        void stroke(float a);

        void noStroke();

        void fill(float r, float g, float b, float a = 1.0);

        void fill(float brightness, float a);

        void fill(float a);

        void noFill();

        void background(float a, float b, float c, float d = 1.0);

        void background(float a);

        void rect(float x, float y, float _width, float _height);

        void ellipse(float x, float y, float width, float height);

        void circle(float x, float y, float radius);

        void line(float x1, float y1, float x2, float y2);

        void pointSize(float point_size);

        void point(float x, float y, float z = 0.0);

        void beginShape(int shape = POLYGON);

        void endShape();

        void vertex(float x, float y, float z = 0.0);

        PFont* loadFont(std::string file, float size); // @development maybe use smart pointers here

        void textFont(PFont* font);

        void textSize(float size);

        void text(const std::string& text, float x, float y, float z = 0.0);

        template<typename T>
        void text(const T& value, float x, float y, float z);

        PImage* loadImage(const std::string& filename);

        void image(PImage* img, float x, float y, float w, float h);

        void image(PImage* img, float x, float y);

        void popMatrix();

        void pushMatrix();

        void translate(float x, float y, float z = 0);

        void rotateX(float angle);

        void rotateY(float angle);

        void rotateZ(float angle);

        void rotate(float angle);

        void rotate(float angle, float x, float y, float z);

        void scale(float x);

        void scale(float x, float y);

        void scale(float x, float y, float z);

    private:
        PFont* fCurrentFont = nullptr;
        float  fPointSize   = 1;

        struct {
            float r      = 0;
            float g      = 0;
            float b      = 0;
            float a      = 1;
            bool  active = false;
        } fill_color, stroke_color;

        const static int ELLIPSE_NUM_SEGMENTS = 32;
        GLuint           ellipseVBO;
        int              ellipseSegments;
        bool             bufferInitialized;
        void             setupEllipseBuffer(int num_segments);
    };
} // namespace umgebung