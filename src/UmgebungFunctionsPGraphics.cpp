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

#include "UmgebungFunctionsPGraphics.h"

namespace umgebung {
    void background(const float a) {
        if (g == nullptr) {
            return;
        }
        g->background(a);
    }

    void background(const float a, const float b, const float c, const float d) {
        if (g == nullptr) {
            return;
        }
        g->background(a, b, c, d);
    }

    void beginShape(const int shape) {
        if (g == nullptr) {
            return;
        }
        g->beginShape(shape);
    }

    void endShape(const bool close_shape) {
        if (g == nullptr) {
            return;
        }
        g->endShape(close_shape);
    }

    void bezier(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float x4, const float y4) {
        if (g == nullptr) {
            return;
        }
        g->bezier(x1, y1, x2, y2, x3, y3, x4, y4);
    }

    void bezier(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, const float x3, const float y3, const float z3, const float x4, const float y4, const float z4) {
        if (g == nullptr) {
            return;
        }
        g->bezier(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
    }

    void bezierDetail(const int detail) {
        if (g == nullptr) {
            return;
        }
        g->bezierDetail(detail);
    }

    void pointSize(const float point_size) {
        if (g == nullptr) {
            return;
        }
        g->pointSize(point_size);
    }

    void circle(const float x, const float y, const float diameter) {
        if (g == nullptr) {
            return;
        }
        g->circle(x, y, diameter);
    }

    void ellipse(const float x, const float y, const float width, const float height) {
        if (g == nullptr) {
            return;
        }
        g->ellipse(x, y, width, height);
    }

    void ellipseDetail(const int detail) {
        if (g == nullptr) {
            return;
        }
        g->ellipseDetail(detail);
    }

    void fill(const float r, const float g, const float b, const float a) {
        if (umgebung::g == nullptr) {
            return;
        }
        umgebung::g->fill(r, g, b, a);
    }

    void fill(const float brightness, const float a) {
        if (g == nullptr) {
            return;
        }
        g->fill(brightness, a);
    }

    void fill(const float a) {
        if (g == nullptr) {
            return;
        }
        g->fill(a);
    }

    void fill_color(const uint32_t c) {
        if (g == nullptr) {
            return;
        }
        g->fill_color(c);
    }

    void noFill() {
        if (g == nullptr) {
            return;
        }
        g->noFill();
    }

    void image(PImage* img, const float x, const float y, const float w, const float h) {
        if (g == nullptr) {
            return;
        }
        g->image(img, x, y, w, h);
    }

    void image(PImage* img, const float x, const float y) {
        if (g == nullptr) {
            return;
        }
        g->image(img, x, y);
    }

    void texture(PImage* img) {
        if (g == nullptr) {
            return;
        }
        g->texture(img);
    }

    PImage* loadImage(const std::string& filename) {
        if (g == nullptr) {
            return nullptr;
        }
        return g->loadImage(filename);
    }

    void line(const float x1, const float y1, const float x2, const float y2) {
        if (g == nullptr) {
            return;
        }
        g->line(x1, y1, x2, y2);
    }

    void line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
        if (g == nullptr) {
            return;
        }
        g->line(x1, y1, z1, x2, y2, z2);
    }

    void triangle(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, const float x3, const float y3, const float z3) {
        if (g == nullptr) {
            return;
        }
        g->triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3);
    }

    void point(const float x, const float y, const float z) {
        if (g == nullptr) {
            return;
        }
        g->point(x, y, z);
    }

    void rect(const float x, const float y, const float width, const float height) {
        if (g == nullptr) {
            return;
        }
        g->rect(x, y, width, height);
    }

    void stroke(const float r, const float g, const float b, const float a) {
        if (umgebung::g == nullptr) {
            return;
        }
        umgebung::g->stroke(r, g, b, a);
    }

    void stroke(const float brightness, const float a) {
        if (g == nullptr) {
            return;
        }
        g->stroke(brightness, a);
    }

    void stroke(const float a) {
        if (g == nullptr) {
            return;
        }
        g->stroke(a);
    }

    void stroke_color(const uint32_t c) {
        if (g == nullptr) {
            return;
        }
        g->stroke_color(c);
    }

    void noStroke() {
        if (g == nullptr) {
            return;
        }
        g->noStroke();
    }

    void strokeWeight(const float weight) {
        if (g == nullptr) {
            return;
        }
        g->strokeWeight(weight);
    }

    void vertex(const float x, const float y, const float z) {
        if (g == nullptr) {
            return;
        }
        g->vertex(x, y, z);
    }

    void vertex(const float x, const float y, const float z, const float u, const float v) {
        if (g == nullptr) {
            return;
        }
        g->vertex(x, y, z, u, v);
    }

    PFont* loadFont(const std::string& file, const float size) {
        if (g == nullptr) {
            return nullptr;
        }
        return g->loadFont(file, size);
    }

    void textFont(PFont* font) {
        if (g == nullptr) {
            return;
        }
        g->textFont(font);
    }

    void textSize(const float size) {
        if (g == nullptr) {
            return;
        }
        g->textSize(size);
    }

    void text(const char* value, const float x, const float y, const float z) {
        if (g == nullptr) {
            return;
        }
        g->text(value, x, y, z);
    }

    float textWidth(const std::string& text) {
        if (g == nullptr) {
            return 0.0f;
        }
        return g->textWidth(text);
    }

    void popMatrix() {
        if (g == nullptr) {
            return;
        }
        g->popMatrix();
    }

    void pushMatrix() {
        if (g == nullptr) {
            return;
        }
        g->pushMatrix();
    }

    void translate(const float x, const float y, const float z) {
        if (g == nullptr) {
            return;
        }
        g->translate(x, y, z);
    }

    void rotateX(const float angle) {
        if (g == nullptr) {
            return;
        }
        g->rotateX(angle);
    }

    void rotateY(const float angle) {
        if (g == nullptr) {
            return;
        }
        g->rotateY(angle);
    }

    void rotateZ(const float angle) {
        if (g == nullptr) {
            return;
        }
        g->rotateZ(angle);
    }

    void rotate(const float angle) {
        if (g == nullptr) {
            return;
        }
        g->rotate(angle);
    }

    void rotate(const float angle, const float x, const float y, const float z) {
        if (g == nullptr) {
            return;
        }
        g->rotate(angle, x, y, z);
    }

    void scale(const float x) {
        if (g == nullptr) {
            return;
        }
        g->scale(x);
    }

    void scale(const float x, const float y) {
        if (g == nullptr) {
            return;
        }
        g->scale(x, y);
    }

    void scale(const float x, const float y, const float z) {
        if (g == nullptr) {
            return;
        }
        g->scale(x, y, z);
    }

    void pixelDensity(const int density) {
        if (g == nullptr) {
            return;
        }
        g->pixelDensity(density);
    }

    void hint(const uint16_t property) {
        if (g == nullptr) {
            return;
        }
        g->hint(property);
    }
} // namespace umgebung