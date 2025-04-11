/*
 * Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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

#include "Umfeld.h"
#include "UmfeldCallbacks.h"

namespace umfeld {

    static bool _handle_events_in_loop = true;
    static bool _mouse_is_pressed      = false;

    void hid_handle_events_in_loop(const bool events_in_loop) {
        _handle_events_in_loop = events_in_loop;
    }

    static void handle_gamepad_event(const SDL_Event& event) {
        sdl_event(event);

        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                umfeld::key = static_cast<int>(event.key.key);
                keyPressed();
                umfeld::isKeyPressed = true;
                break;
            case SDL_EVENT_KEY_UP:
                umfeld::key          = static_cast<int>(event.key.key);
                umfeld::isKeyPressed = false;
                keyReleased();
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                umfeld::mouseButton = event.button.button; // TODO not sure how consistent these are across platforms
                _mouse_is_pressed   = true;
                mousePressed();
                umfeld::isMousePressed = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                _mouse_is_pressed   = false;
                umfeld::mouseButton = -1;
                mouseReleased();
                umfeld::isMousePressed = false;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                umfeld::pmouseX = umfeld::mouseX;
                umfeld::pmouseY = umfeld::mouseY;
                umfeld::mouseX  = static_cast<float>(event.motion.x);
                umfeld::mouseY  = static_cast<float>(event.motion.y);

                if (_mouse_is_pressed) {
                    mouseDragged();
                } else {
                    mouseMoved();
                }
                break;
                // case SDL_MULTIGESTURE:
            case SDL_EVENT_MOUSE_WHEEL:
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

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event(SDL_Event* event) {
        if (is_hid_event(event)) {
            if (!_handle_events_in_loop) {
                handle_gamepad_event(*event);
            }
        }
    }

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    static void event_in_update_loop(SDL_Event* event) {
        if (is_hid_event(event)) {
            if (_handle_events_in_loop) {
                handle_gamepad_event(*event);
            }
        }
    }

    static const char* name() {
        return "HID Events ( mouse, keyboard, drag-n-drop, … )";
    }
} // namespace umfeld

umfeld::Subsystem* umfeld_create_subsystem_hid() {
    auto* libraries                 = new umfeld::Subsystem{};
    libraries->shutdown             = umfeld::shutdown;
    libraries->set_flags            = umfeld::set_flags;
    libraries->event                = umfeld::event;
    libraries->event_in_update_loop = umfeld::event_in_update_loop;
    libraries->name                 = umfeld::name;
    return libraries;
}
