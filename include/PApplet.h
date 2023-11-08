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

    int framebuffer_width;
    int framebuffer_height;
    float mouseX;
    float mouseY;
    float pmouseX;
    float pmouseY;
    int mouseButton;
    int key;
    int frameCount;
    float frameRate;

    PApplet() {
        this->width = DEFAULT_WINDOW_WIDTH;
        this->height = DEFAULT_WINDOW_HEIGHT;
        this->mouseX = 0;
        this->mouseY = 0;
        this->pmouseX = 0;
        this->pmouseY = 0;
        this->mouseButton = -1;
        this->key = -1;
        this->frameCount = 0;
        this->frameRate = 0;
    }

    void size(int _width, int _height) {
        this->width = _width;
        this->height = _height;
        this->framebuffer_width = _width;
        this->framebuffer_height = _height;
    }

    virtual void settings() {
        size(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        audio_devices(DEFAULT_AUDIO_DEVICE, DEFAULT_AUDIO_DEVICE);
        monitor = DEFAULT;
        antialiasing = DEFAULT;
    }

    virtual void setup() {}

    virtual void pre_draw() {
#ifndef DISABLE_GRAPHICS
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glScalef(1, -1, 1);
        glTranslatef(0, (float) -height, 0);

        glViewport(0, 0, framebuffer_width, framebuffer_height);
#endif
    }

    virtual void draw() {}

    virtual void post_draw() {}

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
