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

#include "SubsystemGraphicsOpenGL.h"
#include "PGraphicsOpenGLv20.h"

namespace umgebung {
    static SDL_Window*   window     = nullptr;
    static SDL_GLContext gl_context = nullptr;

    static void draw_pre();
    static void draw_post();

    static bool init() {
        return OGL_init(window, gl_context, 2, 0, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    }

    static void setup_pre() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::setup_pre(begin)");
        OGL_setup_pre(window);
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::setup_pre(end)");
    }

    static void setup_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::setup_post(begin)");
        OGL_setup_post();
        OGL_draw_post(window, false); // TODO maybe move this to shared methods once it is fully integrated
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::setup_post(end)");
    }

    static void draw_pre() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::draw_pre(begin)");
        OGL_draw_pre();
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::draw_pre(end)");
    }

    static void draw_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::draw_post(begin)");
        OGL_draw_post(window, false);
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL20::draw_post(end)");
    }

    static void shutdown() {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
    }

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_VIDEO;
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event(SDL_Event* event) {
        if (event->type == SDL_EVENT_WINDOW_RESIZED) {
            warning("TODO implement resize in OGLv20");
        }
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event_in_update_loop(SDL_Event* event) {
        if (event->type == SDL_EVENT_WINDOW_RESIZED) {
            warning("TODO implement resize in OGLv20");
        }
    }

    static PGraphics* create_native_graphics(const bool render_to_offscreen) {
        return new PGraphicsOpenGLv20(render_to_offscreen);
    }

    static SDL_Window* get_sdl_window() {
        return window;
    }

    static void* get_renderer() {
        return gl_context;
    }

    static int get_renderer_type() {
        return OPENGL_2_0;
    }

    static const char* name() {
        return "OpenGL 2.0";
    }
} // namespace umgebung

umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_openglv20() {
    auto* graphics                   = new umgebung::SubsystemGraphics{};
    graphics->set_flags              = umgebung::set_flags;
    graphics->init                   = umgebung::init;
    graphics->setup_pre              = umgebung::setup_pre;
    graphics->setup_post             = umgebung::setup_post;
    graphics->draw_pre               = umgebung::draw_pre;
    graphics->draw_post              = umgebung::draw_post;
    graphics->shutdown               = umgebung::shutdown;
    graphics->event                  = umgebung::event;
    graphics->event_in_update_loop   = umgebung::event_in_update_loop;
    graphics->create_native_graphics = umgebung::create_native_graphics;
    graphics->get_sdl_window         = umgebung::get_sdl_window;
    graphics->get_renderer           = umgebung::get_renderer;
    graphics->get_renderer_type      = umgebung::get_renderer_type;
    graphics->name                   = umgebung::name;
    return graphics;
}

namespace umgebung {
    // TODO implement set_resizable and fullscreen() (at runtime) and is_minimized()
    //    SDL_SetWindowFullscreen(window?!?, fullscreen_state ? SDL_WINDOW_FULLSCREEN: SDL_WINDOW_FULLSCREEN_DESKTOP);
    //    SDL_SetWindowResizable(window?!?, SDL_TRUE);
    //    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {}

    // TODO these are currently in Umgebung.cpp
    // std::string get_window_title() {
    //     // TODO move this to subsystem and make it configurable
    //     return UMGEBUNG_WINDOW_TITLE;
    // }
    //
    // void set_window_title(std::string title) {
    //     // TODO move this to subsystem
    // }

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
} // namespace umgebung