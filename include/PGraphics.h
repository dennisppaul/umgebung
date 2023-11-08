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

#include <string>
#include "UmgebungConstants.h"

class PImage;

class PFont;

class PGraphics {
public:
    int width = 0;
    int height = 0;

    void stroke(float r, float g, float b, float a = 1.0);

    void stroke(float a);

    void noStroke();

    void fill(float r, float g, float b, float a = 1.0);

    void fill(float a);

    void noFill();

    void background(float a, float b, float c, float d = 1.0);

    void background(float a);

    void rect(float x, float y, float _width, float _height);

    void line(float x1, float y1, float x2, float y2);

    void pointSize(float point_size);

    void point(float x, float y, float z = 0.0);

    void beginShape(int shape = POLYGON);

    void endShape();

    void vertex(float x, float y, float z = 0.0);

    PFont *loadFont(const char *file, float size); // @development maybe use smart pointers here

    void textFont(PFont *font);

    void textSize(float size);

    void text(const std::string &text, float x, float y);

    PImage *loadImage(const std::string &filename);

    void image(PImage *img, float x, float y, float w, float h);

    void image(PImage *img, float x, float y);

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
    PFont *fCurrentFont = nullptr;
    float fPointSize = 1;

    struct {
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 1;
        bool active = false;
    } fill_color, stroke_color;
};