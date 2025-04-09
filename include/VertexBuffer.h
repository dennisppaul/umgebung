/*
* Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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
#include <GL/glew.h>

#include "Umfeld.h"
#include "Vertex.h"

namespace umfeld {
    class VertexBuffer {
    public:
        ~VertexBuffer();

        void                 add_vertex(const Vertex& vertex);
        void                 add_vertices(const std::vector<Vertex>& new_vertices);
        void                 draw();
        void                 clear();
        void                 update();
        std::vector<Vertex>& vertices_data() { return _vertices; }
        void                 init();
        void                 set_shape(const int shape) { this->shape = shape; }
        int                  get_shape() const { return shape; }

    private:
        const int           VBO_BUFFER_CHUNK_SIZE_BYTES = 1024 * 16 * sizeof(Vertex);
        std::vector<Vertex> _vertices;
        GLuint              vbo = 0, vao = 0;
        bool                vao_supported      = false;
        bool                initial_upload     = false;
        bool                buffer_initialized = false;
        int                 server_buffer_size{0};
        int                 shape = TRIANGLES;
        bool                dirty = false;

        void resize_buffer();
        void upload();
        void checkVAOSupport();
    };
} // namespace umfeld