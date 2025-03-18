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

// ReSharper disable CppCStyleCast
#include <glm/glm.hpp>
#include <iostream>

#include "PMesh.h"

using namespace umgebung;

void PMesh::init() {
    checkVAOSupport();
    glGenBuffers(1, &vbo);
    if (vao_supported) {
        glGenVertexArrays(1, &vao);
    }
    buffer_initialized = true;
}

PMesh::~PMesh() {
    glDeleteBuffers(1, &vbo);
    if (vao_supported) {
        glDeleteVertexArrays(1, &vao);
    }
}

void PMesh::checkVAOSupport() {
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if ((major > 3) || (major == 3 && minor >= 3)) {
        vao_supported = true; // OpenGL 3.3+ supports VAOs
    } else {
        // For OpenGL 2.0, check if GL_ARB_vertex_array_object is available
        const char* extensions = (const char*) glGetString(GL_EXTENSIONS);
        if (extensions && strstr(extensions, "GL_ARB_vertex_array_object")) {
            vao_supported = true;
        }
    }
}

void PMesh::add_vertex(const Vertex& vertex) {
    _vertices.push_back(vertex);
}

void PMesh::add_vertices(const std::vector<Vertex>& new_vertices) {
    _vertices.insert(_vertices.end(), new_vertices.begin(), new_vertices.end());
}

void PMesh::clear() {
    _vertices.clear();
}

void PMesh::resize_buffer() {
    const size_t client_buffer_size_bytes = _vertices.size() * sizeof(Vertex);
    const size_t server_buffer_size_bytes = server_buffer_size * sizeof(Vertex);

    if (client_buffer_size_bytes > server_buffer_size_bytes) {
        const size_t growSize = client_buffer_size_bytes + VBO_BUFFER_CHUNK_SIZE_BYTES;
        // std::cout << "Growing vertex buffer from " << server_buffer_size_bytes << " to " << growSize << " bytes" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, vbo); // Ensure VBO is bound
        glBufferData(GL_ARRAY_BUFFER, growSize, _vertices.data(), GL_DYNAMIC_DRAW);
        server_buffer_size = growSize / sizeof(Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if (client_buffer_size_bytes < server_buffer_size_bytes) {
        const size_t shrinkSize = client_buffer_size_bytes;
        // std::cout << "Shrinking vertex buffer to " << client_buffer_size_bytes << " bytes" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, vbo); // Ensure VBO is bound
        glBufferData(GL_ARRAY_BUFFER, shrinkSize, _vertices.data(), GL_DYNAMIC_DRAW);
        server_buffer_size = shrinkSize / sizeof(Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void PMesh::upload() {
    if (_vertices.empty()) {
        return;
    }

    if (vao_supported) {
        glBindVertexArray(vao);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_DYNAMIC_DRAW);

    // Define vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tex_coord));

    if (vao_supported) {
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    server_buffer_size = _vertices.size();
}

void PMesh::draw(const int mode) {
    if (!buffer_initialized) { init(); }

    if (_vertices.empty()) {
        return;
    }

    if (vao_supported) {
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, _vertices.size());
        glBindVertexArray(0);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tex_coord));

        glDrawArrays(mode, 0, _vertices.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void PMesh::update() {
    if (!buffer_initialized) { init(); }

    if (_vertices.empty()) {
        return;
    }

    if (!initial_upload) {
        initial_upload = true;
        upload();
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (_vertices.size() > server_buffer_size ||
        _vertices.size() + VBO_BUFFER_CHUNK_SIZE_BYTES / sizeof(Vertex) < server_buffer_size) {
        resize_buffer();
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(Vertex), _vertices.data());
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
