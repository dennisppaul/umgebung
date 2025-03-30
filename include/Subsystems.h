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

#include <SDL3/SDL.h>

#include "PGraphics.h"
#include "PAudio.h"

namespace umgebung {

    struct Subsystem {
        void (*set_flags)(uint32_t& subsystem_flags);
        bool (*init)();
        void (*setup_pre)();
        void (*setup_post)();
        void (*loop)(); /* higher frequency loop but on same thread as draw, called before each draw */
        void (*draw_pre)();
        void (*draw_post)();
        void (*shutdown)();
        void (*event)(SDL_Event* event);
        void (*event_loop)(SDL_Event* event);
        const char* (*name)();
    };

    struct SubsystemGraphics : Subsystem {
        PGraphics* (*create_main_graphics)(bool render_to_offscreen);
        void (*post)();
        void (*set_title)(const char* title);
        SDL_Window* (*get_sdl_window)();
        void* (*get_renderer)();
        int (*get_renderer_type)();
    };

    struct SubsystemAudio : Subsystem {
        void (*start)(PAudio* device);
        void (*stop)(PAudio* device);
        PAudio* (*create_audio)(const AudioUnitInfo* device_info);
    };

    /**
    * this function pointer is used to create an audio subsystem.
    *
    * it can be used e.g like this:
    *
    *     create_subsystem_audio = []() -> SubsystemAudio* {
    *         return umgebung_subsystem_audio_create_sdl();
    *     };
    *
    * or like this:
    *
    *     SubsystemAudio* create_sdl_audio() {
    *         return umgebung_subsystem_audio_create_sdl(); // <<< example implementation
    *     }
    *
    *     void settings() {
    *        create_subsystem_audio = create_sdl_audio;
    *        ...
    *     }
    */
    inline SubsystemAudio* (*create_subsystem_audio)()       = nullptr;
    inline SubsystemGraphics* (*create_subsystem_graphics)() = nullptr;

    class LibraryListener {
    public:
        virtual ~LibraryListener() = default;
        // TODO what are they listening to?
        virtual void setup_pre()                  = 0;
        virtual void setup_post()                 = 0;
        virtual void draw_pre()                   = 0;
        virtual void draw_post()                  = 0;
        virtual void event(SDL_Event* event)      = 0; // NOTE events maybe handled in own thread
        virtual void event_loop(SDL_Event* event) = 0; // NOTE events are handled in the main loop
        virtual void shutdown()                   = 0;
    };
} // namespace umgebung

/* implemented subsystems */

umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_sdl2d();
umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_openglv20();
umgebung::SubsystemGraphics* umgebung_create_subsystem_graphics_openglv33();
umgebung::SubsystemAudio*    umgebung_create_subsystem_audio_sdl();
umgebung::SubsystemAudio*    umgebung_create_subsystem_audio_portaudio();
umgebung::Subsystem*         umgebung_create_subsystem_hid_events();
umgebung::Subsystem*         umgebung_create_subsystem_libraries();
