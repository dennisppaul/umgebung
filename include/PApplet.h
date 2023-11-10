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

#include "Umgebung.h"

class PApplet : public PGraphics {
public:

    int   framebuffer_width;
    int   framebuffer_height;
    float mouseX;
    float mouseY;
    float pmouseX;
    float pmouseY;
    int   mouseButton;
    int   key;
    int   frameCount;
    float frameRate;

#ifndef DISABLE_GRAPHICS
#if RENDER_INTO_FRAMEBUFFER
    GLuint framebuffer, texture;
#endif // RENDER_INTO_FRAMEBUFFER
#endif // DISABLE_GRAPHICS

    PApplet() {
        this->width       = DEFAULT_WINDOW_WIDTH;
        this->height      = DEFAULT_WINDOW_HEIGHT;
        this->mouseX      = 0;
        this->mouseY      = 0;
        this->pmouseX     = 0;
        this->pmouseY     = 0;
        this->mouseButton = -1;
        this->key         = -1;
        this->frameCount  = 0;
        this->frameRate   = 0;
    }

    /**
     * called right before `setup()`. at this point OpenGL is valid.
     * TODO maybe move to PGraphics
     */
    void init() {
#ifndef DISABLE_GRAPHICS
#if RENDER_INTO_FRAMEBUFFER
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            // Handle framebuffer incomplete error
            std::cerr << "ERROR Framebuffer is not complete!" << std::endl;
        }
        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
#endif // RENDER_INTO_FRAMEBUFFER
#endif // DISABLE_GRAPHICS
    }

    void size(int _width, int _height) {
        this->width              = _width;
        this->height             = _height;
        this->framebuffer_width  = _width;
        this->framebuffer_height = _height;
        // TODO maybe implement some kind of mechanism that alerts if width or height are changed after this point
    }

    virtual void settings() {
        size(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        audio_devices(DEFAULT_AUDIO_DEVICE, DEFAULT_AUDIO_DEVICE);
        monitor      = DEFAULT;
        antialiasing = DEFAULT;
    }

    virtual void setup() {}

    virtual void pre_draw() {
#ifndef DISABLE_GRAPHICS
#if RENDER_INTO_FRAMEBUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif // RENDER_INTO_FRAMEBUFFER
        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glScalef(1, -1, 1);
        glTranslatef(0, (float) -height, 0);
#endif // DISABLE_GRAPHICS
    }

    virtual void draw() {}

    virtual void post_draw() {
#ifndef DISABLE_GRAPHICS
//        glFlush();
//        glFinish();
#endif // DISABLE_GRAPHICS
#ifndef DISABLE_GRAPHICS
#if RENDER_INTO_FRAMEBUFFER
        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Render the FBO to the entire screen
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);

        glViewport(0, 0, framebuffer_width, framebuffer_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, framebuffer_width, 0, framebuffer_height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glBindTexture(GL_TEXTURE_2D, texture);
        glEnable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 1);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0, 0);
        glTexCoord2f(1.0, 0.0);
        glVertex2f(framebuffer_width, 0);
        glTexCoord2f(1.0, 1.0);
        glVertex2f(framebuffer_width, framebuffer_height);
        glTexCoord2f(0.0, 1.0);
        glVertex2f(0, framebuffer_height);
        glEnd();

//        /* with padding of 10px */
//        glBegin(GL_QUADS);
//        glTexCoord2f(0.0, 0.0);
//        glVertex2f(10, 10);
//        glTexCoord2f(1.0, 0.0);
//        glVertex2f(framebuffer_width - 10, 10);
//        glTexCoord2f(1.0, 1.0);
//        glVertex2f(framebuffer_width - 10, framebuffer_height - 10);
//        glTexCoord2f(0.0, 1.0);
//        glVertex2f(10, framebuffer_height - 10);
//        glEnd();

//        /* 10% tiny view */
//        glBegin(GL_QUADS);
//        glTexCoord2f(0.0, 0.0);
//        glVertex2f(20, 20);
//        glTexCoord2f(1.0, 0.0);
//        glVertex2f(20 + framebuffer_width * 0.1, 20);
//        glTexCoord2f(1.0, 1.0);
//        glVertex2f(20 + framebuffer_width * 0.1, 20 + framebuffer_height * 0.1);
//        glTexCoord2f(0.0, 1.0);
//        glVertex2f(20, 20 + framebuffer_height * 0.1);
//        glEnd();

//        glDisable(GL_TEXTURE_2D);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glPopAttrib();

#endif // RENDER_INTO_FRAMEBUFFER
#endif // DISABLE_GRAPHICS

    }

    virtual void finish() {}

    virtual void audioblock(const float *input, float *output, unsigned long length) {};

    virtual void mouseMoved() {}

    virtual void mouseDragged() {}

    virtual void mousePressed() {}

    virtual void mouseReleased() {}

    virtual void keyPressed() {}

    virtual void keyReleased() {}

};

extern PApplet *instance();
