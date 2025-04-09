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
#include <SDL3/SDL_main.h>

#include <iostream>
#include <string>

#include "Umfeld.h"
#include "PAudio.h"

using namespace std::chrono;

namespace umfeld {
    static high_resolution_clock::time_point lastFrameTime                       = {};
    static bool                              initialized                         = false;
    static double                            target_frame_duration               = 1.0 / frameRate;
    static bool                              handle_subsystem_graphics_cleanup   = false;
    static bool                              handle_subsystem_audio_cleanup      = false;
    static bool                              handle_subsystem_libraries_cleanup  = false;
    static bool                              handle_subsystem_hid_events_cleanup = false;
    static bool                              _app_is_running                     = true;
    static std::vector<SDL_Event>            event_cache;

    void exit() {
        _app_is_running = false; // NOTE layzily implemented it here and not in `UmfeldFunctionsGraphics.cpp`
    }

    bool is_initialized() {
        return initialized;
    }

    std::string get_window_title() {
        // TODO move this to subsystem and make it configurable
        return UMFELD_WINDOW_TITLE;
    }

    void set_window_title(std::string title) {
        // TODO move this to subsystem
    }

    void set_frame_rate(const float fps) {
        target_frame_duration = 1.0 / fps;
    }

    SDL_WindowFlags get_SDL_WindowFlags(SDL_WindowFlags& flags) {
        /*
         * SDL_WINDOW_FULLSCREEN           SDL_UINT64_C(0x0000000000000001)    //  window is in fullscreen mode
         * SDL_WINDOW_OPENGL               SDL_UINT64_C(0x0000000000000002)    //  window usable with OpenGL context
         * SDL_WINDOW_OCCLUDED             SDL_UINT64_C(0x0000000000000004)    //  window is occluded
         * SDL_WINDOW_HIDDEN               SDL_UINT64_C(0x0000000000000008)    //  window is neither mapped onto the desktop nor shown in the taskbar/dock/window list; SDL_ShowWindow() is required for it to become visible
         * SDL_WINDOW_BORDERLESS           SDL_UINT64_C(0x0000000000000010)    //  no window decoration
         * SDL_WINDOW_RESIZABLE            SDL_UINT64_C(0x0000000000000020)    //  window can be resized
         * SDL_WINDOW_MINIMIZED            SDL_UINT64_C(0x0000000000000040)    //  window is minimized
         * SDL_WINDOW_MAXIMIZED            SDL_UINT64_C(0x0000000000000080)    //  window is maximized
         * SDL_WINDOW_MOUSE_GRABBED        SDL_UINT64_C(0x0000000000000100)    //  window has grabbed mouse input
         * SDL_WINDOW_INPUT_FOCUS          SDL_UINT64_C(0x0000000000000200)    //  window has input focus
         * SDL_WINDOW_MOUSE_FOCUS          SDL_UINT64_C(0x0000000000000400)    //  window has mouse focus
         * SDL_WINDOW_EXTERNAL             SDL_UINT64_C(0x0000000000000800)    //  window not created by SDL
         * SDL_WINDOW_MODAL                SDL_UINT64_C(0x0000000000001000)    //  window is modal
         * SDL_WINDOW_HIGH_PIXEL_DENSITY   SDL_UINT64_C(0x0000000000002000)    //  window uses high pixel density back buffer if possible
         * SDL_WINDOW_MOUSE_CAPTURE        SDL_UINT64_C(0x0000000000004000)    //  window has mouse captured (unrelated to MOUSE_GRABBED)
         * SDL_WINDOW_MOUSE_RELATIVE_MODE  SDL_UINT64_C(0x0000000000008000)    //  window has relative mode enabled
         * SDL_WINDOW_ALWAYS_ON_TOP        SDL_UINT64_C(0x0000000000010000)    //  window should always be above others
         * SDL_WINDOW_UTILITY              SDL_UINT64_C(0x0000000000020000)    //  window should be treated as a utility window, not showing in the task bar and window list
         * SDL_WINDOW_TOOLTIP              SDL_UINT64_C(0x0000000000040000)    //  window should be treated as a tooltip and does not get mouse or keyboard focus, requires a parent window
         * SDL_WINDOW_POPUP_MENU           SDL_UINT64_C(0x0000000000080000)    //  window should be treated as a popup menu, requires a parent window
         * SDL_WINDOW_KEYBOARD_GRABBED     SDL_UINT64_C(0x0000000000100000)    //  window has grabbed keyboard input
         * SDL_WINDOW_VULKAN               SDL_UINT64_C(0x0000000010000000)    //  window usable for Vulkan surface
         * SDL_WINDOW_METAL                SDL_UINT64_C(0x0000000020000000)    //  window usable for Metal view
         * SDL_WINDOW_TRANSPARENT          SDL_UINT64_C(0x0000000040000000)    //  window with transparent buffer
         * SDL_WINDOW_NOT_FOCUSABLE        SDL_UINT64_C(0x0000000080000000)    //  window should not be focusable
         */
        flags |= SDL_WINDOW_HIDDEN; // NOTE always hide window
        flags |= SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;
        flags |= fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
        flags |= borderless ? SDL_WINDOW_BORDERLESS : 0;
        flags |= resizable ? SDL_WINDOW_RESIZABLE : 0;
        flags |= retina_support ? SDL_WINDOW_HIGH_PIXEL_DENSITY : 0;
        flags |= always_on_top ? SDL_WINDOW_ALWAYS_ON_TOP : 0;
        return flags;
    }
} // namespace umfeld

static void handle_arguments(const int argc, char* argv[]) {
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
    // subsystem_flags |= SDL_INIT_EVENTS;
    return subsystem_flags;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

    /*
     * 1. prepare umfeld application ( e.g args, settings )
     * 2. initialize SDL
     * 3. initialize graphics
     * 4. initialize audio
     * 5. setup application
     */

    /* 1. prepare umfeld application */

    handle_arguments(argc, argv);
    settings();

    /* create/check graphics subsystem */
    if (umfeld::enable_graphics) {
        if (umfeld::renderer > umfeld::DEFAULT) {
            umfeld::console("+++ setting renderer from paramter `size()`.");
            switch (umfeld::renderer) {
                case umfeld::OPENGL_2_0:
                    umfeld::subsystem_graphics = umfeld_create_subsystem_graphics_openglv20();
                    break;
                case umfeld::OPENGL_ES_3_0:
                    umfeld::warning("+++ OpenGL ES 3.0 not supported yet.");
                    break;
                default:
                case umfeld::OPENGL_3_3:
                    umfeld::subsystem_graphics = umfeld_create_subsystem_graphics_openglv33();
            }
        }
        if (umfeld::subsystem_graphics == nullptr) {
            if (umfeld::create_subsystem_graphics != nullptr) {
                umfeld::console("+++ creating graphics subsystem with callback.");
                umfeld::subsystem_graphics                = umfeld::create_subsystem_graphics();
                umfeld::handle_subsystem_graphics_cleanup = true;
            } else {
                umfeld::console("+++ no graphics subsystem provided, using default.");
                // umfeld::subsystem_graphics = umfeld_create_subsystem_graphics_sdl2d();
                // umfeld::subsystem_graphics = umfeld_create_subsystem_graphics_openglv20();
                umfeld::subsystem_graphics                = umfeld_create_subsystem_graphics_openglv33();
                umfeld::handle_subsystem_graphics_cleanup = true;
            }
            if (umfeld::subsystem_graphics == nullptr) {
                umfeld::console("+++ did not create graphics subsystem.");
            }
        } else {
            umfeld::console("+++ client provided graphics subsystem ( including `size()` ).");
        }
        if (umfeld::subsystem_graphics != nullptr) {
            umfeld::subsystems.push_back(umfeld::subsystem_graphics);
            if (umfeld::subsystem_graphics->name != nullptr) {
                umfeld::console("+++ created subsystem graphics  : ", umfeld::subsystem_graphics->name());
            } else {
                umfeld::console("+++ created subsystem graphics  : ( no name specified )");
            }
        }
    } else {
        umfeld::console("+++ graphics disabled.");
    }

    /* create/check audio subsystem */
    if (umfeld::enable_audio) {
        if (umfeld::subsystem_audio == nullptr) {
            if (umfeld::create_subsystem_audio != nullptr) {
                umfeld::console("+++ creating audio subsystem via callback.");
                umfeld::subsystem_audio                = umfeld::create_subsystem_audio();
                umfeld::handle_subsystem_audio_cleanup = true;
            } else {
                umfeld::console("+++ no audio subsystem provided, using default ( PortAudio ).");
                // umfeld::subsystem_audio                = umfeld_create_subsystem_audio_sdl();
                umfeld::subsystem_audio                = umfeld_create_subsystem_audio_portaudio();
                umfeld::handle_subsystem_audio_cleanup = true;
            }
            if (umfeld::subsystem_audio == nullptr) {
                umfeld::console("+++ did not create audio subsystem.");
            }
        } else {
            umfeld::console("+++ client provided audio subsystem.");
        }
        umfeld::console("+++ adding audio subsystem.");
        umfeld::subsystems.push_back(umfeld::subsystem_audio);
        if (umfeld::subsystem_audio->name != nullptr) {
            umfeld::console("+++ created subsystem audio     : ", umfeld::subsystem_audio->name());
        } else {
            umfeld::console("+++ created subsystem audio     : ( no name specified )");
        }
    } else {
        umfeld::console("+++ audio disabled.");
    }

    /* create/check libraries subsystem */
    if (umfeld::enable_libraries) {
        if (umfeld::subsystem_libraries == nullptr) {
            umfeld::subsystem_libraries                = umfeld_create_subsystem_libraries();
            umfeld::handle_subsystem_libraries_cleanup = true;
        } else {
            umfeld::console("+++ client provided library subsystem.");
        }
        umfeld::console("+++ created subsystem libraries : ", umfeld::subsystem_libraries->name());
        umfeld::subsystems.push_back(umfeld::subsystem_libraries);
    } else {
        umfeld::console("+++ libraries disabled.");
    }

    /* create/check events subsystem */
    if (umfeld::enable_events) {
        if (umfeld::subsystem_hid_events == nullptr) {
            umfeld::subsystem_hid_events                = umfeld_create_subsystem_hid_events();
            umfeld::handle_subsystem_hid_events_cleanup = true;
        } else {
            umfeld::console("+++ client provided HID events subsystem.");
        }
        umfeld::console("+++ created subsystem HID events: ", umfeld::subsystem_hid_events->name());
        umfeld::subsystems.push_back(umfeld::subsystem_hid_events);
    } else {
        umfeld::console("+++ HID events disabled.");
    }

    /* 2. initialize SDL */

    uint32_t subsystem_flags = compile_subsystems_flag();

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->set_flags != nullptr) {
                subsystem->set_flags(subsystem_flags);
            }
        }
    }

    if (!SDL_Init(subsystem_flags)) {
        SDL_Log("couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // TODO make this configurable
    SDL_SetAppMetadata("Umfeld Application", "1.0", "de.dennisppaul.umfeld.application");

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->init != nullptr) {
                if (!subsystem->init()) {
                    umfeld::warning("Couldn't initialize subsystem.");
                }
            }
        }
    }

    if (umfeld::enable_graphics) {
        if (umfeld::subsystem_graphics != nullptr) {
            if (umfeld::subsystem_graphics->create_native_graphics != nullptr) {
                umfeld::g = umfeld::subsystem_graphics->create_native_graphics(umfeld::render_to_buffer);
            }
        }
    }

    if (umfeld::enable_audio) {
        if (umfeld::subsystem_audio != nullptr) {
            if (umfeld::subsystem_audio->create_audio != nullptr) {
                // NOTE fill in the values from `Umfeld.h`
                umfeld::AudioUnitInfo _audio_unit_info;
                // _audio_unit_info.unique_id       = 0; // NOTE set by subsystem
                _audio_unit_info.input_buffer       = nullptr;
                _audio_unit_info.input_channels     = umfeld::audio_input_channels;
                _audio_unit_info.output_buffer      = nullptr;
                _audio_unit_info.output_channels    = umfeld::audio_output_channels;
                _audio_unit_info.buffer_size        = umfeld::audio_buffer_size;
                _audio_unit_info.sample_rate        = umfeld::audio_sample_rate;
                _audio_unit_info.input_device_id    = umfeld::audio_input_device_id;
                _audio_unit_info.input_device_name  = umfeld::audio_input_device_name;
                _audio_unit_info.output_device_id   = umfeld::audio_output_device_id;
                _audio_unit_info.output_device_name = umfeld::audio_output_device_name;
                umfeld::a                         = umfeld::subsystem_audio->create_audio(&_audio_unit_info);
            }
        }
    }

    umfeld::initialized = true;

    /* - setup_pre */

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->setup_pre != nullptr) {
                subsystem->setup_pre();
            }
        }
    }

    // TODO feed back the values to the global variables
    //      NEED TO find a good place to do this … also for audio
    //      values should be updated before `setup()` is called:
    if (umfeld::g != nullptr && umfeld::enable_graphics) {
        umfeld::width  = umfeld::g->width;
        umfeld::height = umfeld::g->height;
    }
    if (umfeld::a != nullptr && umfeld::enable_audio) {
        // NOTE copy values back to global variables after initialization … a bit hackish but well.
        umfeld::audio_input_buffer       = umfeld::a->input_buffer;
        umfeld::audio_input_channels           = umfeld::a->input_channels;
        umfeld::audio_output_buffer      = umfeld::a->output_buffer;
        umfeld::audio_output_channels          = umfeld::a->output_channels;
        umfeld::audio_buffer_size        = umfeld::a->buffer_size;
        umfeld::audio_sample_rate              = umfeld::a->sample_rate;
        umfeld::audio_input_device_id    = umfeld::a->input_device_id;
        umfeld::audio_input_device_name  = umfeld::a->input_device_name;
        umfeld::audio_output_device_id   = umfeld::a->output_device_id;
        umfeld::audio_output_device_name = umfeld::a->output_device_name;
    }

    setup();

    /* - setup_post */

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->setup_post != nullptr) {
                subsystem->setup_post();
            }
        }
    }

    umfeld::lastFrameTime = std::chrono::high_resolution_clock::now();

    return SDL_APP_CONTINUE;
}

static void handle_event(const SDL_Event& event, bool& app_is_running) {
    switch (event.type) {
        case SDL_EVENT_WINDOW_RESIZED:
            // // TODO implement window resize … how will the subsystems be updated?
            umfeld::warning("TODO window resized. subsystem needs to be update …");
            windowResized(-1, -1);
            break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            // // TODO implement
            break;
        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_HIDDEN:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
            // umfeld::warning("TODO window status has changed ( e.g minimized, maximaized, shown, hidden )");
            break;
        case SDL_EVENT_QUIT:
            app_is_running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (umfeld::use_esc_key_to_quit) {
                umfeld::key = static_cast<int>(event.key.key);
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    app_is_running = false;
                }
            }
            break;
        default: break;
    }
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->event != nullptr) {
                subsystem->event(event);
            }
        }
    }

    umfeld::event_cache.push_back(*event);

    /* only quit events */
    handle_event(*event, umfeld::_app_is_running);
    if (!umfeld::_app_is_running) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

static void handle_draw() {
    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->draw_pre != nullptr) {
                subsystem->draw_pre();
            }
        }
    }

    draw();

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->draw_post != nullptr) {
                subsystem->draw_post();
            }
        }
    }

    if (umfeld::subsystem_graphics != nullptr) {
        if (umfeld::subsystem_graphics->post != nullptr) {
            umfeld::subsystem_graphics->post();
        }
    }

    post();
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    const high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();
    const auto                              frameDuration    = duration_cast<duration<double>>(currentFrameTime - umfeld::lastFrameTime);
    const double                            frame_duration   = frameDuration.count();

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->event_in_update_loop != nullptr) {
                for (auto e: umfeld::event_cache) {
                    subsystem->event_in_update_loop(&e);
                }
            }
        }
    }
    umfeld::event_cache.clear();

    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->update_loop != nullptr) {
                subsystem->update_loop();
            }
        }
    }

    if (frame_duration >= umfeld::target_frame_duration) {
        handle_draw();

        if (frame_duration == 0) {
            umfeld::frameRate = 1;
        } else {
            umfeld::frameRate = static_cast<float>(1.0 / frame_duration);
        }

        umfeld::frameCount++;
        umfeld::lastFrameTime = currentFrameTime;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    // NOTE 1. call `void umfeld::shutdown()`(?)
    //      2. clean up subsytems e.g audio, graphics, ...
    for (const umfeld::Subsystem* subsystem: umfeld::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->shutdown != nullptr) {
                subsystem->shutdown();
            }
            // NOTE custom subsystems must be cleaned by client: `delete subsystem;`
        }
    }

    // NOTE clean up graphics + audio subsystem if created internally
    if (umfeld::subsystem_graphics != nullptr) {
        if (umfeld::handle_subsystem_graphics_cleanup) {
            delete umfeld::subsystem_graphics;
        }
        umfeld::subsystem_graphics = nullptr;
    }
    if (umfeld::subsystem_audio != nullptr) {
        if (umfeld::handle_subsystem_audio_cleanup) {
            delete umfeld::subsystem_audio;
        }
        umfeld::subsystem_audio = nullptr;
    }
    if (umfeld::subsystem_libraries != nullptr) {
        if (umfeld::handle_subsystem_libraries_cleanup) {
            delete umfeld::subsystem_libraries;
        }
        umfeld::subsystem_libraries = nullptr;
    }
    if (umfeld::subsystem_hid_events != nullptr) {
        if (umfeld::handle_subsystem_hid_events_cleanup) {
            delete umfeld::subsystem_hid_events;
        }
        umfeld::subsystem_hid_events = nullptr;
    }

    umfeld::subsystems.clear();

    shutdown();
    SDL_Quit();
}
