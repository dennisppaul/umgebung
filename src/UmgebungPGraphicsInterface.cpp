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

#include "UmgebungPGraphicsInterface.h"

namespace umgebung {

    void background(const float a) {
        if (g == nullptr) {
            return;
        }
        g->background(a);
    }

    void stroke(const float a) {
        if (g == nullptr) {
            return;
        }
        g->stroke(a);
    }

    void noFill() {
        if (g == nullptr) {
            return;
        }
        g->noFill();
    }

    void fill(const float a) {
        if (g == nullptr) {
            return;
        }
        g->fill(a);
    }

    void noStroke() {
        if (g == nullptr) {
            return;
        }
        g->noStroke();
    }

    void rect(const float x, const float y, const float width, const float height) {
        if (g == nullptr) {
            return;
        }
        g->rect(x, y, width, height);
    }
} // namespace umgebung