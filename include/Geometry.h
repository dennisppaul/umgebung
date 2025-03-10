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

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UmgebungConstants.h"

// #define GEOMETRY_DRAW_DEBUG

namespace umgebung {

    struct Segment {
        glm::vec2 position;
        glm::vec2 normal;
        glm::vec2 direction;
        glm::vec2 next_position;
    };

    inline void triangulate_polygon(const std::vector<glm::vec2>& polygon_outline,
                                    std::vector<glm::vec2>&       resulting_triangles) {
        TESStesselator* tess = tessNewTess(nullptr);
        tessAddContour(tess, 2, polygon_outline.data(), sizeof(glm::vec2), polygon_outline.size());
        if (tessTesselate(tess, TESS_WINDING_NONZERO, TESS_POLYGONS, 3, 2, nullptr)) {
            const float*     verts  = tessGetVertices(tess);
            const TESSindex* elems  = tessGetElements(tess);
            const int        nelems = tessGetElementCount(tess);
            resulting_triangles.reserve(nelems * 3);
            for (int i = 0; i < nelems; ++i) {
                const TESSindex* tri = &elems[i * 3];
                resulting_triangles.insert(resulting_triangles.end(), {glm::make_vec2(&verts[tri[0] * 2]),
                                                                       glm::make_vec2(&verts[tri[1] * 2]),
                                                                       glm::make_vec2(&verts[tri[2] * 2])});
            }
        }
        tessDeleteTess(tess);
    }

    inline float angle_between_vectors(const glm::vec2& d1, const glm::vec2& d2) {
        const float determinant = d1.x * d2.y - d1.y * d2.x;
        const float angle       = std::atan2(determinant, glm::dot(d1, d2));
        return angle;
    }

    inline bool are_almost_parallel(const glm::vec3& n1, const glm::vec3& n2, const float epsilon = 0.01f) {
        const float dotProduct = glm::dot(n1, n2);
        return -dotProduct > (1.0f - epsilon); // Closer to 1 or -1 means nearly parallel
        // return fabs(dotProduct) > (1.0f - epsilon); // Closer to 1 or -1 means nearly parallel
    }

    inline bool intersect_lines(const glm::vec2& p1, const glm::vec2& d1,
                                const glm::vec2& p2, const glm::vec2& d2,
                                glm::vec2& intersection) {
        const float det = d1.x * d2.y - d1.y * d2.x;

        if (fabs(det) < 1e-6f) {
            return false; // Parallel or coincident lines
        }

        const float t = ((p2.x - p1.x) * d2.y - (p2.y - p1.y) * d2.x) / det;

        intersection = glm::vec2(p1 + t * d1);
        return true;
    }

    inline void add_cap(const Segment&          cap_seg,
                        const float             half_width,
                        const int               stroke_cap_mode,
                        const float             direction,
                        const float             stroke_cap_round_resolution,
                        std::vector<glm::vec2>& poly_list) {
        /* POINTED */
        if (stroke_cap_mode == POINTED) {
            const glm::vec2 cap_point_project = cap_seg.position - cap_seg.direction * half_width * direction;
            poly_list.push_back(cap_point_project);
#ifdef GEOMETRY_DRAW_DEBUG
            fill(0, 0.5f, 1);
            circle(cap_point_project.x, cap_point_project.y, 10);
#endif // GEOMETRY_DRAW_DEBUG
        }
        /* PROJECT */
        if (stroke_cap_mode == PROJECT) {
            const glm::vec2 cap_point_project = cap_seg.position - cap_seg.direction * half_width * direction;
            const glm::vec2 cap_norm          = cap_seg.normal * half_width;
            poly_list.push_back(cap_point_project + cap_norm);
            poly_list.push_back(cap_point_project - cap_norm);
        }
        /* ROUNG */
        if (stroke_cap_mode == ROUND) {
            const float angle       = atan2(cap_seg.normal.y, cap_seg.normal.x) - 3.0f * HALF_PI * direction;
            const float angle_start = angle - HALF_PI;
            const float angle_end   = angle + HALF_PI;
            const float step        = direction * stroke_cap_round_resolution;
            const bool  forward     = direction == 1.0f;
            for (float r = (forward ? angle_start : angle_end); forward ? r <= angle_end : r >= angle_start; r += step) {
                glm::vec2 circle_segment = cap_seg.position + glm::vec2{cos(r), sin(r)} * half_width;
                poly_list.push_back(circle_segment);
#ifdef GEOMETRY_DRAW_DEBUG
                fill(0, 0.5f, 1);
                circle(circle_segment.x, circle_segment.y, 3);
#endif // GEOMETRY_DRAW_DEBUG
            }
        }
        /* SQUARE */
        // NOTE nothing to do here
    }

    inline void create_stroke_join_tessellate(std::vector<glm::vec2>&     triangles,
                                              const bool                  close_shape,
                                              const float                 half_width,
                                              const int                   stroke_join_mode,
                                              const int                   stroke_cap_mode,
                                              const float                 stroke_join_round_resolution,
                                              const float                 stroke_cap_round_resolution,
                                              const float                 stroke_join_miter_max_angle,
                                              const std::vector<Segment>& segments) {
        std::vector<glm::vec2> outline_left;
        std::vector<glm::vec2> outline_right;
        outline_left.reserve(segments.size());
        outline_right.reserve(segments.size());

        /* CAP START */
        if (!close_shape) {
            constexpr int           seg_index = 0;
            constexpr float         direction = 1.0f;
            std::vector<glm::vec2>& poly_list = outline_left;
            const Segment           cap_seg   = segments[seg_index];
            add_cap(cap_seg, half_width, stroke_cap_mode, direction, stroke_cap_round_resolution, poly_list);
        }

        const int num_segments = segments.size() + (close_shape ? +1 : 0);
        for (int i = 0; i < num_segments; ++i) {
            const Segment& s1 = segments[(i) % segments.size()];
            const Segment& s2 = segments[(i + 1) % segments.size()];
#ifdef GEOMETRY_DRAW_DEBUG
            stroke(0);
            fill(0);
            circle(s1.position.x, s1.position.y, 10);
            noFill();
            circle(s2.position.x, s2.position.y, 15);
#endif // GEOMETRY_DRAW_DEBUG
            const glm::vec2 s1_norm  = s1.normal * half_width;
            const glm::vec2 s1_left  = s1.position - s1_norm;
            const glm::vec2 s1_right = s1.position + s1_norm;
            const glm::vec2 s2_left  = s2.position - s1_norm;
            const glm::vec2 s2_right = s2.position + s1_norm;
            const glm::vec2 s2_norm  = s2.normal * half_width;
#ifdef GEOMETRY_DRAW_DEBUG
            fill(1, 0, 0);
            circle(s1_left.x, s1_left.y, 10 + i * 2);
            fill(0, 1, 0);
            circle(s1_right.x, s1_right.y, 10 + i * 2);

            noFill();
            stroke(1, 0, 0);
            circle(s2_left.x, s2_left.y, 10 + i * 2);
            stroke(0, 1, 0);
            circle(s2_right.x, s2_right.y, 10 + i * 2);
#endif // GEOMETRY_DRAW_DEBUG
            if (close_shape) {
                outline_left.push_back(s1_left);
                outline_left.push_back(s2_left);
                outline_right.push_back(s1_right);
                outline_right.push_back(s2_right);
            } else {
                outline_left.push_back(s1_left);
                outline_right.push_back(s1_right);
                if (i < num_segments - 1) { // works for round & bevel but not vor miter
                    outline_left.push_back(s2_left);
                    outline_right.push_back(s2_right);
                }
            }

            /* ROUND */

            if (stroke_join_mode == ROUND) {
                // skip last one ( creates a cap )
                if (i != num_segments - 1) {
                    float angle_s1 = atan2(s1.normal.y, s1.normal.x);
                    float angle_s2 = atan2(s2.normal.y, s2.normal.x);
                    // compute the signed angular difference
                    float angular_diff = angle_s2 - angle_s1;
                    while (angular_diff <= -PI) {
                        angular_diff += TWO_PI;
                    }
                    while (angular_diff > PI) {
                        angular_diff -= TWO_PI;
                    }
                    float r_offset = (angular_diff > 0) ? PI : 0.0f;
                    // flip the start/end if segment goes in the wrong direction
                    if (angular_diff < 0) {
                        std::swap(angle_s1, angle_s2);
                        angular_diff = -angular_diff;
                    }
                    for (float r = angle_s1; r <= angle_s1 + angular_diff; r += stroke_join_round_resolution) {
                        glm::vec2 circle_segment = s2.position + glm::vec2{
                                                                     cos(r + r_offset),
                                                                     sin(r + r_offset)} *
                                                                     half_width;
                        outline_left.push_back(circle_segment);
                        // TODO check if it would be better to add this to right side depending on angle
                    }
                }
            }

            /* BEVEL */

            // NOTE no need to do anything special

            /* MITER */

            if (stroke_join_mode == MITER) {
                if (close_shape || i < num_segments - 1) {
                    const float angle     = glm::degrees(angle_between_vectors(s1.direction, s2.direction));
                    const float abs_angle = fabs(angle);
                    if (abs_angle < stroke_join_miter_max_angle && angle > 0) {
                        const glm::vec2 s2_proj_left = s2.position - s2_norm;
                        glm::vec2       intersection_left;
                        const bool      result_left = intersect_lines(s1_left, s1.direction,
                                                                      s2_proj_left, s2.direction,
                                                                      intersection_left);
                        if (result_left) {
                            outline_left.push_back(intersection_left);
                        }
                    }
                    if (abs_angle < stroke_join_miter_max_angle && angle < 0) {
                        const glm::vec2 p2_proj_right = s2.position + s2_norm;
                        glm::vec2       intersection_right;
                        const bool      result_right = intersect_lines(s1_right, s1.direction,
                                                                       p2_proj_right, s2.direction,
                                                                       intersection_right);
                        if (result_right) {
                            outline_right.push_back(intersection_right);
                        }
                    }
                }
            }
        }

        /* CAP END */
        if (!close_shape) {
            const int               seg_index = segments.size() - 1;
            constexpr float         direction = -1.0f;
            std::vector<glm::vec2>& poly_list = outline_right;
            const Segment           cap_seg   = segments[seg_index];
            add_cap(cap_seg, half_width, stroke_cap_mode, direction, stroke_cap_round_resolution, poly_list);
        }

        std::vector<glm::vec2> polygon_outline;
        polygon_outline.insert(polygon_outline.end(), outline_left.begin(), outline_left.end());
        polygon_outline.insert(polygon_outline.end(), outline_right.rbegin(), outline_right.rend());
        triangulate_polygon(polygon_outline, triangles);
    }

    inline void add_quad_as_triangles(std::vector<glm::vec2>& triangles,
                                      const glm::vec2&        p1,
                                      const glm::vec2&        p2,
                                      const glm::vec2&        p3,
                                      const glm::vec2&        p4) {
        triangles.emplace_back(p1);
        triangles.emplace_back(p2);
        triangles.emplace_back(p4);
        triangles.emplace_back(p4);
        triangles.emplace_back(p2);
        triangles.emplace_back(p3);
    }

    inline void create_stroke_join_bevel(std::vector<glm::vec2>& triangles,
                                         const bool              close_shape,
                                         const float             half_width,
                                         std::vector<Segment>&   segments) {
        glm::vec2 p_prev_left{};
        glm::vec2 p_prev_right{};
        const int num_segments = segments.size() - (close_shape ? 0 : 1);
        for (int i = 0; i < num_segments; ++i) {
            Segment&        s1           = segments[i];
            const glm::vec2 p1_line_norm = s1.normal * half_width;
            const glm::vec2 p1_left      = s1.position + p1_line_norm;
            const glm::vec2 p1_right     = s1.position - p1_line_norm;
            const glm::vec2 p2_left      = s1.next_position + p1_line_norm;
            const glm::vec2 p2_right     = s1.next_position - p1_line_norm;
            add_quad_as_triangles(triangles,
                                  p1_left,
                                  p2_left,
                                  p2_right,
                                  p1_right);
            if (i != 0) {
                Segment&    s2    = segments[i - 1];
                const float angle = glm::degrees(angle_between_vectors(s1.direction, s2.direction));
                if (angle > 0) {
                    triangles.emplace_back(s1.position);
                    triangles.emplace_back(p_prev_left);
                    triangles.emplace_back(p1_left);
                } else {
                    triangles.emplace_back(s1.position);
                    triangles.emplace_back(p_prev_right);
                    triangles.emplace_back(p1_right);
                }
            }
            p_prev_left  = p2_left;
            p_prev_right = p2_right;
        }
        if (close_shape) {
            Segment&        s1           = segments[0];
            Segment&        s2           = segments[segments.size() - 1];
            const float     angle        = glm::degrees(angle_between_vectors(s1.direction, s2.direction));
            const glm::vec2 p1_line_norm = s1.normal * half_width;
            const glm::vec2 p1_left      = s1.position + p1_line_norm;
            const glm::vec2 p1_right     = s1.position - p1_line_norm;
            if (angle > 0) {
                triangles.emplace_back(s1.position);
                triangles.emplace_back(p_prev_left);
                triangles.emplace_back(p1_left);
            } else {
                triangles.emplace_back(s1.position);
                triangles.emplace_back(p_prev_right);
                triangles.emplace_back(p1_right);
            }
        }
    }

    inline void create_stroke_join_none(std::vector<glm::vec2>& triangles,
                                        const bool              close_shape,
                                        const float             half_width,
                                        std::vector<Segment>&   segments) {
        const int num_segments = segments.size() - (close_shape ? 0 : 1);
        for (int i = 0; i < num_segments; ++i) {
            Segment&        s1           = segments[i];
            const glm::vec2 p1_line_norm = s1.normal * half_width;
            const glm::vec2 p1_left      = s1.position + p1_line_norm;
            const glm::vec2 p1_right     = s1.position - p1_line_norm;
            const glm::vec2 p2_left      = s1.next_position + p1_line_norm;
            const glm::vec2 p2_right     = s1.next_position - p1_line_norm;
            add_quad_as_triangles(triangles,
                                  p1_left,
                                  p2_left,
                                  p2_right,
                                  p1_right);
        }
    }

    inline void create_stroke_join_miter(std::vector<glm::vec2>& triangles,
                                         const bool              close_shape,
                                         const float             half_width,
                                         const float             stroke_join_miter_max_angle,
                                         std::vector<Segment>&   segments) {
        glm::vec2 p_prev_left{};
        glm::vec2 p_prev_right{};

        const int num_segments = segments.size() + (close_shape ? 1 : -1);
        for (int i = 0; i < num_segments; ++i) {
            const int      j  = i % segments.size();
            const int      jj = (i + 1) % segments.size();
            const Segment& s1 = segments[j];
            const Segment& s2 = segments[jj];

            const glm::vec2 p1_line_norm = s1.normal * half_width;
            const glm::vec2 p1_left      = s1.position + p1_line_norm;
            const glm::vec2 p1_right     = s1.position - p1_line_norm;

            const glm::vec2 p2_line_norm = s2.normal * half_width;
            const glm::vec2 p2_left      = s2.position + p2_line_norm;
            const glm::vec2 p2_right     = s2.position - p2_line_norm;

            glm::vec2  intersection_left;
            const bool result_left = intersect_lines(p1_left, s1.direction,
                                                     p2_left, s2.direction,
                                                     intersection_left);
            glm::vec2  intersection_right;
            const bool result_right = intersect_lines(p1_right, s1.direction,
                                                      p2_right, s2.direction,
                                                      intersection_right);

            if (!result_left) {
                intersection_left = s2.position + p1_line_norm;
            }

            if (!result_right) {
                intersection_right = s2.position - p1_line_norm;
            }

            bool        flip_intersection = false;
            const float angle             = glm::degrees(angle_between_vectors(s1.direction, s2.direction));
            if (fabs(angle) > stroke_join_miter_max_angle) {
                intersection_left  = s1.next_position + p1_line_norm;
                intersection_right = s1.next_position - p1_line_norm;
                flip_intersection  = true;
            }

            if (i != 0) {
                add_quad_as_triangles(triangles,
                                      p_prev_left,
                                      intersection_left,
                                      intersection_right,
                                      p_prev_right);
            } else {
                if (!close_shape) {
                    add_quad_as_triangles(triangles,
                                          p1_left,
                                          intersection_left,
                                          intersection_right,
                                          p1_right);
                }
            }

            if (flip_intersection) {
                p_prev_left  = intersection_right;
                p_prev_right = intersection_left;
            } else {
                p_prev_left  = intersection_left;
                p_prev_right = intersection_right;
            }
        }
    }

    inline void line_strip(const std::vector<glm::vec2>& polygon_outline,
                           const bool                    close_shape,
                           const float                   stroke_weight,
                           const int                     stroke_join_mode,
                           const int                     stroke_cap_mode,
                           const float                   stroke_join_round_resolution,
                           const float                   stroke_cap_round_resolution,
                           const float                   stroke_join_miter_max_angle,
                           std::vector<glm::vec2>&       triangle_vertices) {
        const float half_width = stroke_weight * 0.5f;

        // TODO handle this case i.e LINES
        // if (points.size() < 2) {
        //     return triangles;
        // }

        // TODO does this still work?
        // if (points.size() < 3) {
        //     return triangles;
        // }

        std::vector<Segment> segments(polygon_outline.size());
        for (int i = 0; i < polygon_outline.size(); ++i) {
            Segment s;
            s.position = glm::vec2(polygon_outline[i].x, polygon_outline[i].y);
            if (i == polygon_outline.size() - 1) { // last point needs special care
                if (close_shape) {                 // copy first point for closed shapes
                    s.next_position = glm::vec2(polygon_outline[0].x, polygon_outline[0].y);
                } else { // project point last for open shapes
                    s.next_position = polygon_outline[i] + (polygon_outline[i] - polygon_outline[i - 1]);
                }
            } else { // for all other segments use next point
                const int ii    = (i + 1) % polygon_outline.size();
                s.next_position = glm::vec2(polygon_outline[ii].x, polygon_outline[ii].y);
            }
            s.direction = s.next_position - s.position;
            // s.normal    = glm::normalize(s.direction);
            // s.normal    = glm::vec2(-s.normal.y, s.normal.x);
            if (glm::length(s.direction) > 0.0001f) {
                s.direction = glm::normalize(s.direction);
                s.normal    = glm::vec2(-s.direction.y, s.direction.x);
            } else {
                s.normal = glm::vec2(1.0f, 0.0f);
            }
            segments[i] = s;
        }

        switch (stroke_join_mode) {
            case NONE:
                create_stroke_join_none(triangle_vertices, close_shape, half_width, segments);
                break;
            case MITER_FAST:
                create_stroke_join_miter(triangle_vertices, close_shape, half_width, stroke_join_miter_max_angle, segments);
                break;
            case BEVEL_FAST:
                create_stroke_join_bevel(triangle_vertices, close_shape, half_width, segments);
                break;
            case BEVEL:
            case MITER:
            case ROUND:
            default:
                create_stroke_join_tessellate(triangle_vertices,
                                              close_shape,
                                              half_width,
                                              stroke_join_mode,
                                              stroke_cap_mode,
                                              stroke_join_round_resolution,
                                              stroke_cap_round_resolution,
                                              stroke_join_miter_max_angle,
                                              segments);
        }
    }
} // namespace umgebung