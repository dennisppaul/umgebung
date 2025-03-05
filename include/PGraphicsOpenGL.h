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

#include <GL/glew.h>

#include "UmgebungFunctionsAdditional.h"

#ifndef PGRAPHICS_OPENGL_DO_NOT_CHECK_ERRORS
#define GL_CALL(func) \
    func;             \
    checkOpenGLError(#func)
#else
#define GL_CALL(func) func;
#endif

namespace umgebung {
    struct OpenGLCapabilities;
    static constexpr GLint UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE    = GL_UNSIGNED_INT_8_8_8_8_REV;
    static constexpr GLint UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT = GL_RGBA;

    inline double depth_range = 10000; // TODO this should be configurable … maybe in `reset_matrices()`

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

    static std::string fl(const std::string& text) {
        constexpr size_t column_width = 40;
        return format_label(text, column_width);
    }

    inline void printOpenGLInfo() {
        const GLubyte* version         = glGetString(GL_VERSION);
        const GLubyte* renderer        = glGetString(GL_RENDERER);
        const GLubyte* vendor          = glGetString(GL_VENDOR);
        const GLubyte* shadingLanguage = glGetString(GL_SHADING_LANGUAGE_VERSION);

        console(fl("OpenGL Version"), version);
        console(fl("Renderer"), renderer);
        console(fl("Vendor"), vendor);
        console(fl("GLSL Version"), shadingLanguage);
    }

    /* opengl capabilities */
    struct OpenGLCapabilities;
    void query_opengl_capabilities(OpenGLCapabilities& capabilities);

    struct OpenGLCapabilities {
        float point_size_min{0};
        float point_size_max{0};
        float point_size_granularity{0};
    };

    inline OpenGLCapabilities open_gl_capabilities;

    inline void query_opengl_capabilities(OpenGLCapabilities& capabilities) {
        console("================================================================================");
        console("OPENGL CAPABILITIES");
        console("================================================================================");

        printOpenGLInfo();

        GLfloat pointSizeRange[2]{};
        glGetFloatv(GL_POINT_SIZE_RANGE, pointSizeRange);
        capabilities.point_size_min = pointSizeRange[0];
        capabilities.point_size_max = pointSizeRange[1];
        console(fl("Min Point Size"), capabilities.point_size_min);
        console(fl("Max Point Size"), capabilities.point_size_max);

        GLfloat pointGranularity{0};
        glGetFloatv(GL_POINT_SIZE_GRANULARITY, &pointGranularity);
        console(fl("Point Size Granularity"), pointGranularity);
        capabilities.point_size_granularity = pointGranularity;

        console("================================================================================");
    }
} // namespace umgebung