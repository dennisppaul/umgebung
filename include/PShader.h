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

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace umgebung {
    class PShader {
    public:
        PShader();
        ~PShader();

        bool        load(const std::string& vertex_code, const std::string& fragment_code, const std::string& geometry_code = "");
        void        use() const;
        static void unuse();
        GLuint      getProgramID() const { return programID; }

        // Uniform setters
        void set_uniform(const std::string& name, int value);
        void set_uniform(const std::string& name, float value);
        void set_uniform(const std::string& name, const glm::vec2& value);
        void set_uniform(const std::string& name, const glm::vec3& value);
        void set_uniform(const std::string& name, const glm::vec4& value);
        void set_uniform(const std::string& name, const glm::mat4& value);

    private:
        GLuint                                 programID;
        std::unordered_map<std::string, GLint> uniformLocations;

        static GLuint compileShader(const std::string& source, GLenum type);
        static void   checkCompileErrors(GLuint shader, GLenum type);
        static void   checkLinkErrors(GLuint program);
        GLint         getUniformLocation(const std::string& name);
    };
} // namespace umgebung
