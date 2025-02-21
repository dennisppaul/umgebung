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
#include <SDL3/SDL_main.h>

#include <iostream>
#include <sstream>
#include <string>

#include "Umgebung.h"

namespace umgebung {
    static bool initialized = false;

    bool is_initialized() {
        return initialized;
    }

    std::string get_window_title() {
        return UMGEBUNG_WINDOW_TITLE;
    }

} // namespace umgebung

// TODO move the functions to the respective subsystems
void umgebung_subsystem_audio_init();
void umgebung_subsystem_audio_poll_devices();
void umgebung_subsystem_events_init();

// TODO add console ( e.g SDL_log )
// TODO not in umgebung namespace?
static void handle_arguments(int argc, char* argv[]) {
    std::vector<std::string> args;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            args.emplace_back(argv[i]);
        }
    }
    arguments(args);
}

static uint32_t compile_subsystems_flag() {
    /* from `SDL.h`:
     * - `SDL_INIT_TIMER`: timer subsystem
     * - `SDL_INIT_AUDIO`: audio subsystem
     * - `SDL_INIT_VIDEO`: video subsystem; automatically initializes the events
     *   subsystem
     * - `SDL_INIT_JOYSTICK`: joystick subsystem; automatically initializes the
     *   events subsystem
     * - `SDL_INIT_HAPTIC`: haptic (force feedback) subsystem
     * - `SDL_INIT_GAMECONTROLLER`: controller subsystem; automatically
     *   initializes the joystick subsystem
     * - `SDL_INIT_EVENTS`: events subsystem
     * - `SDL_INIT_EVERYTHING`: all of the above subsystems
     * - `SDL_INIT_NOPARACHUTE`: compatibility; this flag is ignored
     */
    uint32_t subsystem_flags = 0;
    subsystem_flags |= SDL_INIT_AUDIO; // TODO move this to device?
    subsystem_flags |= SDL_INIT_EVENTS;
    return subsystem_flags;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

    /* check graphics subsystem */

    if (umgebung::subsystem_graphics == nullptr) {
        if (umgebung::create_subsystem_graphics != nullptr) {
            umgebung::subsystem_graphics = umgebung::create_subsystem_graphics();
        } else {
            umgebung::console("No graphics subsystem provided, using default.");
            // umgebung::subsystem_graphics = umgebung_subsystem_graphics_create_default();
            umgebung::subsystem_graphics = umgebung_subsystem_graphics_create_opengl2();
        }
        if (umgebung::subsystem_graphics == nullptr) {
            umgebung::console("Couldn't create graphics subsystem.");
            return SDL_APP_FAILURE;
        }
    }

    /* check graphics subsystem */
    if (umgebung::subsystem_audio == nullptr) {
        if (umgebung::create_subsystem_audio != nullptr) {
            umgebung::subsystem_audio = umgebung::create_subsystem_audio();
        } else {
            umgebung::console("No audio subsystem provided, using default.");
            // umgebung::subsystem_audio = umgebung_create_audio_subsystem_default();
        }
    }

    /*
     * 1. prepare umgebung application ( e.g args, settings )
     * 2. initialize SDL
     * 3. initialize graphics
     * 4. initialize audio
     * 5. setup application
     */

    /* 1. prepare umgebung application */

    handle_arguments(argc, argv);
    settings();

    /* 2. initialize SDL */

    uint32_t subsystem_flags = compile_subsystems_flag();
    if (umgebung::subsystem_graphics->set_flags != nullptr) {
        umgebung::subsystem_graphics->set_flags(subsystem_flags);
    }

    if (!SDL_Init(subsystem_flags)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetAppMetadata("Umgebung Application", "1.0", "de.dennisppaul.umgebung.application");

    /* 3. initialize graphics */
    if (umgebung::subsystem_graphics->init != nullptr) {
        if (!umgebung::subsystem_graphics->init(umgebung::width, umgebung::height)) {
            SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
    }

    if (umgebung::subsystem_graphics->create_graphics != nullptr) {
        umgebung::g = umgebung::subsystem_graphics->create_graphics();
    }

    umgebung::initialized = true;

    if (umgebung::subsystem_graphics->setup_pre != nullptr) {
        umgebung::subsystem_graphics->setup_pre();
    }

    setup();

    if (umgebung::subsystem_graphics->setup_post != nullptr) {
        umgebung::subsystem_graphics->setup_post();
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    /*
     * 1. mouse events
     * 2. key events
     * 3. quit events
     */
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    if (umgebung::subsystem_graphics->draw_pre != nullptr) {
        umgebung::subsystem_graphics->draw_pre();
    }

    draw();

    if (umgebung::subsystem_graphics->draw_post != nullptr) {
        umgebung::subsystem_graphics->draw_post();
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    /*
     * 1. call `void umgebung::quit()`(?)
     * 2. clean up subsytems e.g audio, graphics, ...
     */
    if (umgebung::subsystem_graphics->shutdown != nullptr) {
        umgebung::subsystem_graphics->shutdown();
    }

    delete umgebung::subsystem_graphics;
    SDL_Quit();
}
