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
    fill_state.r      = r;
    fill_state.g      = g;
    fill_state.b      = b;
    fill_state.a      = alpha;
    fill_state.active = true;
}

void PGraphics::fill(const float gray, const float alpha) {
    fill(gray, gray, gray, alpha);
}

void PGraphics::fill_color(const uint32_t c) {
    color_inv(c, fill_state.r, fill_state.g, fill_state.b, fill_state.a);
    fill_state.active = true;
}

void PGraphics::noFill() {
    fill_state.active = false;
}

void PGraphics::stroke(const float r, const float g, const float b, const float alpha) {
    stroke_state.r      = r;
    stroke_state.g      = g;
    stroke_state.b      = b;
    stroke_state.a      = alpha;
    stroke_state.active = true;
}

void PGraphics::stroke(const float gray, const float alpha) {
    stroke_state.r      = gray;
    stroke_state.g      = gray;
    stroke_state.b      = gray;
    stroke_state.a      = alpha;
    stroke_state.active = true;
}

void PGraphics::stroke_color(const uint32_t c) {
    color_inv(c, stroke_state.r, stroke_state.g, stroke_state.b, stroke_state.a);
    stroke_state.active = true;
}

void PGraphics::stroke(const float a) {
    stroke(a, a, a);
}

void PGraphics::noStroke() {
    stroke_state.active = false;
}
