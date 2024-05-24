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

#include <iostream>
#include "PShape.h"

using namespace umgebung;

#ifndef DISABLE_GRAPHICS

void PShape::beginShape(int shape) {
    isRecording = true;
    fShape      = shape; // TODO implement
}

void PShape::endShape() {
    isRecording = false;

    //    glGenVertexArrays(1, &vertexArray);
    //    glGenBuffers(1, &vertexBuffer);
    //
    //    glBindVertexArray(vertexArray);
    //    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    //    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    //
    //    // Create and compile the vertex shader
    //    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    //    glCompileShader(vertexShader);
    //
    //    // Create and compile the fragment shader
    //    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    //    glCompileShader(fragmentShader);
    //
    //    // Create shader program
    //    shaderProgram = glCreateProgram();
    //    glAttachShader(shaderProgram, vertexShader);
    //    glAttachShader(shaderProgram, fragmentShader);
    //    glLinkProgram(shaderProgram);
    //    glUseProgram(shaderProgram);
    //
    //    // Specify vertex attributes
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *) 0);
    //    glEnableVertexAttribArray(0);
    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
    //    glEnableVertexAttribArray(1);
    //
    //    glBindVertexArray(0);
    //
    //    fVerticesSize = vertices.size();
    //    vertices.clear(); // TODO can it be cleared here?
}

void PShape::vertex(float x, float y, float z, float r, float g, float b) {
    if (!isRecording) return;
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
}

void PShape::release() {
    //    glDeleteVertexArrays(1, &vertexArray);
    //    glDeleteBuffers(1, &vertexBuffer);
    //    glDeleteProgram(shaderProgram);
    //    glDeleteShader(vertexShader);
    //    glDeleteShader(fragmentShader);
}

void PShape::draw() {
    //    glUseProgram(shaderProgram);
    //    glBindVertexArray(vertexArray);
    //    glDrawArrays(GL_TRIANGLES, 0, fVerticesSize / 6);  // TODO implement different shapes
    //    glBindVertexArray(0);
    int mShape;
    switch (fShape) {
        case TRIANGLES:
            mShape = GL_TRIANGLES;
            break;
        case TRIANGLE_STRIP:
            mShape = GL_TRIANGLE_STRIP;
            break;
        case TRIANGLE_FAN:
            mShape = GL_TRIANGLE_FAN;
            break;
        case QUADS:
            mShape = GL_QUADS;
            break;
        case QUAD_STRIP:
            mShape = GL_QUAD_STRIP;
            break;
        case POLYGON:
            mShape = GL_POLYGON;
            break;
        case POINTS:
            mShape = GL_POINTS;
            break;
        case LINES:
            mShape = GL_LINES;
            break;
        case LINE_STRIP:
            mShape = GL_LINE_STRIP;
            break;
        default:
            mShape = GL_TRIANGLES;
    }
    glBegin(mShape);
    for (int i = 0; i < vertices.size(); i += 6) {
        glColor3f(vertices[i + 3],
                  vertices[i + 4],
                  vertices[i + 5]);
        glVertex3f(vertices[i + 0],
                   vertices[i + 1],
                   vertices[i + 2]);
    }
    glEnd();
}

#else // DISABLE_GRAPHICS

void PShape::beginShape(int shape) {
}

void PShape::endShape() {
}

void PShape::vertex(float x, float y, float z, float r, float g, float b) {
}

void PShape::release() {
}

void PShape::draw() {
}

#endif // DISABLE_GRAPHICS
