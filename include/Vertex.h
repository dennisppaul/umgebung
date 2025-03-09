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
        Vertex(const float x, const float y, const float z,
               const float r, const float g, const float b, const float a,
               const float u, const float v)
            : position(x, y, z),
              color(r, g, b, a),
              tex_coord(u, v) {}
        Vertex(const glm::vec3& position,
               const glm::vec4& color,
               const glm::vec2& tex_coord)
            : position(position),
              color(color),
              tex_coord(tex_coord) {}
        Vertex() : position(), color(), tex_coord() {}
    };
} // namespace umgebung