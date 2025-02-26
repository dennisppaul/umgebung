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

#include <SDL3/SDL.h>

#include "UmgebungConstants.h"
#include "UmgebungPGraphicsInterface.h"
#include "UmgebungSubsystems.h"

UMGEBUNG_NAMESPACE_BEGIN
// TODO Ref https://github.com/libsdl-org/SDL/blob/main/docs/hello.c

static SDL_Window*   window   = nullptr;
static SDL_Renderer* renderer = nullptr;

bool subsystem_graphics_default_init(int width, int height);
void subsystem_graphics_default_setup_pre() { printf("Setup Pre\n"); }
void subsystem_graphics_default_setup_post() { printf("Setup Post\n"); }
void subsystem_graphics_default_draw_pre();
void subsystem_graphics_default_draw_post();
void subsystem_graphics_default_shutdown() { printf("Shutdown\n"); }

bool subsystem_graphics_default_init() {
    SDL_Log("Graphics Init: %d x %d\n", static_cast<int>(width), static_cast<int>(height));
    if (!SDL_CreateWindowAndRenderer("Hello World",
                                     static_cast<int>(umgebung::width),
                                     static_cast<int>(umgebung::height),
                                     0,
                                     // SDL_WINDOW_FULLSCREEN,
                                     &window,
                                     &renderer)) {
        return false;
    }
    return true;
}

void subsystem_graphics_default_draw_pre() {
    const char*     message = "Hello World!";
    int             w = 0, h = 0;
    constexpr float scale = 2.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    const float x = ((static_cast<float>(w) / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    const float y = ((static_cast<float>(h) / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);
    SDL_RenderPresent(renderer);
}

void subsystem_graphics_default_draw_post() {}

void subsystem_graphics_default_set_flags(uint32_t& subsystem_flags) {
    subsystem_flags |= SDL_INIT_VIDEO;
}

PGraphics* subsystem_graphics_default_create_graphics() {
    // auto* graphics = new PGraphicsOpenGL2();
    // graphics->init(nullptr, umgebung::width, umgebung::height, 4);
    // return graphics;
    return nullptr;
}
UMGEBUNG_NAMESPACE_END

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_default() {
    auto* graphics            = new umgebung::SubsystemGraphics{};
    graphics->init            = umgebung::subsystem_graphics_default_init;
    graphics->setup_pre       = umgebung::subsystem_graphics_default_setup_pre;
    graphics->setup_post      = umgebung::subsystem_graphics_default_setup_post;
    graphics->draw_pre        = umgebung::subsystem_graphics_default_draw_pre;
    graphics->draw_post       = umgebung::subsystem_graphics_default_draw_post;
    graphics->shutdown        = umgebung::subsystem_graphics_default_shutdown;
    graphics->set_flags       = umgebung::subsystem_graphics_default_set_flags;
    graphics->create_graphics = umgebung::subsystem_graphics_default_create_graphics;
    return graphics;
}
