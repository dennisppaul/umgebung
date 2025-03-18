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

#include "Umgebung.h"

// TODO add functionality to (un-)register libraries

namespace umgebung {

    class LibraryListener;
    void register_library(LibraryListener* listener);
    void unregister_library(const LibraryListener* listener);

    class LibraryListener {
    public:
        virtual ~LibraryListener() = default;
        // TODO what are they listening to?
        virtual void setup_pre()                  = 0;
        virtual void setup_post()                 = 0;
        virtual void draw_pre()                   = 0;
        virtual void draw_post()                  = 0;
        virtual void event(SDL_Event* event)      = 0;
        virtual void event_loop(SDL_Event* event) = 0;
    };

    static std::vector<LibraryListener*> _listeners;

    void register_library(LibraryListener* listener) {
        if (listener != nullptr) {
            _listeners.push_back(listener);
        }
    }

    void unregister_library(const LibraryListener* listener) {
        if (listener != nullptr) {
            _listeners.erase(std::find(_listeners.begin(), _listeners.end(), listener));
        }
    }

    static void shutdown() {
        // TODO clean up
    }

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_EVENTS;
    }

    static void setup_pre() {}
    static void setup_post() {}
    static void draw_pre() {}
    static void draw_post() {}

    static void event(SDL_Event* event) {
        // TODO console("library@event");
    }

    static void event_loop(SDL_Event* event) {
        // TODO console("library@event_loop");
    }

    static const char* name() {
        return "Client Libraries";
    }
} // namespace umgebung

umgebung::Subsystem* umgebung_create_subsystem_libraries() {
    auto* libraries       = new umgebung::Subsystem{};
    libraries->shutdown   = umgebung::shutdown;
    libraries->set_flags  = umgebung::set_flags;
    libraries->setup_pre  = umgebung::setup_pre;
    libraries->setup_post = umgebung::setup_post;
    libraries->draw_pre   = umgebung::draw_pre;
    libraries->draw_post  = umgebung::draw_post;
    libraries->event      = umgebung::event;
    libraries->event_loop = umgebung::event_loop;
    libraries->name       = umgebung::name;
    return libraries;
}
