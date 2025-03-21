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
#include <glm/gtx/type_aligned.hpp>

namespace umgebung {
    struct Vertex {
        static constexpr auto DEFAULT_POSITION  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        static constexpr auto DEFAULT_NORMAL    = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        static constexpr auto DEFAULT_COLOR     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        static constexpr auto DEFAULT_TEX_COORD = glm::vec2(0.0f, 0.0f);
        glm::aligned_vec4     position;
        glm::aligned_vec4     normal;
        glm::aligned_vec4     color;
        glm::vec2             tex_coord;
        glm::vec2             padding = glm::vec2(0.0f);

        // NOTE aligned_vec4 makes sure all data types are aligned to 16 bytes ( GLM_ENABLE_EXPERIMENTAL )
        //      make sure this does not cause any issues … it s experimental after all

        Vertex(const float x, const float y, const float z = DEFAULT_POSITION.z,
               const float r = DEFAULT_COLOR.r, const float g = DEFAULT_COLOR.g, const float b = DEFAULT_COLOR.b, const float a = DEFAULT_COLOR.a,
               const float u = DEFAULT_TEX_COORD.x, const float v = DEFAULT_TEX_COORD.y)
            : position(x, y, z, DEFAULT_POSITION.w),
              normal(DEFAULT_NORMAL),
              color(r, g, b, a),
              tex_coord(u, v) {}

        explicit Vertex(const glm::vec3& position,
                        const glm::vec4& color     = DEFAULT_COLOR,
                        const glm::vec2& tex_coord = DEFAULT_TEX_COORD,
                        const glm::vec4& normal    = DEFAULT_NORMAL)
            : position(position, DEFAULT_POSITION.w),
              normal(normal),
              color(color),
              tex_coord(tex_coord) {}

        explicit Vertex(const glm::vec2& position)
            : Vertex(glm::vec3(position, DEFAULT_POSITION.z)) {}

        Vertex()
            : Vertex(glm::vec3(DEFAULT_POSITION)) {}
    };
} // namespace umgebung