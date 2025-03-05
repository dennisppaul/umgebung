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

#include "UmgebungFunctionsAdditional.h"

#ifndef PGRAPHICS_OPENGL_DO_NOT_CHECK_ERRORS
#define GL_CALL(func) \
    func;             \
    checkOpenGLError(#func)
#else
#define GL_CALL(func) func;
#endif

namespace umgebung {
    inline std::string getOpenGLErrorString(const GLenum error) {
        switch (error) {
            case GL_NO_ERROR: return "No error";
            case GL_INVALID_ENUM: return "Invalid enum (GL_INVALID_ENUM)";
            case GL_INVALID_VALUE: return "Invalid value (GL_INVALID_VALUE)";
            case GL_INVALID_OPERATION: return "Invalid operation (GL_INVALID_OPERATION)";
            case GL_STACK_OVERFLOW: return "Stack overflow (GL_STACK_OVERFLOW)";
            case GL_STACK_UNDERFLOW: return "Stack underflow (GL_STACK_UNDERFLOW)";
            case GL_OUT_OF_MEMORY: return "Out of memory (GL_OUT_OF_MEMORY)";
            case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation (GL_INVALID_FRAMEBUFFER_OPERATION)";
            default: return "Unknown OpenGL error";
        }
    }

    inline void checkOpenGLError(const std::string& functionName) {
#ifndef PGRAPHICS_OPENGL_DO_NOT_CHECK_ERRORS
        GLenum opengl_error;
        while ((opengl_error = glGetError()) != GL_NO_ERROR) {
            warning("[OpenGL Error] @", functionName, ": ", getOpenGLErrorString(opengl_error));
        }
#endif
    }
} // namespace umgebung