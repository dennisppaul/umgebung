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
#include "UmgebungCallbacks.h"

namespace umgebung {

    void handle_events_in_loop(bool events_in_loop);

    static bool _handle_events_in_loop = true;
    static bool _mouse_is_pressed      = false;

    void handle_events_in_loop(bool events_in_loop) {
        _handle_events_in_loop = events_in_loop;
    }

    static void handle_hid_event(const SDL_Event& event) {
        // imgui_processevent(event);

        // generic sdl event handler
        // TODO maybe cache this and handle it in loop
        sdl_event(event);

        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                umgebung::key = static_cast<int>(event.key.key);
                // if (!imgui_is_keyboard_captured()) {
                keyPressed();
                // }
                umgebung::isKeyPressed = true;
                break;
            case SDL_EVENT_KEY_UP:
                // if (imgui_is_keyboard_captured()) { break; }
                umgebung::key          = static_cast<int>(event.key.key);
                umgebung::isKeyPressed = false;
                keyReleased();
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                // if (imgui_is_mouse_captured()) { break; }
                umgebung::mouseButton = event.button.button;
                _mouse_is_pressed     = true;
                mousePressed();
                umgebung::isMousePressed = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                // if (imgui_is_mouse_captured()) { break; }
                _mouse_is_pressed     = false;
                umgebung::mouseButton = -1;
                mouseReleased();
                umgebung::isMousePressed = false;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                // if (imgui_is_mouse_captured()) { break; }
                umgebung::pmouseX = umgebung::mouseX;
                umgebung::pmouseY = umgebung::mouseY;
                umgebung::mouseX  = static_cast<float>(event.motion.x);
                umgebung::mouseY  = static_cast<float>(event.motion.y);

                if (_mouse_is_pressed) {
                    mouseDragged();
                } else {
                    mouseMoved();
                }
                break;
                // case SDL_MULTIGESTURE:
            case SDL_EVENT_MOUSE_WHEEL:
                // if (imgui_is_mouse_captured()) { break; }
                mouseWheel(event.wheel.mouse_x, event.wheel.mouse_y);
                break;
            case SDL_EVENT_DROP_FILE: {
                // only allow drag and drop on main window
                // if (event.drop.windowID != 1) { break; }
                const char* dropped_filedir = event.drop.data;
                dropped(dropped_filedir);
                break;
            }
            default: break;
        }
    }

    static void shutdown() {}

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_EVENTS;
    }

    static bool is_hid_event(const SDL_Event* event) {
        return event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               event->type == SDL_EVENT_MOUSE_BUTTON_UP ||
               event->type == SDL_EVENT_MOUSE_MOTION ||
               event->type == SDL_EVENT_MOUSE_WHEEL ||
               event->type == SDL_EVENT_KEY_UP ||
               event->type == SDL_EVENT_KEY_DOWN ||
               event->type == SDL_EVENT_DROP_FILE;
    }
    static void event(SDL_Event* event) {
        if (is_hid_event(event)) {
            if (!_handle_events_in_loop) {
                handle_hid_event(*event);
            }
        }
    }

    static void event_loop(SDL_Event* event) {
        if (is_hid_event(event)) {
            if (_handle_events_in_loop) {
                handle_hid_event(*event);
            }
        }
    }

    static const char* name() {
        return "HID Events ( mouse, keyboard, drag-n-drop, ... )";
    }
} // namespace umgebung

umgebung::Subsystem* umgebung_create_subsystem_hid_events() {
    auto* libraries       = new umgebung::Subsystem{};
    libraries->shutdown   = umgebung::shutdown;
    libraries->set_flags  = umgebung::set_flags;
    libraries->event      = umgebung::event;
    libraries->event_loop = umgebung::event_loop;
    libraries->name       = umgebung::name;
    return libraries;
}
