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

void PGraphics::fill(const float r, const float g, const float b, const float alpha) {
    current_fill_color.r      = r;
    current_fill_color.g      = g;
    current_fill_color.b      = b;
    current_fill_color.a      = alpha;
    current_fill_color.active = true;
}

void PGraphics::fill(const float gray, const float alpha) {
    fill(gray, gray, gray, alpha);
}

void PGraphics::fill_color(const uint32_t c) {
    color_inv(c, current_fill_color.r, current_fill_color.g, current_fill_color.b, current_fill_color.a);
    current_fill_color.active = true;
}

void PGraphics::noFill() {
    current_fill_color.active = false;
}

void PGraphics::stroke(const float r, const float g, const float b, const float alpha) {
    current_stroke_color.r      = r;
    current_stroke_color.g      = g;
    current_stroke_color.b      = b;
    current_stroke_color.a      = alpha;
    current_stroke_color.active = true;
}

void PGraphics::stroke(const float gray, const float alpha) {
    current_stroke_color.r      = gray;
    current_stroke_color.g      = gray;
    current_stroke_color.b      = gray;
    current_stroke_color.a      = alpha;
    current_stroke_color.active = true;
}

void PGraphics::stroke_color(const uint32_t c) {
    color_inv(c, current_stroke_color.r, current_stroke_color.g, current_stroke_color.b, current_stroke_color.a);
    current_stroke_color.active = true;
}

void PGraphics::stroke(const float a) {
    stroke(a, a, a);
}

void PGraphics::noStroke() {
    current_stroke_color.active = false;
}
