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
    inline ShaderSource shader_source_basic{
        R"(
            #version 330 core

            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aColor;

            out vec3 ourColor;

            void main() {
                gl_Position = vec4(aPos, 1.0);
                ourColor = aColor;
            }
        )",
        R"(
            #version 330 core

            out vec4 FragColor;

            in vec3 ourColor;

            void main() {
                FragColor = vec4(ourColor, 1.0);
            }
        )",
        ""};
}
