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

static void setup_pre() { printf("Setup Pre\n"); }
static void setup_post() { printf("Setup Post\n"); }
static void shutdown() { printf("Shutdown\n"); }

static bool init() {
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

static void draw_pre() {
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

static void draw_post() {}

static void set_flags(uint32_t& subsystem_flags) {
    subsystem_flags |= SDL_INIT_VIDEO;
}

static PGraphics* create_graphics(const int width, const int height) {
    (void) width;
    (void) height;
    // auto* graphics = new PGraphicsOpenGL2();
    // graphics->init(nullptr, umgebung::width, umgebung::height, 4);
    // return graphics;
    return nullptr;
}
UMGEBUNG_NAMESPACE_END

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_default() {
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
