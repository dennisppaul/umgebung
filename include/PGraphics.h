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

        ~PGraphics() override;

        void stroke(float r, float g, float b, float a = 1.0);

        void stroke(float brightness, float a);

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

        static constexpr int ELLIPSE_NUM_SEGMENTS = 32;

#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    public:
        GLuint fbo, fbo_texture;
        int    fbo_width;
        int    fbo_height;
        GLint  fPreviousFBO = 0;

        void beginDraw() {
            /* save state */
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fPreviousFBO);
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix();

            // Bind the FBO for offscreen rendering
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glViewport(0, 0, fbo_width, fbo_height);

            // Setup projection and modelview matrices
            glMatrixMode(GL_PROJECTION);
            glPushMatrix(); // Save the current projection matrix
            glLoadIdentity();
            glOrtho(0, fbo_width, 0, fbo_height, -1, 1);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix(); // Save the current modelview matrix
            glLoadIdentity();
        }

        void endDraw() const {
            // Restore projection and modelview matrices
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            /* restore state */
            glBindFramebuffer(GL_FRAMEBUFFER, fPreviousFBO); // Restore the previously bound FBO
            glPopMatrix();
            glPopAttrib();
        }

        void bind() const override {
            glBindTexture(GL_TEXTURE_2D, fbo_texture);
        }

        // handled in `image()`
        // void draw_as_fbo(const float x, const float y, const float w, const float h) const {
        //     glEnable(GL_TEXTURE_2D);
        //     glColor4f(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        //     glBindTexture(GL_TEXTURE_2D, fbo_texture);
        //
        //     glBegin(GL_QUADS);
        //     glTexCoord2f(0, 0);
        //     glVertex2f(x, y);
        //
        //     glTexCoord2f(1, 0);
        //     glVertex2f(x + w, y);
        //
        //     glTexCoord2f(1, 1);
        //     glVertex2f(x + w, y + h);
        //
        //     glTexCoord2f(0, 1);
        //     glVertex2f(x, y + h);
        //     glEnd();
        //     glDisable(GL_TEXTURE_2D);
        // }

        void init(uint32_t* pixels, const int width, const int height, int format) override {
        // void init_as_fbo(const int width, const int height) {
            this->width  = width;
            this->height = height;
            fbo_width    = width;
            fbo_height   = height;
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glGenTextures(1, &fbo_texture);
            glBindTexture(GL_TEXTURE_2D, fbo_texture);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         fbo_width,
                         fbo_height,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                // Handle framebuffer incomplete error
                std::cerr << "ERROR Framebuffer is not complete!" << std::endl;
            }
            glViewport(0, 0, fbo_width, fbo_height);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER

#ifdef PGRAPHICS_USE_VBO
        GLuint ellipseVBO;
        int    ellipseSegments;
        bool   bufferInitialized;
        void   setupEllipseBuffer(int num_segments);
#endif // PGRAPHICS_USE_VBO
    };
} // namespace umgebung