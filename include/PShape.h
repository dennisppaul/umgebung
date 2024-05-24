/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2023 Dennis P Paul.
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

// TODO implement properly with VBOs + *modern* OpenGL

#include <vector>

#ifndef DISABLE_GRAPHICS

#include <GL/glew.h>

#endif

#include "UmgebungConstants.h"

namespace umgebung {

    class PShape {
    public:
        void beginShape(int shape = POLYGON);

        void endShape();

        void vertex(float x, float y, float z, float r, float g, float b);

        void release();

        void draw();

    private:
        bool isRecording   = false;
        int  fShape        = POLYGON;
        int  fVerticesSize = 0;
#ifndef DISABLE_GRAPHICS
        std::vector<GLfloat> vertices;
        GLuint               vertexArray, vertexBuffer;
        GLuint               vertexShader, fragmentShader, shaderProgram;

        const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;
    out vec3 color;
    void main() {
        gl_Position = vec4(aPos, 1.0);
        color = aColor;
    }
)";

        const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 color;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(color, 1.0);
    }
)";
#endif // DISABLE_GRAPHICS
    };

    PShape* createShape();

} // namespace umgebung