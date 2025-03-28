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

#include <iostream>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "earcut.hpp"
#include "polypartition.h"
#include "clipper2/clipper.h"

#include "Umgebung.h"
#include "UmgebungFunctionsGraphics.h"
#include "UmgebungFunctionsAdditional.h"
#include "PGraphics.h"
#include "Vertex.h"
#include "Geometry.h"

using namespace umgebung;

PGraphics::PGraphics() : PImage(0, 0, 0) {
    PGraphics::fill(1.0f);
    PGraphics::stroke(0.0f);
    PGraphics::ellipseDetail(ELLIPSE_DETAIL_DEFAULT);
    generate_box(box_vertices_LUT);
    generate_sphere(sphere_vertices_LUT);
}

void PGraphics::beginDraw() {
    reset_mvp_matrices();
    blendMode(BLEND);
}

void PGraphics::endDraw() {
    restore_mvp_matrices();
}

void PGraphics::hint(const uint16_t property) {}

void PGraphics::pixelDensity(const int density) {
    static bool emitted_warning = false;
    if (!emitted_warning && init_properties_locked) {
        warning("`pixelDensity()` should not be set after context is created. use `retina_support` in settings instead.");
        emitted_warning = true;
    }
    pixel_density = density;
}

void PGraphics::stroke_properties(const float stroke_join_round_resolution,
                                  const float stroke_cap_round_resolution,
                                  const float stroke_join_miter_max_angle) {
    this->stroke_join_round_resolution = stroke_join_round_resolution;
    this->stroke_cap_round_resolution  = stroke_cap_round_resolution;
    this->stroke_join_miter_max_angle  = stroke_join_miter_max_angle;
}

void PGraphics::background(PImage* img) {
    background(0, 0, 0, 0);
    fill(1);
    image(img, 0, 0, framebuffer.width, framebuffer.height);
}

void PGraphics::background(const float a) {
    background(a, a, a);
}

void PGraphics::background(const float a, const float b, const float c, const float d) {
    IMPL_background(a, b, c, d);
}

/* --- transform matrices --- */

void PGraphics::popMatrix() {
    if (!model_matrix_stack.empty()) {
        model_matrix = model_matrix_stack.back();
        model_matrix_stack.pop_back();
    }
}


void PGraphics::pushMatrix() {
    model_matrix_stack.push_back(model_matrix);
}

void PGraphics::resetMatrix() { // NOTE this just resets the model view matrix
    model_matrix = glm::mat4(1.0f);
}

void PGraphics::printMatrix(const glm::mat4& matrix) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << matrix[j][i] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PGraphics::printMatrix() {
    printMatrix(model_matrix);
}

void PGraphics::translate(const float x, const float y, const float z) {
    model_matrix       = glm::translate(model_matrix, glm::vec3(x, y, z));
    model_matrix_dirty = true;
}

void PGraphics::rotateX(const float angle) {
    model_matrix       = glm::rotate(model_matrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix_dirty = true;
}

void PGraphics::rotateY(const float angle) {
    model_matrix       = glm::rotate(model_matrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix_dirty = true;
}

void PGraphics::rotateZ(const float angle) {
    model_matrix       = glm::rotate(model_matrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix_dirty = true;
}

void PGraphics::rotate(const float angle) {
    model_matrix       = glm::rotate(model_matrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix_dirty = true;
}

void PGraphics::rotate(const float angle, const float x, const float y, const float z) {
    model_matrix       = glm::rotate(model_matrix, angle, glm::vec3(x, y, z));
    model_matrix_dirty = true;
}

void PGraphics::scale(const float x) {
    model_matrix       = glm::scale(model_matrix, glm::vec3(x, x, x));
    model_matrix_dirty = true;
}

void PGraphics::scale(const float x, const float y) {
    model_matrix       = glm::scale(model_matrix, glm::vec3(x, y, 1));
    model_matrix_dirty = true;
}

void PGraphics::scale(const float x, const float y, const float z) {
    model_matrix       = glm::scale(model_matrix, glm::vec3(x, y, z));
    model_matrix_dirty = true;
}

/* --- color, stroke, and fill --- */

void PGraphics::fill(const float r, const float g, const float b, const float alpha) {
    color_fill.r      = r;
    color_fill.g      = g;
    color_fill.b      = b;
    color_fill.a      = alpha;
    color_fill.active = true;
}

void PGraphics::fill(const float gray, const float alpha) {
    fill(gray, gray, gray, alpha);
}

void PGraphics::fill_color(const uint32_t c) {
    color_inv(c, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
    color_fill.active = true;
}

void PGraphics::noFill() {
    color_fill.active = false;
}

void PGraphics::stroke(const float r, const float g, const float b, const float alpha) {
    color_stroke.r      = r;
    color_stroke.g      = g;
    color_stroke.b      = b;
    color_stroke.a      = alpha;
    color_stroke.active = true;
}

void PGraphics::stroke(const float gray, const float alpha) {
    color_stroke.r      = gray;
    color_stroke.g      = gray;
    color_stroke.b      = gray;
    color_stroke.a      = alpha;
    color_stroke.active = true;
}

void PGraphics::stroke_color(const uint32_t c) {
    color_inv(c, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    color_stroke.active = true;
}

void PGraphics::stroke(const float a) {
    stroke(a, a, a);
}

void PGraphics::noStroke() {
    color_stroke.active = false;
}

void PGraphics::strokeWeight(const float weight) {
    stroke_weight = weight;
}

/**
 *  can be MITER, BEVEL, ROUND, NONE, BEVEL_FAST or MITER_FAST
 * @param join
 */
void PGraphics::strokeJoin(const int join) {
    stroke_join_mode = join;
}

/**
 * can be PROJECT, ROUND, POINTED or SQUARE
 * @param cap
 */
void PGraphics::strokeCap(const int cap) {
    stroke_cap_mode = cap;
}

void PGraphics::rectMode(const int mode) {
    rect_mode = mode;
}

void PGraphics::ellipseMode(const int mode) {
    ellipse_mode = mode;
}

void PGraphics::ellipseDetail(const int detail) {
    if (ellipse_detail == detail) {
        return;
    }
    if (detail < ELLIPSE_DETAIL_MIN) {
        return;
    }
    ellipse_detail = detail;
    resize_ellipse_points_LUT();
}

void PGraphics::bezier(const float x1, const float y1,
                       const float x2, const float y2,
                       const float x3, const float y3,
                       const float x4, const float y4) {
    if (!color_stroke.active) {
        return;
    }

    if (bezier_detail < 2) {
        return;
    }

    const int   segments = bezier_detail;
    const float step     = 1.0f / static_cast<float>(segments);

    beginShape(LINE_STRIP);
    for (int i = 0; i < segments + 1; ++i) {
        const float t = static_cast<float>(i) * step;
        const float u = 1.0f - t;

        const float b0 = u * u * u;
        const float b1 = 3 * u * u * t;
        const float b2 = 3 * u * t * t;
        const float b3 = t * t * t;

        const float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        const float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;

        vertex(x, y);
    }
    endShape();
}

void PGraphics::bezier(const float x1, const float y1, const float z1,
                       const float x2, const float y2, const float z2,
                       const float x3, const float y3, const float z3,
                       const float x4, const float y4, const float z4) {
    if (!color_stroke.active) {
        return;
    }
    if (bezier_detail < 2) {
        return;
    }

    const int   segments = bezier_detail;
    const float step     = 1.0f / static_cast<float>(segments);

    beginShape(LINE_STRIP);
    for (int i = 0; i < segments + 1; ++i) {
        const float t = static_cast<float>(i) * step;
        const float u = 1.0f - t;

        const float b0 = u * u * u;
        const float b1 = 3 * u * u * t;
        const float b2 = 3 * u * t * t;
        const float b3 = t * t * t;

        const float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        const float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;
        const float z = b0 * z1 + b1 * z2 + b2 * z3 + b3 * z4;

        vertex(x, y, z);
    }
    endShape();
}

void PGraphics::bezierDetail(const int detail) {
    bezier_detail = detail;
}

static glm::vec2 hermite(const float t, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& m1, const glm::vec2& m2) {
    const float t2 = t * t;
    const float t3 = t2 * t;

    return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 +
           (t3 - 2.0f * t2 + t) * m1 +
           (-2.0f * t3 + 3.0f * t2) * p2 +
           (t3 - t2) * m2;
}

void PGraphics::curve(const float x1, const float y1,
                      const float x2, const float y2,
                      const float x3, const float y3,
                      const float x4, const float y4) {
    const glm::vec2 p1(x1, y1);
    const glm::vec2 p2(x2, y2);
    const glm::vec2 p3(x3, y3);
    const glm::vec2 p4(x4, y4);

    const glm::vec2 m1 = (1.0f - curve_tightness) * 0.5f * (p3 - p1);
    const glm::vec2 m2 = (1.0f - curve_tightness) * 0.5f * (p4 - p2);

    const int segments = curve_detail;
    glm::vec2 prev     = p2;

    for (int i = 1; i <= segments; ++i) {
        const float     t  = i / static_cast<float>(segments);
        const glm::vec2 pt = hermite(t, p2, p3, m1, m2);
        line(prev.x, prev.y, pt.x, pt.y); // your existing line() function
        prev = pt;
    }
}

static glm::vec3 hermite(const float t, const glm::vec3& p1, const glm::vec3& p2,
                         const glm::vec3& m1, const glm::vec3& m2) {
    const float t2 = t * t;
    const float t3 = t2 * t;

    return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 +
           (t3 - 2.0f * t2 + t) * m1 +
           (-2.0f * t3 + 3.0f * t2) * p2 +
           (t3 - t2) * m2;
}

void PGraphics::curve(const float x1, const float y1, const float z1,
                      const float x2, const float y2, const float z2,
                      const float x3, const float y3, const float z3,
                      const float x4, const float y4, const float z4) {
    const glm::vec3 p1(x1, y1, z1);
    const glm::vec3 p2(x2, y2, z2);
    const glm::vec3 p3(x3, y3, z3);
    const glm::vec3 p4(x4, y4, z4);

    const glm::vec3 m1 = (1.0f - curve_tightness) * 0.5f * (p3 - p1);
    const glm::vec3 m2 = (1.0f - curve_tightness) * 0.5f * (p4 - p2);

    const int segments = curve_detail;
    glm::vec3 prev     = p2;

    for (int i = 1; i <= segments; ++i) {
        const float     t  = i / static_cast<float>(segments);
        const glm::vec3 pt = hermite(t, p2, p3, m1, m2);
        line(prev.x, prev.y, prev.z, pt.x, pt.y, pt.z); // 3D line function
        prev = pt;
    }
}

void PGraphics::curveDetail(const int detail) {
    curve_detail = detail;
}

void PGraphics::curveTightness(const float tightness) {
    curve_tightness = tightness;
}

// std::vector<glm::vec3> curve_vertices;
// void PGraphics::beginShape() {
//     curve_vertices.clear();
//     // other shape setup
// }
// void PGraphics::curveVertex(float x, float y, float z) {
//     curve_vertices.emplace_back(x, y, z);
//
//     if (curve_vertices.size() < 4) return;
//
//     const glm::vec3& p1 = curve_vertices[curve_vertices.size() - 4];
//     const glm::vec3& p2 = curve_vertices[curve_vertices.size() - 3];
//     const glm::vec3& p3 = curve_vertices[curve_vertices.size() - 2];
//     const glm::vec3& p4 = curve_vertices[curve_vertices.size() - 1];
//
//     const glm::vec3 m1 = (1.0f - curve_tightness) * 0.5f * (p3 - p1);
//     const glm::vec3 m2 = (1.0f - curve_tightness) * 0.5f * (p4 - p2);
//
//     const int segments = curve_detail;
//     glm::vec3 prev     = p2;
//
//     for (int i = 1; i <= segments; ++i) {
//         const float     t  = i / float(segments);
//         const glm::vec3 pt = hermite(t, p2, p3, m1, m2);
//         line(prev.x, prev.y, prev.z, pt.x, pt.y, pt.z);
//         prev = pt;
//     }
// }

void PGraphics::ellipse(const float x, const float y, const float width, const float height) {
    if (!color_fill.active && !color_stroke.active) {
        return;
    }

    // TODO: Implement `ellipseMode()`
    const float radiusX = width * 0.5f;
    const float radiusY = height * 0.5f;

    std::vector<glm::vec3> points;
    points.reserve(ellipse_detail + 1);

    // TODO create and recompute LUT for when `ellipse_detail` changes
    float i_f = 0.0f;
    for (int i = 0; i <= ellipse_detail; ++i, i_f += 1.0f) {
        points.emplace_back(x + radiusX * ellipse_points_LUT[i].x,
                            y + radiusY * ellipse_points_LUT[i].y,
                            0.0f);
    }

    beginShape(POLYGON);
    points.pop_back();
    for (const auto& p: points) {
        // TODO maybe add texcoords
        vertex(p.x, p.y, 0.0f);
    }
    endShape(CLOSE);
}

void PGraphics::image(PImage* img, const float x, const float y, float w, float h) {
    if (!color_fill.active) {
        return;
    }

    if (img == nullptr) {
        error("img is null");
        return;
    }

    if (w < 0) {
        w = img->width;
    }
    if (h < 0) {
        h = img->height;
    }

    const bool _stroke_active = color_stroke.active;
    noStroke();
    push_texture_id();
    texture(img);
    rect(x, y, w, h);
    pop_texture_id();
    color_stroke.active = _stroke_active;
}

void PGraphics::image(PImage* img, const float x, const float y) {
    image(img, x, y, img->width, img->height);
}

void PGraphics::circle(const float x, const float y, const float diameter) {
    ellipse(x, y, diameter, diameter);
}

PImage* PGraphics::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}

PFont* PGraphics::loadFont(const std::string& file, const float size) {
    auto* font = new PFont(file, size); // TODO what about pixel_density … see FTGL implementation
    return font;
}

void PGraphics::textFont(PFont* font) {
    current_font = font;
}

void PGraphics::textSize(const float size) {
    if (current_font == nullptr) {
        return;
    }
    current_font->textSize(size);
}

void PGraphics::text(const char* value, const float x, const float y, const float z) {
    text_str(value, x, y, z);
}

float PGraphics::textWidth(const std::string& text) {
    if (current_font == nullptr) {
        return 0;
    }
    return current_font->textWidth(text.c_str());
}

void PGraphics::textAlign(const int alignX, const int alignY) {
    if (current_font == nullptr) {
        return;
    }
    current_font->textAlign(alignX, alignY);
}

float PGraphics::textAscent() {
    if (current_font == nullptr) {
        return 0;
    }
    return current_font->textAscent();
}

float PGraphics::textDescent() {
    if (current_font == nullptr) {
        return 0;
    }
    return current_font->textDescent();
}

void PGraphics::textLeading(const float leading) {
    if (current_font == nullptr) {
        return;
    }
    current_font->textLeading(leading);
}

void PGraphics::text_str(const std::string& text, const float x, const float y, const float z) {
    if (current_font == nullptr) {
        return;
    }
    if (!color_fill.active) {
        return;
    }

    current_font->draw(this, text, x, y, z);
}

void PGraphics::texture(PImage* img) {
    IMPL_set_texture(img);
}

void PGraphics::point(const float x, const float y, const float z) {
    beginShape(POINTS);
    vertex(x, y, z);
    endShape();
}

void PGraphics::pointSize(const float size) {
    point_size = size;
}

void PGraphics::line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
    if (!color_stroke.active) {
        return;
    }
    beginShape(LINES);
    vertex(x1, y1, z1);
    vertex(x2, y2, z2);
    endShape();
}

void PGraphics::line(const float x1, const float y1, const float x2, const float y2) {
    line(x1, y1, 0, x2, y2, 0);
}

void PGraphics::triangle(const float x1, const float y1, const float z1,
                         const float x2, const float y2, const float z2,
                         const float x3, const float y3, const float z3) {
    beginShape(TRIANGLES);
    vertex(x1, y1, z1);
    vertex(x2, y2, z2);
    vertex(x3, y3, z3);
    endShape();
}

void PGraphics::quad(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, const float x3, const float y3, const float z3, const float x4, const float y4, const float z4) {
    beginShape(QUADS);
    vertex(x1, y1, z1, 1, 1);
    vertex(x2, y2, z2, 0, 1);
    vertex(x3, y3, z3, 0, 0);
    vertex(x4, y4, z4, 1, 0);
    endShape();
}

void PGraphics::rect(const float x, const float y, const float width, const float height) {
    if (!color_stroke.active && !color_fill.active) {
        return;
    }

    // compute rectangle corners using glm::vec2
    glm::vec2 p1, p2;
    switch (rect_mode) {
        case CORNERS:
            p1 = {x, y};
            p2 = {width, height};
            break;
        case CENTER:
            p1 = {x - width * 0.5f, y - height * 0.5f};
            p2 = {x + width * 0.5f, y + height * 0.5f};
            break;
        case RADIUS:
            p1 = {x - width, y - height};
            p2 = {x + width, y + height};
            break;
        case CORNER:
        default:
            p1 = {x, y};
            p2 = {x + width, y + height};
            break;
    }

    // // define colors once (avoiding redundant glm::vec4 conversions)
    // const glm::vec4 fill_color   = as_vec4(color_fill);
    // const glm::vec4 stroke_color = as_vec4(color_stroke);

    // define rectangle vertices (shared for fill and stroke)
    static constexpr uint8_t                  NUM_VERTICES  = 4;
    const std::array<glm::vec3, NUM_VERTICES> rect_vertices = {
        glm::vec3{p1.x, p1.y, 0},
        glm::vec3{p2.x, p1.y, 0},
        glm::vec3{p2.x, p2.y, 0},
        glm::vec3{p1.x, p2.y, 0}};
    constexpr std::array<glm::vec2, NUM_VERTICES> rect_tex_coords = {
        glm::vec2{0, 0},
        glm::vec2{1, 0},
        glm::vec2{1, 1},
        glm::vec2{0, 1}};

    beginShape(QUADS);
    for (int i = 0; i < rect_vertices.size(); ++i) {
        vertex_vec(rect_vertices[i], rect_tex_coords[i]);
    }
    endShape();
}

void PGraphics::box(const float width, const float height, const float depth) {
    beginShape(TRIANGLES);
    for (const auto& v: box_vertices_LUT) {
        vertex(v.x * width, v.y * height, v.z * depth);
    }
    endShape();
}

void PGraphics::sphere(const float width, const float height, const float depth) {
    beginShape(TRIANGLES);
    for (const auto& v: sphere_vertices_LUT) {
        vertex(v.x * width, v.y * height, v.z * depth);
    }
    endShape();
}

void PGraphics::resize_ellipse_points_LUT() {
    if (ellipse_detail < ELLIPSE_DETAIL_MIN) {
        return;
    }
    ellipse_points_LUT.clear();
    ellipse_points_LUT.resize(ellipse_detail + 1); // Resize instead of reserve

    constexpr float PI         = 3.14159265358979323846f;
    const float     deltaTheta = (2.0f * PI) / static_cast<float>(ellipse_detail);

    for (int i = 0; i <= ellipse_detail; ++i) {
        const float theta     = deltaTheta * static_cast<float>(i);
        ellipse_points_LUT[i] = {std::cos(theta), std::sin(theta)};
    }
}

/* --- triangulation --- */

// TODO move to Gemoetry or Triangulation

// EARCUT

std::vector<Vertex> PGraphics::triangulate_faster(const std::vector<Vertex>& vertices) {
    std::vector<std::vector<std::array<float, 2>>> polygon;
    polygon.emplace_back(); // Outer boundary

    // convert Vertex to 2D format for earcut (ignore z)
    for (const auto& v: vertices) {
        polygon[0].push_back({v.position.x, v.position.y});
    }

    // perform triangulation
    const std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
    std::vector<Vertex>         triangleList;

    for (size_t i = 0; i < indices.size(); i++) {
        const int index = indices[i];
        if (i < indices.size()) {
            triangleList.push_back(vertices[index]);
        }
    }
    return triangleList;
}

// POLYPARTITION + CLIPPER2

Clipper2Lib::PathD convertToClipperPath(const std::vector<Vertex>& vertices) {
    Clipper2Lib::PathD path;
    for (const auto& v: vertices) {
        path.push_back({v.position.x, v.position.y});
    }
    return path;
}

std::vector<TPPLPoly> convertToPolyPartition(const Clipper2Lib::PathsD& paths) {
    std::vector<TPPLPoly> polys;
    for (const auto& path: paths) {
        TPPLPoly poly;
        poly.Init(path.size());
        for (size_t i = 0; i < path.size(); ++i) {
            poly[i].x = path[i].x;
            poly[i].y = path[i].y;
        }
        polys.push_back(poly);
    }
    return polys;
}

std::vector<Vertex> PGraphics::triangulate_good(const std::vector<Vertex>& vertices) {
    const std::vector<Vertex> triangles = triangulator.triangulate(vertices, Triangulator::Winding::WINDING_ODD);
    return triangles;
}

std::vector<Vertex> PGraphics::triangulate_better_quality(const std::vector<Vertex>& vertices) {
    const glm::vec4 first_color  = vertices[0].color;
    const glm::vec4 first_normal = vertices[0].normal;

    TPPLPartition partitioner;

    // use Clipper2 to resolve self-intersections
    const Clipper2Lib::PathsD inputPaths = {convertToClipperPath(vertices)};
    const Clipper2Lib::PathsD fixedPaths = Clipper2Lib::Union(inputPaths, Clipper2Lib::FillRule::NonZero);

    if (fixedPaths.empty()) {
        std::cerr << "Clipper2 failed to fix the polygon!" << std::endl;
        return {};
    }

    // convert to PolyPartition format
    std::vector<TPPLPoly> convexPolygons;
    for (auto& poly: convertToPolyPartition(fixedPaths)) {
        std::list<TPPLPoly> convexParts;
        if (!partitioner.ConvexPartition_HM(&poly, &convexParts)) {
            std::cerr << "Convex partitioning failed!" << std::endl;
            continue;
        }
        convexPolygons.insert(convexPolygons.end(), convexParts.begin(), convexParts.end());
    }

    // triangulate each convex part
    std::vector<Vertex> triangleList;
    for (auto& part: convexPolygons) {
        std::list<TPPLPoly> triangles;
        if (!partitioner.Triangulate_EC(&part, &triangles)) {
            std::cerr << "Triangulation failed for a convex part!" << std::endl;
            continue;
        }

        // extract triangle vertices
        for (const auto& tri: triangles) {
            for (int i = 0; i < 3; ++i) {
                triangleList.emplace_back(glm::vec3(tri[i].x, tri[i].y, 0.0f),
                                          first_color,
                                          glm::vec2(0.0f, 0.0f),
                                          first_normal);
            }
        }
    }
    return triangleList;
}

void PGraphics::reset_mvp_matrices() {
    /* model_matrix */
    resetMatrix();
    /* projection_matrix */
    in_camera_block = false;
    perspective(DEFAULT_CAMERA_FOV_RADIANS, width / height, 0.1f, depth_range);
    projection_matrix[1][1] *= -1.0f;
    /* view_matrix */
    camera();

    // orthographic projection NOTE use framebuffer.width and framebuffer.height
    // projection_matrix_2D = glm::ortho(0.0f, static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height), 0.0f);

    model_matrix_dirty = false;
}

void PGraphics::restore_mvp_matrices() {
    // NOTE nothing to restore here
}

void PGraphics::to_screen_space(glm::vec3& model_position) const {
    glm::vec4 clipPos = projection_matrix * view_matrix * model_matrix * glm::vec4(model_position, 1.0f);

    // perspective divide
    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
        clipPos.z /= clipPos.w;
    }

    // NDC → screen
    const float screenX = (clipPos.x * 0.5f + 0.5f) * width;
    const float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * height; // flip Y
    const float screenZ = clipPos.z;                                   // depth, optional

    model_position = glm::vec3(screenX, screenY, screenZ);
}

void PGraphics::to_world_space(glm::vec3& model_position) const {
    model_position = model_matrix * glm::vec4(model_position, 1.0f);
}

static glm::vec3 world_to_screen(
    const glm::vec3& P, // world-space point
    const glm::mat4& mvp,
    const int        screen_width,
    const int        screen_height) {
    const glm::vec4 clip_space = mvp * glm::vec4(P, 1.0f);

    if (clip_space.w == 0.0f) {
        return glm::vec3(-1.0f); // error
    }

    // Perspective divide to get NDC
    const glm::vec3 ndc = glm::vec3(clip_space) / clip_space.w;

    // NDC to screen
    glm::vec3 screen;
    screen.x = (ndc.x * 0.5f + 0.5f) * screen_width;
    screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screen_height; // flip Y
    screen.z = ndc.z;                                          // optional, -1 (near) to 1 (far)

    return screen;
}

void PGraphics::triangulate_line_strip_vertex(const std::vector<Vertex>& line_strip, const bool close_shape, std::vector<Vertex>& line_vertices) const {
    const glm::vec4        color  = line_strip[0].color;
    const glm::vec4        normal = line_strip[0].normal;
    std::vector<glm::vec2> points(line_strip.size());
    std::vector<glm::vec2> triangles;

    const glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
    for (int i = 0; i < line_strip.size(); ++i) {
        // glm::vec3 _position = line_strip[i].position;
        // to_screen_space(_position);
        points[i] = world_to_screen(line_strip[i].position, mvp, width, height);
    }

    triangulate_line_strip(points,
                           close_shape,
                           stroke_weight,
                           stroke_join_mode,
                           stroke_cap_mode,
                           stroke_join_round_resolution,
                           stroke_cap_round_resolution,
                           stroke_join_miter_max_angle,
                           triangles);
    line_vertices.reserve(triangles.size());
    for (const auto& triangle: triangles) {
        line_vertices.emplace_back(glm::vec3(triangle, 0.0f), color, glm::vec2(0.0f, 0.0f), normal);
    }
}

void PGraphics::normal(const float x, const float y, const float z, const float w) {
    current_normal.x = x;
    current_normal.y = y;
    current_normal.z = z;
    current_normal.w = w;
}

// void PGraphics::beginCamera() {
//     in_camera_block        = true;
//     temp_view_matrix       = view_matrix;
//     temp_projection_matrix = projection_matrix;
// }
//
// void PGraphics::endCamera() {
//     if (in_camera_block) {
//         view_matrix       = temp_view_matrix;
//         projection_matrix = temp_projection_matrix;
//         in_camera_block   = false;
//     }
// }

void PGraphics::camera(const float eyeX, const float eyeY, const float eyeZ,
                       const float centerX, const float centerY, const float centerZ,
                       const float upX, const float upY, const float upZ) {
    const glm::vec3 eye(eyeX, eyeY, eyeZ);
    const glm::vec3 center(centerX, centerY, centerZ);
    const glm::vec3 up(upX, upY, upZ);

    if (in_camera_block) {
        temp_view_matrix = glm::lookAt(eye, center, up);
    } else {
        view_matrix = glm::lookAt(eye, center, up);
    }
}

void PGraphics::camera() {
    // from https://processing.org/reference/camera_.html with FOV 60°
    // camera(width / 2.0, height / 2.0, (height / 2.0) / tan(PI * 30.0 / 180.0),
    //        width / 2.0, height / 2.0, 0,
    //        0, 1, 0);
    constexpr float fov            = DEFAULT_CAMERA_FOV_RADIANS;
    const float     cameraDistance = (height / 2.0f) / tan(fov / 2.0f);
    PGraphics::camera(width / 2.0, height / 2.0, cameraDistance,
                      width / 2.0, height / 2.0, 0,
                      0, 1, 0);
}

void PGraphics::frustum(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    const glm::mat4 proj = glm::frustum(left, right, bottom, top, near, far);
    if (in_camera_block) {
        temp_projection_matrix = proj;
    } else {
        projection_matrix = proj;
    }
}

void PGraphics::ortho(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    const glm::mat4 proj = glm::ortho(left, right, bottom, top, near, far);
    if (in_camera_block) {
        temp_projection_matrix = proj;
    } else {
        projection_matrix = proj;
    }
}

void PGraphics::perspective(const float fovy, const float aspect, const float near, const float far) {
    const glm::mat4 proj = glm::perspective(fovy, aspect, near, far);
    if (in_camera_block) {
        temp_projection_matrix = proj;
    } else {
        projection_matrix = proj;
    }
}

void PGraphics::printCamera() {
    const glm::mat4& mat = in_camera_block ? temp_view_matrix : view_matrix;
    printMatrix(mat);
}

void PGraphics::printProjection() {
    const glm::mat4& mat = in_camera_block ? temp_projection_matrix : projection_matrix;
    printMatrix(mat);
}

/* --- shape --- */

void PGraphics::beginShape(const int shape) {
    shape_fill_vertex_buffer.clear();
    shape_stroke_vertex_buffer.clear();
    shape_mode_cache = shape;
    shape_has_begun  = true;
}


void PGraphics::endShape(const bool close_shape) {
    /*
     * OpenGL ES 3.1 is stricter:
     *
     * 1. No GL_LINES, GL_LINE_STRIP, or GL_LINE_LOOP support in core spec!
     * 2. No glLineWidth support at all.
     * 3. Only GL_TRIANGLES, GL_TRIANGLE_STRIP, and GL_TRIANGLE_FAN are guaranteed.
     *
     * i.e GL_LINES + GL_LINE_STRIP must be emulated
     */
    process_collected_fill_vertices();
    process_collected_stroke_vertices(close_shape);
    shape_fill_vertex_buffer.clear();
    shape_stroke_vertex_buffer.clear();
    shape_has_begun = false;
}

void PGraphics::process_collected_fill_vertices() {
    const int tmp_shape_mode_cache = shape_mode_cache;
    if (!shape_fill_vertex_buffer.empty()) {
        switch (tmp_shape_mode_cache) {
            case POINTS:
            case LINES:
            case LINE_STRIP:
                break;
            case TRIANGLES:
                emit_shape_fill_triangles(shape_fill_vertex_buffer);
                break;
            case TRIANGLE_FAN: {
                std::vector<Vertex> vertices_fill_quads = convertTriangleFanToTriangles(shape_fill_vertex_buffer);
                emit_shape_fill_triangles(vertices_fill_quads);
            } break;
            case QUAD_STRIP: // NOTE does this just work?!?
            case TRIANGLE_STRIP: {
                std::vector<Vertex> vertices_fill_quads = convertTriangleStripToTriangles(shape_fill_vertex_buffer);
                emit_shape_fill_triangles(vertices_fill_quads);
            } break;
            case QUADS: {
                std::vector<Vertex> vertices_fill_quads = convertQuadsToTriangles(shape_fill_vertex_buffer);
                emit_shape_fill_triangles(vertices_fill_quads);
            } break;
            default:
            case POLYGON: {
                std::vector<Vertex> vertices_fill_polygon;
                if (polygon_triangulation_strategy == POLYGON_TRIANGULATION_FASTER) {
                    // EARCUT :: supports concave polygons, textures but no holes or selfintersection
                    vertices_fill_polygon = triangulate_faster(shape_fill_vertex_buffer);
                } else if (polygon_triangulation_strategy == POLYGON_TRIANGULATION_BETTER) {
                    // LIBTESS2 :: supports concave polygons, textures, holes and selfintersection but no textures
                    vertices_fill_polygon = triangulate_good(shape_fill_vertex_buffer);
                } else if (polygon_triangulation_strategy == POLYGON_TRIANGULATION_MID) {
                    // POLYPARTITION + CLIPPER2 // TODO maybe remove this option
                    vertices_fill_polygon = triangulate_better_quality(shape_fill_vertex_buffer);
                }
                emit_shape_fill_triangles(vertices_fill_polygon);
                // TODO what if polygon has only 3 ( triangle ) or 4 vertices ( quad )? could shortcut … here
            } break;
        }
    }
}

void PGraphics::process_collected_stroke_vertices(const bool close_shape) {
    const int tmp_shape_mode_cache = shape_mode_cache;
    if (!shape_stroke_vertex_buffer.empty()) {
        if (tmp_shape_mode_cache == POINTS) {
            // TODO STROKE_RENDER_MODE_NATIVE is handled in renderer
            // TODO decide if point is a fill shape or it s own shape
            // if (point_render_mode == POINT_RENDER_MODE_NATIVE) {
            //     // TODO does this still work under macOS? it renders squares … maybe texturize them
            //     // TODO @OpenGLES3.1 replace with circle or textured quad
            //     const float tmp_point_size = std::max(std::min(point_size, open_gl_capabilities.point_size_max), open_gl_capabilities.point_size_min);
            //     glPointSize(tmp_point_size);
            //     OGL_tranform_model_matrix_and_render_vertex_buffer(IM_primitive_shape, GL_POINTS, shape_stroke_vertex_buffer);
            // }
            if (point_render_mode == POINT_RENDER_MODE_TRIANGULATE) {
                std::vector<Vertex> line_vertices = convertPointsToTriangles(shape_stroke_vertex_buffer, point_size);
                emit_shape_fill_triangles(line_vertices);
            }
            return; // NOTE rendered as points exit early
        }

        switch (tmp_shape_mode_cache) {
            case LINES: {
                const int buffer_size = shape_stroke_vertex_buffer.size() / 2 * 2;
                for (int i = 0; i < buffer_size; i += 2) {
                    std::vector line = {shape_stroke_vertex_buffer[i], shape_stroke_vertex_buffer[i + 1]};
                    emit_shape_stroke_line_strip(line, false);
                }
            } break;
            case TRIANGLE_FAN: {
                const std::vector<Vertex> _triangle_vertices = convertTriangleFanToTriangles(shape_stroke_vertex_buffer);
                const int                 buffer_size        = _triangle_vertices.size() / 3 * 3;
                for (int i = 0; i < buffer_size; i += 3) {
                    std::vector line = {_triangle_vertices[i], _triangle_vertices[i + 1], _triangle_vertices[i + 2]};
                    emit_shape_stroke_line_strip(line, true);
                }
            } break;
            case TRIANGLES: {
                const int buffer_size = shape_stroke_vertex_buffer.size() / 3 * 3;
                for (int i = 0; i < buffer_size; i += 3) {
                    std::vector line = {shape_stroke_vertex_buffer[i], shape_stroke_vertex_buffer[i + 1], shape_stroke_vertex_buffer[i + 2]};
                    emit_shape_stroke_line_strip(line, true);
                }
            } break;
            case TRIANGLE_STRIP: {
                const std::vector<Vertex> _triangle_vertices = convertTriangleStripToTriangles(shape_stroke_vertex_buffer);
                const int                 buffer_size        = _triangle_vertices.size() / 3 * 3;
                for (int i = 0; i < buffer_size; i += 3) {
                    std::vector line = {_triangle_vertices[i], _triangle_vertices[i + 1], _triangle_vertices[i + 2]};
                    emit_shape_stroke_line_strip(line, true);
                }
            } break;
            case QUAD_STRIP: {
                const std::vector<Vertex> _quad_vertices = convertQuadStripToQuads(shape_stroke_vertex_buffer);
                const int                 buffer_size    = _quad_vertices.size() / 4 * 4;
                for (int i = 0; i < buffer_size; i += 4) {
                    std::vector line = {_quad_vertices[i], _quad_vertices[i + 1], _quad_vertices[i + 2], _quad_vertices[i + 3]};
                    emit_shape_stroke_line_strip(line, true);
                }
            } break;
            case LINE_STRIP: {
                emit_shape_stroke_line_strip(shape_stroke_vertex_buffer, false);
            } break;
            case QUADS: {
                const int buffer_size = shape_stroke_vertex_buffer.size() / 4 * 4;
                for (int i = 0; i < buffer_size; i += 4) {
                    std::vector line = {shape_stroke_vertex_buffer[i], shape_stroke_vertex_buffer[i + 1], shape_stroke_vertex_buffer[i + 2], shape_stroke_vertex_buffer[i + 3]};
                    emit_shape_stroke_line_strip(line, true);
                }
            }
            default:
            case POLYGON: {
                emit_shape_stroke_line_strip(shape_stroke_vertex_buffer, close_shape);
            } break;
        }
    }
}

void PGraphics::vertex(const float x, const float y, const float z) {
    vertex(x, y, z, 0, 0);
}

void PGraphics::vertex(const float x, const float y, const float z, const float u, const float v) {
    if (!color_stroke.active && !color_fill.active) {
        return;
    }

    const glm::vec3 position{x, y, z};

    if (color_stroke.active) {
        const glm::vec4 strokeColor = as_vec4(color_stroke);
        shape_stroke_vertex_buffer.emplace_back(position, strokeColor, glm::vec2{u, v}, current_normal);
    }

    if (color_fill.active) {
        const glm::vec4 fillColor = as_vec4(color_fill);
        shape_fill_vertex_buffer.emplace_back(position, fillColor, glm::vec2{u, v}, current_normal);
    }
}