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

#include <glm/glm.hpp>

namespace umgebung {
    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 tex_coord;

        Vertex(const float x, const float y, const float z = 0,
               const float r = 1, const float g = 1, const float b = 1, const float a = 1,
               const float u = 0, const float v = 0)
            : position(x, y, z), color(r, g, b, a), tex_coord(u, v) {}

        explicit Vertex(const glm::vec3& position,
                        const glm::vec4& color     = glm::vec4(1, 1, 1, 1),
                        const glm::vec2& tex_coord = glm::vec2(0, 0))
            : position(position), color(color), tex_coord(tex_coord) {}

        explicit Vertex(const glm::vec2& position)
            : Vertex(glm::vec3(position, 0)) {}

        Vertex() : Vertex(glm::vec3(0, 0, 0)) {}
    };

    // struct VertexNormal : Vertex {
    //     glm::vec3 normal;
    //
    //     explicit VertexNormal(const glm::vec3& position,
    //                           const glm::vec4& color     = glm::vec4(1, 1, 1, 1),
    //                           const glm::vec2& tex_coord = glm::vec2(0, 0),
    //                           const glm::vec3& normal    = glm::vec3(0, 0, 1))
    //         : Vertex(position, color, tex_coord), normal(normal) {}
    // };
} // namespace umgebung