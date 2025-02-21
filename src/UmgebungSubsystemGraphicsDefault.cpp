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

#include "UmgebungPGraphicsInterface.h"
#include "UmgebungSubsystemGraphicsDefault.h"

// TODO Ref https://github.com/libsdl-org/SDL/blob/main/docs/hello.c

static SDL_Window*   window   = nullptr;
static SDL_Renderer* renderer = nullptr;

bool umgebung_subsystem_graphics_default_init(const int width, const int height) {
    SDL_Log("Graphics Init: %d x %d\n", width, height);
    if (!SDL_CreateWindowAndRenderer("Hello World",
                                     umgebung::width,
                                     umgebung::height,
                                     0,
                                     // SDL_WINDOW_FULLSCREEN,
                                     &window,
                                     &renderer)) {
        return false;
    }
    return true;
}

void umgebung_subsystem_graphics_default_draw_pre() {
    const char* message = "Hello World!";
    int         w = 0, h = 0;
    float       x, y;
    const float scale = 2.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);
    SDL_RenderPresent(renderer);
}

void umgebung_subsystem_graphics_draw_default_post() {}

umgebung::PGraphics* create_graphics() {
    auto* graphics = new umgebung::PGraphics();
    graphics->init(nullptr, umgebung::width, umgebung::height, 4);
    return graphics;
}