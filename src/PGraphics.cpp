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

#include "UmgebungFunctionsAdditional.h"
#include "PGraphics.h"

using namespace umgebung;

PGraphics::PGraphics() : PImage(0, 0, 0) {
    PGraphics::fill(1.0f);
    PGraphics::stroke(0.0f);
}

void PGraphics::stroke(const float r, const float g, const float b, const float alpha) {
    stroke_color.r      = r;
    stroke_color.g      = g;
    stroke_color.b      = b;
    stroke_color.a      = alpha;
    stroke_color.active = true;
}

void PGraphics::stroke(const float gray, const float alpha) {
    stroke_color.r      = gray;
    stroke_color.g      = gray;
    stroke_color.b      = gray;
    stroke_color.a      = alpha;
    stroke_color.active = true;
}

void PGraphics::stroke_i(const uint32_t c) {
    color_inv(c, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
    stroke_color.active = true;
}

void PGraphics::fill(const float r, const float g, const float b, const float alpha) {
    fill_color.r      = r;
    fill_color.g      = g;
    fill_color.b      = b;
    fill_color.a      = alpha;
    fill_color.active = true;
}

void PGraphics::fill(const float gray, const float alpha) {
    fill(gray, gray, gray, alpha);
}

void PGraphics::fill_i(const uint32_t c) {
    color_inv(c, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
    fill_color.active = true;
}

void PGraphics::noFill() {
    fill_color.active = false;
}


void PGraphics::stroke(const float a) {
    stroke(a, a, a);
}

void PGraphics::noStroke() {
    stroke_color.active = false;
}

