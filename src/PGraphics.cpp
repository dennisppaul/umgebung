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

using namespace umgebung;

PGraphics::PGraphics() : PImage(0, 0, 0) {
    PGraphics::fill(1.0f);
    PGraphics::stroke(0.0f);
    PGraphics::ellipseDetail(ELLIPSE_DETAIL_DEFAULT);
    generate_box(box_vertices_LUT);
    generate_sphere(sphere_vertices_LUT);
}

// NOTE: done
void PGraphics::background(PImage* img) {
    background(0, 0, 0, 0);
    fill(1);
    image(img, 0, 0, framebuffer.width, framebuffer.height);
}

// NOTE: done
void PGraphics::background(const float a) {
    background(a, a, a);
}

// NOTE: done
void PGraphics::background(const float a, const float b, const float c, const float d) {
    IMPL_background(a, b, c, d);
}

/* --- transform matrices --- */

// NOTE: done
void PGraphics::popMatrix() {
    if (!model_matrix_stack.empty()) {
        model_matrix_client = model_matrix_stack.back();
        model_matrix_stack.pop_back();
    }
}

// NOTE: done
void PGraphics::pushMatrix() {
    model_matrix_stack.push_back(model_matrix_client);
}

void PGraphics::resetMatrix() {
    model_matrix_client = glm::mat4(1.0f);
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
    printMatrix(model_matrix_client);
}

// NOTE: done
void PGraphics::translate(const float x, const float y, const float z) {
    model_matrix_client = glm::translate(model_matrix_client, glm::vec3(x, y, z));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::rotateX(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::rotateY(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::rotateZ(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::rotate(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::rotate(const float angle, const float x, const float y, const float z) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(x, y, z));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::scale(const float x) {
    model_matrix_client = glm::scale(model_matrix_client, glm::vec3(x, x, x));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::scale(const float x, const float y) {
    model_matrix_client = glm::scale(model_matrix_client, glm::vec3(x, y, 1));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphics::scale(const float x, const float y, const float z) {
    model_matrix_client = glm::scale(model_matrix_client, glm::vec3(x, y, z));
    model_matrix_dirty  = true;
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

// NOTE: done
void PGraphics::strokeWeight(const float weight) {
    stroke_weight = weight;
}

// NOTE: done
/**
 *  can be MITER, BEVEL, ROUND, NONE, BEVEL_FAST or MITER_FAST
 * @param join
 */
void PGraphics::strokeJoin(const int join) {
    stroke_join_mode = join;
}

// NOTE: done
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

/* --- shapes --- */


// NOTE: done
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

// NOTE: done
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

// NOTE: done
void PGraphics::bezierDetail(const int detail) {
    bezier_detail = detail;
}

// NOTE: done
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

// NOTE: done
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

// NOTE: done
void PGraphics::image(PImage* img, const float x, const float y) {
    image(img, x, y, img->width, img->height);
}

// NOTE: done
void PGraphics::circle(const float x, const float y, const float diameter) {
    ellipse(x, y, diameter, diameter);
}

// NOTE: done
PImage* PGraphics::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}

// NOTE: done
PFont* PGraphics::loadFont(const std::string& file, const float size) {
    auto* font = new PFont(file, size); // TODO what about pixel_density … see FTGL implementation
    return font;
}

// NOTE: done
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

void PGraphics::text_str(const std::string& text, const float x, const float y, const float z) {
    if (current_font == nullptr) {
        return;
    }
    if (!color_fill.active) {
        return;
    }

    current_font->draw(this, text, x, y, z);
}

// NOTE: done
void PGraphics::texture(PImage* img) {
    IMPL_set_texture(img);
}

// NOTE: done
void PGraphics::point(const float x, const float y, const float z) {
    beginShape(POINTS);
    vertex(x, y, z);
    endShape();
}

// NOTE: done
void PGraphics::pointSize(const float size) {
    point_size = size;
}

// NOTE: done
void PGraphics::line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
    if (!color_stroke.active) {
        return;
    }
    beginShape(LINES);
    vertex(x1, y1, z1);
    vertex(x2, y2, z2);
    endShape();
}

// NOTE: done
void PGraphics::line(const float x1, const float y1, const float x2, const float y2) {
    line(x1, y1, 0, x2, y2, 0);
}

// NOTE: done
void PGraphics::triangle(const float x1, const float y1, const float z1,
                         const float x2, const float y2, const float z2,
                         const float x3, const float y3, const float z3) {
    beginShape(TRIANGLES);
    vertex(x1, y1, z1);
    vertex(x2, y2, z2);
    vertex(x3, y3, z3);
    endShape();
}

// NOTE: done
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

    // define colors once (avoiding redundant glm::vec4 conversions)
    const glm::vec4 fill_color   = as_vec4(color_fill);
    const glm::vec4 stroke_color = as_vec4(color_stroke);

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

void PGraphics::generate_box(std::vector<glm::vec3>& vertices) {
    // Define 8 corner points of a unit cube (centered at origin)
    glm::vec3 p0(-0.5f, -0.5f, -0.5f); // Bottom-left-back
    glm::vec3 p1(0.5f, -0.5f, -0.5f);  // Bottom-right-back
    glm::vec3 p2(0.5f, 0.5f, -0.5f);   // Top-right-back
    glm::vec3 p3(-0.5f, 0.5f, -0.5f);  // Top-left-back
    glm::vec3 p4(-0.5f, -0.5f, 0.5f);  // Bottom-left-front
    glm::vec3 p5(0.5f, -0.5f, 0.5f);   // Bottom-right-front
    glm::vec3 p6(0.5f, 0.5f, 0.5f);    // Top-right-front
    glm::vec3 p7(-0.5f, 0.5f, 0.5f);   // Top-left-front

    // Define triangles for each of the 6 faces (2 triangles per face)
    std::vector<glm::vec3> triangles = {
        // Back Face (-Z)
        p0, p1, p2, p2, p3, p0,
        // Front Face (+Z)
        p5, p4, p7, p7, p6, p5,
        // Left Face (-X)
        p4, p0, p3, p3, p7, p4,
        // Right Face (+X)
        p1, p5, p6, p6, p2, p1,
        // Bottom Face (-Y)
        p4, p5, p1, p1, p0, p4,
        // Top Face (+Y)
        p3, p2, p6, p6, p7, p3};

    // Convert to Vertex format
    for (const auto& pos: triangles) {
        vertices.push_back({pos});
    }
}

void PGraphics::generate_sphere(std::vector<glm::vec3>& vertices, const int stacks, const int slices, const float radius) {
    // Loop through latitude (stacks)
    for (int i = 0; i < stacks; ++i) {
        const float theta1 = glm::pi<float>() * (static_cast<float>(i) / stacks); // From 0 to PI
        const float theta2 = glm::pi<float>() * (static_cast<float>(i + 1) / stacks);

        // Loop through longitude (slices)
        for (int j = 0; j < slices; ++j) {
            const float phi1 = 2.0f * glm::pi<float>() * (static_cast<float>(j) / slices); // From 0 to 2PI
            const float phi2 = 2.0f * glm::pi<float>() * (static_cast<float>(j + 1) / slices);

            // Convert spherical coordinates to Cartesian (x, y, z)
            auto p0 = glm::vec3(
                radius * sin(theta1) * cos(phi1),
                radius * cos(theta1),
                radius * sin(theta1) * sin(phi1));

            auto p1 = glm::vec3(
                radius * sin(theta2) * cos(phi1),
                radius * cos(theta2),
                radius * sin(theta2) * sin(phi1));

            auto p2 = glm::vec3(
                radius * sin(theta2) * cos(phi2),
                radius * cos(theta2),
                radius * sin(theta2) * sin(phi2));

            auto p3 = glm::vec3(
                radius * sin(theta1) * cos(phi2),
                radius * cos(theta1),
                radius * sin(theta1) * sin(phi2));

            // Two triangles per quad
            vertices.push_back({p0});
            vertices.push_back({p1});
            vertices.push_back({p2});

            vertices.push_back({p2});
            vertices.push_back({p3});
            vertices.push_back({p0});
        }
    }
}

/* --- triangulation --- */

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

const Triangulator PGraphics::triangulator;

std::vector<Vertex> PGraphics::triangulate_good(const std::vector<Vertex>& vertices) {
    const std::vector<Vertex> triangles = triangulator.triangulate(vertices, Triangulator::Winding::WINDING_ODD);
    return triangles;
}

std::vector<Vertex> PGraphics::triangulate_better_quality(const std::vector<Vertex>& vertices) {
    glm::vec4 first_color = vertices[0].color;

    TPPLPartition partitioner;

    // Step 1: Use Clipper2 to resolve self-intersections
    const Clipper2Lib::PathsD inputPaths = {convertToClipperPath(vertices)};
    const Clipper2Lib::PathsD fixedPaths = Clipper2Lib::Union(inputPaths, Clipper2Lib::FillRule::NonZero);

    if (fixedPaths.empty()) {
        std::cerr << "Clipper2 failed to fix the polygon!" << std::endl;
        return {};
    }

    // Step 2: Convert to PolyPartition format
    std::vector<TPPLPoly> convexPolygons;
    for (auto& poly: convertToPolyPartition(fixedPaths)) {
        std::list<TPPLPoly> convexParts;
        if (!partitioner.ConvexPartition_HM(&poly, &convexParts)) {
            std::cerr << "Convex partitioning failed!" << std::endl;
            continue;
        }
        convexPolygons.insert(convexPolygons.end(), convexParts.begin(), convexParts.end());
    }

    // Step 3: Triangulate each convex part
    std::vector<Vertex> triangleList;
    for (auto& part: convexPolygons) {
        std::list<TPPLPoly> triangles;
        if (!partitioner.Triangulate_EC(&part, &triangles)) {
            std::cerr << "Triangulation failed for a convex part!" << std::endl;
            continue;
        }

        // Extract triangle vertices
        for (const auto& tri: triangles) {
            for (int i = 0; i < 3; ++i) {
                triangleList.push_back({static_cast<float>(tri[i].x), static_cast<float>(tri[i].y), 0.0f,
                                        first_color.r, first_color.g, first_color.b, first_color.a,
                                        0, 0});
            }
        }
    }
    return triangleList;
}

void PGraphics::reset_matrices() {
    // TODO clean up framebuffer_width and height handling … also see Umgebung for this
    framebuffer_width  = framebuffer.width;
    framebuffer_height = framebuffer.height;

    model_matrix_client = glm::mat4(1.0f);
    model_matrix_dirty  = false;

    // orthographic projection
    projection_matrix_2D = glm::ortho(0.0f, static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height), 0.0f);

    const float fov            = DEFAULT_FOV;
    const float cameraDistance = (height / 2.0f) / tan(fov / 2.0f);

    // perspective projection
    projection_matrix_3D = glm::perspective(fov, width / height, 0.1f, static_cast<float>(depth_range));

    view_matrix = glm::lookAt(glm::vec3(width / 2.0f, height / 2.0f, -cameraDistance),
                              glm::vec3(width / 2.0f, height / 2.0f, 0.0f),
                              glm::vec3(0.0f, -1.0f, 0.0f));
}

void PGraphics::to_screen_space(glm::vec3& world_position) const {
    // Transform world position to camera (view) space
    const glm::vec4 viewPos = view_matrix * model_matrix_client * glm::vec4(world_position, 1.0f);
    // const glm::vec4 viewPos = view_matrix * model_matrix_client * glm::vec4(world_position, 1.0f);

    // Project onto clip space
    glm::vec4 clipPos = projection_matrix_3D * viewPos;

    // Perspective divide (convert to normalized device coordinates)
    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
    }

    // Now the coordinates are in NDC (-1 to 1 range)
    // Convert NDC to screen space (assuming viewport width and height)
    // TODO what is it? `width` or `framebuffer_width`
    // float screenX = (clipPos.x * 0.5f + 0.5f) * static_cast<float>(framebuffer_width);
    // float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * static_cast<float>(framebuffer_height);
    const float screenX = (clipPos.x * 0.5f + 0.5f) * static_cast<float>(width);
    const float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * static_cast<float>(height);

    world_position.x = screenX;
    world_position.y = screenY;
    world_position.z = 0.0f;
}

void PGraphics::to_world_space(glm::vec3& model_position) const {
    model_position = model_matrix_client * glm::vec4(model_position, 1.0f);
}
