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

#include "earcut.hpp"
#include "polypartition.h"
#include "clipper2/clipper.h"

#include "UmgebungFunctionsAdditional.h"
#include "PGraphics.h"
#include "Vertex.h"

#include <UmgebungFunctionsPGraphics.h>
#include <glm/gtc/epsilon.hpp>

using namespace umgebung;

PGraphics::PGraphics() : PImage(0, 0, 0) {
    PGraphics::fill(1.0f);
    PGraphics::stroke(0.0f);
    PGraphics::ellipseDetail(ELLIPSE_DETAIL_DEFAULT);
}

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
        // error("ellipseDetail too small: ", detail, ". must be at least ", ELLIPSE_DETAIL_MIN);
        return;
    }
    ellipse_detail = detail;
    resize_ellipse_points_LUT();
}

void PGraphics::pointSize(const float size) { point_size = size < 1 ? 1 : size; }

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

/* --- triangulation / tesselation --- */

// EARCUT

std::vector<Vertex> PGraphics::triangulate_faster(const std::vector<Vertex>& vertices) {
    std::vector<std::vector<std::array<float, 2>>> polygon;
    polygon.emplace_back(); // Outer boundary

    // convert Vertex to 2D format for earcut (ignore z)
    for (const auto& v: vertices) {
        polygon[0].push_back({v.position.x, v.position.y});
    }

    // perform triangulation
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
    std::vector<Vertex>   triangleList;

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
