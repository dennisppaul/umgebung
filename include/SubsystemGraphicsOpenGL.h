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

#include "Umgebung.h"
#include "PGraphicsOpenGL.h"

namespace umgebung {

    void OGL_draw_pre();

    static void center_display(SDL_Window* window) {
        int mDisplayLocation;
        if (display == DEFAULT) {
            mDisplayLocation = SDL_WINDOWPOS_CENTERED;
        } else {
            int mNumDisplays = 0;
            SDL_GetDisplays(&mNumDisplays);
            if (display >= mNumDisplays) {
                error("display index '", display, "' out of range. ",
                      mNumDisplays, " display", mNumDisplays > 1 ? "s are" : " is",
                      " available. using default display.");
                mDisplayLocation = SDL_WINDOWPOS_CENTERED;
            } else {
                mDisplayLocation = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
            }
        }
        SDL_SetWindowPosition(window, mDisplayLocation, mDisplayLocation);
    }

    inline bool OGL_init(SDL_Window*&   window,
                         SDL_GLContext& gl_context,
                         const int      major_version,
                         const int      minor_version,
                         const int      profile) {
        /* setup opengl */

        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // always required on Mac?
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor_version);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        if (antialiasing > 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of available buffers
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
        }

        /* window */

        SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
        window                = SDL_CreateWindow(get_window_title().c_str(),
                                                 static_cast<int>(umgebung::width),
                                                 static_cast<int>(umgebung::height),
                                                 get_SDL_WindowFlags(flags));
        if (window == nullptr) {
            error("Couldn't create window: ", SDL_GetError());
            return false;
        }

        center_display(window);

        /* create opengl context */

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr) {
            error("Couldn't create OpenGL context: ", SDL_GetError());
            SDL_DestroyWindow(window);
            return false;
        }

        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);

        /* display window */

        SDL_ShowWindow(window);

        /* initialize GLEW */

        glewExperimental            = GL_TRUE;
        const GLenum glewInitResult = glewInit();
        if (GLEW_OK != glewInitResult) {
            error("problem initializing GLEW: ", glewGetErrorString(glewInitResult));
            SDL_GL_DestroyContext(gl_context);
            SDL_DestroyWindow(window);
            return false;
        }

        query_opengl_capabilities(open_gl_capabilities);

        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::init(end)");

        return true;
    }

    inline void OGL_setup_pre(SDL_Window*& window) {
        if (window == nullptr) {
            return;
        }

        if (g == nullptr) {
            return;
        }

        int        framebuffer_width;
        int        framebuffer_height;
        const bool success = SDL_GetWindowSizeInPixels(window, &framebuffer_width, &framebuffer_height);
        if (!success) {
            warning("Failed to get window size in pixels.");
        }
        const float pixel_density = SDL_GetWindowPixelDensity(window);

        console("main renderer      : ", g->name());
        console("render to offscreen: ", g->render_to_offscreen ? "true" : "false");
        console("framebuffer size   : ", framebuffer_width, " x ", framebuffer_height);
        console("graphics    size   : ", width, " x ", height);
        console("pixel_density      : ", pixel_density, width != framebuffer_width ? " ( note that if graphics and framebuffer size do not align the pixel density should not be 1 )" : "");
        g->pixelDensity(pixel_density); // NOTE setting pixel density from configuration

        g->init(nullptr, framebuffer_width, framebuffer_height, 0, false);
        g->width  = width;
        g->height = height;
        g->lock_init_properties(true);

        g->set_default_graphics_state();

        OGL_draw_pre();
    }

    inline void OGL_setup_post() {
        // draw_post(); // TODO revive this once it is fully integrated
    }

    inline void OGL_draw_pre() {
        if (g == nullptr) {
            return;
        }
        g->beginDraw();
    }

    inline void OGL_draw_post(SDL_Window*& window, const bool blit_framebuffer_object_to_screenbuffer) {
        if (window == nullptr || g == nullptr) {
            return;
        }

        g->endDraw();

        if (g->render_to_offscreen && g->framebuffer.id > 0) {
            g->render_framebuffer_to_screen(blit_framebuffer_object_to_screenbuffer);
        }
        SDL_GL_SwapWindow(window);
    }

} // namespace umgebung