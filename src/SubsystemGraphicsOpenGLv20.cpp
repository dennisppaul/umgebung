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

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include "Umgebung.h"
#include "UmgebungFunctionsGraphics.h"
#include "Subsystems.h"
#include "PGraphicsOpenGL.h"
#include "PGraphicsOpenGLv20.h"

namespace umgebung {

    static SDL_Window*   window     = nullptr;
    static SDL_GLContext gl_context = nullptr;

    // TODO implement set_resizable and fullscreen() (at runtime) and is_minimized()
    //    SDL_SetWindowFullscreen(window?!?, fullscreen_state ? SDL_WINDOW_FULLSCREEN: SDL_WINDOW_FULLSCREEN_DESKTOP);
    //    SDL_SetWindowResizable(window?!?, SDL_TRUE);
    //    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {}

    // TODO these functions below should go into a more generic place
    void set_window_position(const int x, const int y) {
        if (window == nullptr) {
            return;
        }
        SDL_SetWindowPosition(window, x, y);
    }

    void get_window_position(int& x, int& y) {
        if (window == nullptr) {
            return;
        }
        SDL_GetWindowPosition(window, &x, &y);
    }

    void set_window_size(const int width, const int height) {
        if (window == nullptr) {
            return;
        }
        SDL_SetWindowSize(window, width, height);
    }

    void get_window_size(int& width, int& height) {
        if (window == nullptr) {
            return;
        }
        SDL_GetWindowSize(window, &width, &height);
    }

    void center_display() {
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

    /* --- private functions --- */

    static void setup_pre();
    static void setup_post();
    static void draw_pre();
    static void draw_post();

    static bool init() { // TODO maybe merge v2.0 & v3.3 they are identical except for SDL_GL_CONTEXT_PROFILE_MASK + SDL_GL_CONTEXT_MAJOR_VERSION + SDL_GL_CONTEXT_MINOR_VERSION
        // NOTE this is identical with the other OpenGL renderer >>>
        /* setup opengl */

        // see https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_opengl3/main.cpp
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        if (antialiasing > 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
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

        center_display();

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
        // SDL_RaiseWindow(window); // TODO see if this causes any issues

        PGraphicsOpenGL::set_default_graphics_state();

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
        // <<< NOTE this is identical with the other OpenGL renderer
    }

    static void setup_pre() {
        // NOTE this is identical with the other OpenGL renderer >>>
        if (g == nullptr) {
            return;
        }

        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_pre(begin)");

        int current_framebuffer_width;
        int current_framebuffer_height;
        SDL_GetWindowSizeInPixels(window, &current_framebuffer_width, &current_framebuffer_height);
        int framebuffer_width         = static_cast<float>(current_framebuffer_width);
        int framebuffer_height        = static_cast<float>(current_framebuffer_height);
        const float pixel_density = SDL_GetWindowPixelDensity(window);

        console("main renderer      : ", g->name());
        console("render to offscreen: ", g->render_to_offscreen ? "true" : "false");
        console("framebuffer size   : ", framebuffer_width, " x ", framebuffer_height);
        console("graphics    size   : ", width, " x ", height);
        console("( note that if these do not align the pixel density might not be 1 )");
        console("pixel_density      : ", pixel_density);
        g->pixelDensity(pixel_density); // NOTE setting pixel density from configuration

        // pixelHeight = static_cast<int>(framebuffer_height / height);
        // pixelWidth  = static_cast<int>(framebuffer_width / width);

        g->init(nullptr, static_cast<int>(framebuffer_width), static_cast<int>(framebuffer_height), 0, false);
        g->width  = static_cast<int>(width);
        g->height = static_cast<int>(height);
        g->lock_init_properties(true);

        PGraphicsOpenGL::set_default_graphics_state();
        draw_pre();
        // <<< NOTE this is identical with the other OpenGL renderer
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_pre(end)");
    }

    static void setup_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_post(begin)");
        draw_post();
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_post(end)");
    }

    static void draw_pre() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_pre(begin)");
        if (g == nullptr) {
            return;
        }

        if (g->render_to_offscreen) {
            g->beginDraw();
        } else {
            PGraphicsOpenGL::set_default_graphics_state();

            glViewport(0, 0, g->framebuffer.width, g->framebuffer.height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, 0, height, -depth_range, depth_range);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glScalef(1, -1, 1);
            glTranslatef(0, -height, 0);
        }
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_pre(end)");
    }

    static void draw_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw");

        if (window == nullptr || g == nullptr) {
            return;
        }

        if (g->render_to_offscreen) {
            g->endDraw();
            g->render_framebuffer_to_screen();
        }

        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_post");
        SDL_GL_SwapWindow(window);
    }

    // static void draw_pre() {
    //     checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_pre(begin)");
    //     if (g == nullptr) {
    //         return;
    //     }
    //
    //     if (g->render_to_offscreen) {
    //         g->beginDraw();
    //     } else {
    //         set_default_graphics_state();
    //
    //         glViewport(0, 0, g->framebuffer.width, g->framebuffer.height);
    //
    //         glMatrixMode(GL_PROJECTION);
    //
    //         // glPushMatrix();
    //         glLoadIdentity();
    //         glOrtho(0, width, 0, height, -depth_range, depth_range);
    //
    //         glMatrixMode(GL_MODELVIEW);
    //         glLoadIdentity();
    //
    //         glScalef(1, -1, 1);
    //         glTranslatef(0, -height, 0);
    //     }
    //     checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_pre(end)");
    // }

    // static void draw_post() {
    //     checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw");
    //
    //     if (window == nullptr) {
    //         return;
    //     }
    //
    //     if (g == nullptr) {
    //         return;
    //     }
    //
    //     if (g->render_to_offscreen) {
    //         g->endDraw();
    //
    //         // Unbind framebuffer
    //         glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //         // Render the FBO to the entire screen
    //         glPushAttrib(GL_ALL_ATTRIB_BITS);
    //
    //         glDisable(GL_DEPTH_TEST);
    //         glDisable(GL_BLEND);
    //         glDisable(GL_ALPHA_TEST);
    //
    //         const float viewport_width  = g->framebuffer.width;
    //         const float viewport_height = g->framebuffer.height;
    //         const float ortho_width     = g->width;
    //         const float ortho_height    = g->height;
    //
    //         glViewport(0, 0, viewport_width, viewport_height);
    //         glMatrixMode(GL_PROJECTION);
    //         glLoadIdentity();
    //         glOrtho(0, ortho_width, 0, ortho_height, -1, 1);
    //         glMatrixMode(GL_MODELVIEW);
    //         glLoadIdentity();
    //         glBindTexture(GL_TEXTURE_2D, g->framebuffer.texture_id);
    //         glEnable(GL_TEXTURE_2D);
    //         glColor4f(1, 1, 1, 1);
    //
    //         glBegin(GL_QUADS);
    //         glTexCoord2f(0.0, 0.0);
    //         glVertex2f(0, 0);
    //         glTexCoord2f(1.0, 0.0);
    //         glVertex2f(static_cast<float>(g->framebuffer.width), 0);
    //         glTexCoord2f(1.0, 1.0);
    //         glVertex2f(static_cast<float>(g->framebuffer.width),
    //                    static_cast<float>(g->framebuffer.height));
    //         glTexCoord2f(0.0, 1.0);
    //         glVertex2f(0, static_cast<float>(g->framebuffer.height));
    //         glEnd();
    //
    //         glDisable(GL_TEXTURE_2D);
    //
    //         glPopAttrib();
    //     }
    //
    //     checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_post");
    //     SDL_GL_SwapWindow(window);
    // }

    static void shutdown() {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
    }

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_VIDEO;
    }

    static PGraphics* create_graphics(const bool render_to_offscreen) {
        return new PGraphicsOpenGLv20(render_to_offscreen);
    }
} // namespace umgebung

umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_openglv20() {
    auto* graphics            = new umgebung::SubsystemGraphics{};
    graphics->init            = umgebung::init;
    graphics->setup_pre       = umgebung::setup_pre;
    graphics->setup_post      = umgebung::setup_post;
    graphics->draw_pre        = umgebung::draw_pre;
    graphics->draw_post       = umgebung::draw_post;
    graphics->shutdown        = umgebung::shutdown;
    graphics->set_flags       = umgebung::set_flags;
    graphics->create_graphics = umgebung::create_graphics;
    return graphics;
}
