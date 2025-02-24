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

#include "PGraphics.h"

namespace umgebung {

    class PGraphicsOpenGLv20 final : public PGraphics {
    public:
        PGraphicsOpenGLv20();

        void    strokeWeight(float weight) override;
        void    background(float a, float b, float c, float d = 1.0f) override;
        void    background(float a) override;
        void    rect(float x, float y, float width, float height) const override;
        void    ellipse(float x, float y, float width, float height) const override;
        void    circle(float x, float y, float radius) const override;
        void    ellipseDetail(int detail) override;
        void    line(float x1, float y1, float x2, float y2) const override;
        void    bezier(float x1, float y1,
                       float x2, float y2,
                       float x3, float y3,
                       float x4, float y4) const override;
        void    bezier(float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float x4, float y4, float z4) const override;
        void    bezierDetail(int detail) override;
        void    pointSize(float point_size) override;
        void    point(float x, float y, float z = 0.0f) const override;
        void    beginShape(int shape = POLYGON) override;
        void    endShape() override;
        void    vertex(float x, float y, float z = 0.0f) override;
        void    vertex(float x, float y, float z, float u, float v) override;
        PFont*  loadFont(const std::string& file, float size) override;
        void    textFont(PFont* font) override;
        void    textSize(float size) const override;
        void    text(const char* value, float x, float y, float z = 0.0f) const override;
        float   textWidth(const std::string& text) const override;
        PImage* loadImage(const std::string& filename) override;
        void    image(const PImage* img, float x, float y, float w, float h) const override;
        void    image(PImage* img, float x, float y) override;
        void    texture(const PImage* img) override;
        void    popMatrix() override;
        void    pushMatrix() override;
        void    translate(float x, float y, float z = 0.0f) override;
        void    rotateX(float angle) override;
        void    rotateY(float angle) override;
        void    rotateZ(float angle) override;
        void    rotate(float angle) override;
        void    rotate(float angle, float x, float y, float z) override;
        void    scale(float x) override;
        void    scale(float x, float y) override;
        void    scale(float x, float y, float z) override;
        void    pixelDensity(int density) override;
        void    hint(uint16_t property) override;
        void    text_str(const std::string& text, float x, float y, float z = 0.0f) const override;
        void    beginDraw() override;
        void    endDraw() const override;
        void    bind() const override;
        void    init(uint32_t* pixels, int width, int height, int format) override;

    private:
        static constexpr int ELLIPSE_NUM_SEGMENTS = 32;

        PFont* fCurrentFont           = nullptr;
        float  fPointSize             = 1;
        float  fStrokeWeight          = 1;
        bool   fEnabledTextureInShape = false;
        bool   fShapeBegun            = false;
        int    fEllipseDetail         = 32;
        int    fBezierDetail          = 20;
        int    fPixelDensity          = 1;
        int    fPreviousFBO{};
    };
} // namespace umgebung