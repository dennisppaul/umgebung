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
#include "Gamepad.h"


namespace umfeld {

    void enable_gamepads() {
        add_subsystem(umfeld_create_subsystem_gamepad());
    }

    static bool _handle_events_in_loop = true;
    static int  _motion_event_cooldown = 0;
    static int  _num_gamepads          = 0;
    static bool _print_debug           = false;

    void gamepad_handle_events_in_loop(const bool events_in_loop) {
        _handle_events_in_loop = events_in_loop;
    }

    void gamepad_motion_event_cooldown(const int milliseconds) {
        _motion_event_cooldown = milliseconds;
    }

    std::vector<SDL_Gamepad*> gamepad_connected(const bool print) {
        int                   num_gamepads = 0;
        const SDL_JoystickID* gamepads     = SDL_GetGamepads(&num_gamepads);
        if (print) {
            if (num_gamepads > 0) {
                console("number of gamepads connected: ", num_gamepads);
                for (int i = 0; i < num_gamepads; ++i) {
                    SDL_Gamepad* gamepad = SDL_GetGamepadFromID(gamepads[i]);
                    if (gamepad) {
                        console("[", i, "] Gamepad #", gamepads[i], " '", SDL_GetGamepadName(gamepad));
                    } else {
                        console("[", i, "] Gamepad #", gamepads[i], " not valid");
                    }
                }
            } else {
                console("No gamepads connected");
            }
        }
        std::vector<SDL_Gamepad*> gamepads_list;
        for (int i = 0; i < num_gamepads; ++i) {
            gamepads_list.push_back(SDL_GetGamepadFromID(gamepads[i]));
        }
        return gamepads_list;
    }

    void gamepad_print_debug(const bool print_debug) {
        _print_debug = print_debug;
    }

    static void handle_gamepad_event(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                /**< Gamepad axis motion */
                {
                    static Uint64 axis_motion_cooldown_time = 0; /* these are spammy, only show every X milliseconds. */
                    const Uint64  now                       = SDL_GetTicks();
                    if (axis_motion_cooldown_time <= 0 || now >= axis_motion_cooldown_time) {
                        const SDL_JoystickID which = event.gaxis.which;
                        axis_motion_cooldown_time  = now + _motion_event_cooldown;
                        const float axis_value_f   = static_cast<float>(event.gaxis.value) / 32767.0f;
                        if (_print_debug) {
                            console("Gamepad #", which, " axis: ", static_cast<int>(event.gaxis.axis), " : ", axis_value_f);
                        }
                        gamepadAxis(which, event.gaxis.axis, axis_value_f);
                    }
                }
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                /**< Gamepad button pressed */
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                /**< Gamepad button released */
                {
                    const SDL_JoystickID which = event.gbutton.which;
                    if (_print_debug) {
                        console("Gamepad #", which, " button: ", static_cast<int>(event.gbutton.button), ": ", event.gbutton.down ? "PRESSED" : "RELEASED");
                    }
                    gamepadButton(which, event.gbutton.button, event.gbutton.down);
                }
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                /**< A new gamepad has been inserted into the system */
                {
                    const SDL_JoystickID which   = event.gdevice.which;
                    SDL_Gamepad*         gamepad = SDL_OpenGamepad(which);
                    if (gamepad) {
                        _num_gamepads++;
                        if (_print_debug) {
                            console("Gamepad #", static_cast<int>(which), " '", SDL_GetGamepadName(gamepad), "' added");
                        }
                    } else {
                        if (_print_debug) {
                            console("Gamepad #", static_cast<int>(which), " added, but not opened: %s", SDL_GetError());
                        }
                    }
                }
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                /**< A gamepad has been removed */
                {
                    const SDL_JoystickID which   = event.gdevice.which;
                    SDL_Gamepad*         gamepad = SDL_GetGamepadFromID(which);
                    if (gamepad) {
                        _num_gamepads--;
                        if (_num_gamepads < 0) {
                            _num_gamepads = 0;
                        }
                        if (_print_debug) {
                            console("Gamepad #", static_cast<int>(which), " '", SDL_GetGamepadName(gamepad), "' removed");
                        }
                    } else {
                        if (_print_debug) {
                            console("Gamepad #", static_cast<int>(which), " removed, but was not valid: %s", SDL_GetError());
                        }
                    }
                }
                break;
            case SDL_EVENT_GAMEPAD_REMAPPED:
                /**< The gamepad mapping was updated */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_REMAPPED");
                }
                break;
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                /**< Gamepad touchpad was touched */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN");
                }
                break;
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                /**< Gamepad touchpad finger was moved */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION");
                }
                break;
            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                /**< Gamepad touchpad finger was lifted */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_TOUCHPAD_UP");
                }
                break;
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                /**< Gamepad sensor was updated */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_SENSOR_UPDATE");
                }
                break;
            case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
                /**< Gamepad update is complete */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_UPDATE_COMPLETE");
                }
                break;
            case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
                /**< Gamepad Steam handle has changed */
                if (_print_debug) {
                    console(event.gdevice.which, " SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED");
                }
                break;
            default: break;
        }
    }

    static void shutdown() {}

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_GAMEPAD;
    }

    static bool is_hid_event(const SDL_Event* event) {
        return event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION ||
               event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ||
               event->type == SDL_EVENT_GAMEPAD_BUTTON_UP ||
               event->type == SDL_EVENT_GAMEPAD_ADDED ||
               event->type == SDL_EVENT_GAMEPAD_REMOVED ||
               event->type == SDL_EVENT_GAMEPAD_REMAPPED ||
               event->type == SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN ||
               event->type == SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION ||
               event->type == SDL_EVENT_GAMEPAD_TOUCHPAD_UP ||
               event->type == SDL_EVENT_GAMEPAD_SENSOR_UPDATE ||
               event->type == SDL_EVENT_GAMEPAD_UPDATE_COMPLETE ||
               event->type == SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED;
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
        return "Gamepad ( Nintendo Switch Joy-Con, XBox One, ... )";
    }
} // namespace umfeld

WEAK void gamepadButton(const int id, const int button, const bool down) {
    if (umfeld::_print_debug) {
        umfeld::console("gamepad button", id, " : ", button, " : ", down ? "PRESSED" : "RELEASED");
    }
}

WEAK void gamepadAxis(const int id, const int axis, const float value) {
    if (umfeld::_print_debug) {
        umfeld::console("gamepad axis  ", id, " : ", axis, " : ", value);
    }
}

WEAK void gamepadEvent(const SDL_Event& event) {
    if (umfeld::_print_debug) {
        umfeld::console("gamepad event ", event.type);
    }
}

umfeld::Subsystem* umfeld_create_subsystem_gamepad() {
    auto* libraries                 = new umfeld::Subsystem{};
    libraries->shutdown             = umfeld::shutdown;
    libraries->set_flags            = umfeld::set_flags;
    libraries->event                = umfeld::event;
    libraries->event_in_update_loop = umfeld::event_in_update_loop;
    libraries->name                 = umfeld::name;
    return libraries;
}
