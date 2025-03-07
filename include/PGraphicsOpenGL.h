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
#include "PGraphics.h"

#ifndef PGRAPHICS_OPENGL_DO_NOT_CHECK_ERRORS
#define GL_CALL(func) \
    func;             \
    checkOpenGLError(#func)
#else
#define GL_CALL(func) func;
#endif

namespace umgebung {
    class PGraphicsOpenGL : public PGraphics {
    public:
        ~PGraphicsOpenGL() override = default;

        /* --- interface --- */

        // TODO implement 3D version
        // virtual void    line(float x1, float y1, float z1, float x2, float y2, float z2)    = 0;

        void    strokeWeight(float weight) override                                                                         = 0;
        void    background(float a, float b, float c, float d = 1.0f) override                                              = 0;
        void    background(float a) override                                                                                = 0;
        void    rect(float x, float y, float width, float height) override                                                  = 0;
        void    ellipse(float x, float y, float width, float height) override                                               = 0;
        void    circle(float x, float y, float radius) override                                                             = 0;
        void    line(float x1, float y1, float x2, float y2) override                                                       = 0;
        void    line(float x1, float y1, float z1, float x2, float y2, float z2) override                                   = 0;
        void    triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) override = 0;
        void    bezier(float x1, float y1, float x2, float y2,
                       float x3, float y3, float x4, float y4) override                                                     = 0;
        void    bezier(float x1, float y1, float z1, float x2, float y2, float z2,
                       float x3, float y3, float z3, float x4, float y4, float z4) override                                 = 0;
        void    bezierDetail(int detail) override                                                                           = 0;
        void    pointSize(float size) override                                                                              = 0;
        void    point(float x, float y, float z = 0.0f) override                                                            = 0;
        void    beginShape(int shape = POLYGON) override                                                                    = 0;
        void    endShape(bool close_shape = false) override                                                                 = 0;
        void    vertex(float x, float y, float z = 0.0f) override                                                           = 0;
        void    vertex(float x, float y, float z, float u, float v) override                                                = 0;
        PFont*  loadFont(const std::string& file, float size) override                                                      = 0;
        void    textFont(PFont* font) override                                                                              = 0;
        void    textSize(float size) override                                                                               = 0;
        void    text(const char* value, float x, float y, float z = 0.0f) override                                          = 0;
        float   textWidth(const std::string& text) override                                                                 = 0;
        PImage* loadImage(const std::string& filename) override                                                             = 0;
        void    image(PImage* img, float x, float y, float w, float h) override                                             = 0;
        void    image(PImage* img, float x, float y) override                                                               = 0;
        void    texture(PImage* img) override                                                                               = 0;
        void    pixelDensity(int density) override                                                                          = 0;
        void    hint(uint16_t property) override                                                                            = 0;
        void    beginDraw() override                                                                                        = 0;
        void    endDraw() override                                                                                          = 0;
        void    text_str(const std::string& text, float x, float y, float z = 0.0f) override                                = 0; // TODO maybe make this private?

        void init(uint32_t* pixels, int width, int height, int format, bool generate_mipmap) override = 0;

        /* --- additional methods --- */

        void        upload_texture(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override = 0;
        void        download_texture(PImage* img) override                                                                                              = 0;
        std::string name() override                                                                                                                     = 0;
    };

    // TODO @maybe move the functions below into the class above

    struct OpenGLCapabilities;
    static constexpr GLint UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE    = GL_UNSIGNED_INT_8_8_8_8_REV;
    static constexpr GLint UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT = GL_RGBA;
    static constexpr int   OPENGL_PROFILE_NONE                    = -1;
    static constexpr int   OPENGL_PROFILE_CORE                    = 1;
    static constexpr int   OPENGL_PROFILE_COMPATIBILITY           = 2;

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

    static std::string separator(const bool equal_sign = true) {
        if (equal_sign) {
            return "================================================================================";
        } else {
            return "--------------------------------------------------------------------------------";
        }
    }

    static void get_OpenGL_version(int& major, int& minor) {
        const auto versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        if (!versionStr) {
            major = 0;
            minor = 0;
            return;
        }

        sscanf(versionStr, "%d.%d", &major, &minor);
    }

    /* opengl capabilities */

    struct OpenGLCapabilities {
        int   version_major{0};
        int   version_minor{0};
        int   profile{0};
        float line_size_min{0};
        float line_size_max{0};
        float line_size_granularity{0};
        float point_size_min{0};
        float point_size_max{0};
        float point_size_granularity{0};
    };

    inline OpenGLCapabilities open_gl_capabilities;

    void query_opengl_capabilities(OpenGLCapabilities& capabilities);

    inline void printOpenGLInfo(OpenGLCapabilities& capabilities) {
        const GLubyte* version         = glGetString(GL_VERSION);
        const GLubyte* renderer        = glGetString(GL_RENDERER);
        const GLubyte* vendor          = glGetString(GL_VENDOR);
        const GLubyte* shadingLanguage = glGetString(GL_SHADING_LANGUAGE_VERSION);
        get_OpenGL_version(capabilities.version_major, capabilities.version_minor);

        console(fl("OpenGL Version"), version, " (", capabilities.version_major, ".", capabilities.version_minor, ")");
        console(fl("Renderer"), renderer);
        console(fl("Vendor"), vendor);
        console(fl("GLSL Version"), shadingLanguage);

        std::string profile_str = "none ( pre 3.2 )";
        capabilities.profile    = OPENGL_PROFILE_NONE;
        if (capabilities.version_major > 2) {
            int profile = 0;
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
            if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
                profile_str = "core";
                console("OpenGL Core Profile detected. Deprecated functions are not available.");
                capabilities.profile = OPENGL_PROFILE_CORE;
            }
            if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
                profile_str          = "compatibility ( legacy functions available )";
                capabilities.profile = OPENGL_PROFILE_COMPATIBILITY;
            }
        }
        console(fl("Profile"), profile_str);
    }

    inline void query_opengl_capabilities(OpenGLCapabilities& capabilities) {
        console(separator());
        console("OPENGL CAPABILITIES");
        console(separator());

        printOpenGLInfo(capabilities);

        console(separator(false));

        GLfloat line_size_range[2]{};
        glGetFloatv(GL_LINE_WIDTH_RANGE, line_size_range);
        capabilities.line_size_min = line_size_range[0];
        capabilities.line_size_max = line_size_range[1];
        console(fl("line size min"), capabilities.line_size_min);
        console(fl("line size max"), capabilities.line_size_max);

        if (capabilities.line_size_min == 1.0f && capabilities.line_size_max == 1.0f) {
            console(fl("line support"), "since min and max line size is 1.0");
            console(fl(""), "it is likely that lines are not");
            console(fl(""), "properly supported.");
        }

        GLfloat line_size_granularity{0};
        glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &line_size_granularity);
        console(fl("point size granularity"), line_size_granularity);
        capabilities.line_size_granularity = line_size_granularity;

        GLfloat point_size_range[2]{};
        glGetFloatv(GL_POINT_SIZE_RANGE, point_size_range);
        capabilities.point_size_min = point_size_range[0];
        capabilities.point_size_max = point_size_range[1];
        console(fl("point size min"), capabilities.point_size_min);
        console(fl("point size max"), capabilities.point_size_max);

        GLfloat point_size_granularity{0};
        glGetFloatv(GL_POINT_SIZE_GRANULARITY, &point_size_granularity);
        console(fl("point size granularity"), point_size_granularity);
        capabilities.point_size_granularity = point_size_granularity;

        console(separator());
    }
} // namespace umgebung