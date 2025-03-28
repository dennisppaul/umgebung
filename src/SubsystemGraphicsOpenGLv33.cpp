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
#include "PGraphicsOpenGL.h"
#include "PGraphicsOpenGLv33.h"

namespace umgebung {
    static void draw_pre();
    static void draw_post();

    static SDL_Window*   window                                  = nullptr;
    static SDL_GLContext gl_context                              = nullptr;
    static bool          blit_framebuffer_object_to_screenbuffer = true;
    // NOTE FBO is BLITted directly into the color buffer instead of rendered with a textured quad

    static bool init() {
        return OGL_init(window, gl_context, 3, 3, SDL_GL_CONTEXT_PROFILE_CORE);
    }

    static void setup_pre() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::setup_pre(begin)");
        OGL_setup_pre(window);
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::setup_pre(end)");
    }

    static void setup_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::setup_post(begin)");
        OGL_setup_post();
        OGL_draw_post(window, blit_framebuffer_object_to_screenbuffer); // TODO maybe move this to shared methods once it is fully integrated
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::setup_post(end)");
    }

    static void draw_pre() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::draw_pre(begin)");
        OGL_draw_pre();
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::draw_pre(end)");
    }

    static void draw_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::draw_post(begin)");
        OGL_draw_post(window, blit_framebuffer_object_to_screenbuffer);
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL33::draw_post(end)");
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
            warning("TODO implement resize in OGLv33");
        }
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event_loop(SDL_Event* event) {
        if (event->type == SDL_EVENT_WINDOW_RESIZED) {
            warning("TODO implement resize in OGLv33");
        }
    }

    static const char* name() {
        return "OpenGL 3.3 core";
    }

    static PGraphics* create_main_graphics(const bool render_to_offscreen) {
        return new PGraphicsOpenGLv33(render_to_offscreen);
    }
} // namespace umgebung

umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_openglv33() {
    auto* graphics                 = new umgebung::SubsystemGraphics{};
    graphics->set_flags            = umgebung::set_flags;
    graphics->init                 = umgebung::init;
    graphics->setup_pre            = umgebung::setup_pre;
    graphics->setup_post           = umgebung::setup_post;
    graphics->draw_pre             = umgebung::draw_pre;
    graphics->draw_post            = umgebung::draw_post;
    graphics->shutdown             = umgebung::shutdown;
    graphics->event                = umgebung::event;
    graphics->event_loop           = umgebung::event_loop;
    graphics->name                 = umgebung::name;
    graphics->create_main_graphics = umgebung::create_main_graphics;
    return graphics;
}
