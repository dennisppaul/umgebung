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

#include "Umgebung.h"
#include "PGraphicsDefault2D.h"

namespace umgebung {
    // TODO Ref https://github.com/libsdl-org/SDL/blob/main/docs/hello.c

    static SDL_Window*   window       = nullptr;
    static SDL_Renderer* sdl_renderer = nullptr;

    static void setup_pre() {
        int w = 0, h = 0;
        SDL_GetRenderOutputSize(sdl_renderer, &w, &h);
        umgebung::g->init(nullptr, w, h, 4, false);
    }

    static void setup_post() { printf("Setup Post\n"); }

    static void shutdown() { printf("Shutdown\n"); }

    static bool init() {
        SDL_WindowFlags flags = 0;
        if (!SDL_CreateWindowAndRenderer(get_window_title().c_str(),
                                         static_cast<int>(umgebung::width),
                                         static_cast<int>(umgebung::height),
                                         get_SDL_WindowFlags(flags),
                                         &window,
                                         &sdl_renderer)) {
            return false;
        }

        std::cout << "Renderer Name    : " << SDL_GetRendererName(sdl_renderer) << std::endl;
        std::cout << "Renderer Property: " << SDL_GetRendererProperties(sdl_renderer) << std::endl;

        SDL_ShowWindow(window);
        return true;
    }

    static void draw_pre() {
        SDL_SetRenderScale(sdl_renderer, 1, 1);
    }

    static void draw_post() {
        SDL_RenderPresent(sdl_renderer);
    }

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_VIDEO;
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event(SDL_Event* event) {
        if (event->type == SDL_EVENT_WINDOW_RESIZED) {
            warning("TODO implement resize in SDL_2D");
        }
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event_loop(SDL_Event* event) {
        if (event->type == SDL_EVENT_WINDOW_RESIZED) {
            warning("TODO implement resize in SDL_2D");
        }
    }

    static PGraphics* create_main_graphics(const bool render_to_offscreen) {
        (void) render_to_offscreen;
        return new PGraphicsDefault2D(sdl_renderer);
    }

    static SDL_Window* get_sdl_window() {
        return window;
    }

    static void* get_renderer() {
        return sdl_renderer;
    }

    static int get_renderer_type() {
        return SDL_2D;
    }

    static const char* name() {
        return "SDL 2D";
    }
} // namespace umgebung

umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_sdl2d() {
    auto* graphics       = new umgebung::SubsystemGraphics{};
    graphics->set_flags  = umgebung::set_flags;
    graphics->init       = umgebung::init;
    graphics->setup_pre  = umgebung::setup_pre;
    graphics->setup_post = umgebung::setup_post;
    graphics->draw_pre   = umgebung::draw_pre;
    graphics->draw_post  = umgebung::draw_post;
    // graphics->event                = umgebung::event;
    // graphics->event_loop           = umgebung::event_loop;
    graphics->shutdown             = umgebung::shutdown;
    graphics->create_main_graphics = umgebung::create_main_graphics;
    graphics->get_sdl_window       = umgebung::get_sdl_window;
    graphics->get_renderer         = umgebung::get_renderer;
    graphics->get_renderer_type    = umgebung::get_renderer_type;
    graphics->name                 = umgebung::name;
    return graphics;
}
