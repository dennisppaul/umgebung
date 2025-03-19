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

#include "ShaderSource.h"

namespace umgebung {
    inline ShaderSource shader_source_default{
        .vertex = R"(
            #version 330 core

            layout(location = 0) in vec3 aPosition;
            layout(location = 1) in vec4 aColor;
            layout(location = 2) in vec2 aTexCoord;

            out vec4 vColor;
            out vec2 vTexCoord;

            uniform mat4 uProjection;
            uniform mat4 uViewMatrix;
            uniform mat4 uModelMatrix;

            void main() {
                gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
                vColor = aColor;
                vTexCoord = aTexCoord;
            }
        )",
        .fragment = R"(
            #version 330 core

            in vec4 vColor;
            in vec2 vTexCoord;

            out vec4 FragColor;

            uniform sampler2D uTexture;

            void main() {
                FragColor = texture(uTexture, vTexCoord) * vColor;
            }
        )",
        .geometry = ""
    };
}
