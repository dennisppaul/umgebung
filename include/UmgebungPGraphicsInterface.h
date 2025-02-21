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

#include "UmgebungConstants.h"
#include "PGraphics.h"
#include "PImage.h"
#include "PFont.h"

namespace umgebung {
    /* --- fields --- */
    inline PGraphicsOpenGL2* g = nullptr;

    /* --- functions --- */
    void    background(float a);
    void    background(float a, float b, float c, float d = 1.0);
    void    beginShape(int shape = POLYGON);
    void    endShape();
    void    bezier(float x1, float y1,
                   float x2, float y2,
                   float x3, float y3,
                   float x4, float y4);
    void    bezier(float x1, float y1, float z1,
                   float x2, float y2, float z2,
                   float x3, float y3, float z3,
                   float x4, float y4, float z4);
    void    bezierDetail(int detail);
    void    pointSize(float point_size);
    void    circle(float x, float y, float radius);
    void    ellipse(float x, float y, float width, float height);
    void    ellipseDetail(int detail);
    void    fill(float r, float g, float b, float a = 1.0);
    void    fill(float brightness, float a);
    void    fill(float a);
    void    fill(uint32_t c);
    void    noFill();
    void    image(const PImage* img, float x, float y, float w, float h);
    void    image(PImage* img, float x, float y);
    void    texture(const PImage* img);
    PImage* loadImage(const std::string& filename);
    void    line(float x1, float y1, float x2, float y2);
    void    point(float x, float y, float z = 0.0);
    void    rect(float x, float y, float width, float height);
    void    stroke(float r, float g, float b, float a = 1.0);
    void    stroke(float brightness, float a);
    void    stroke(float a);
    void    stroke(uint32_t c);
    void    noStroke();
    void    strokeWeight(float weight);
    void    vertex(float x, float y, float z = 0.0);
    void    vertex(float x, float y, float z, float u, float v);
    PFont*  loadFont(const std::string& file, float size); // @development maybe use smart pointers here
    void    textFont(PFont* font);
    void    textSize(float size);
    void    text(const char* value, float x, float y, float z = 0.0f);
    float   textWidth(const std::string& text);
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
    void    pixelDensity(int density);
} // namespace umgebung