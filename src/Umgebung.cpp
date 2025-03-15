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
#include <string>

#include "Umgebung.h"
#include "PAudio.h"

namespace umgebung {
    static std::chrono::high_resolution_clock::time_point lastFrameTime                     = {};
    static bool                                           initialized                       = false;
    static double                                         target_frame_duration             = 1.0 / frameRate;
    static bool                                           handle_subsystem_graphics_cleanup = false;
    static bool                                           handle_subsystem_audio_cleanup    = false;

    // TODO move the functions to the respective subsystems
    void umgebung_subsystem_events_init();

    bool is_initialized() {
        return initialized;
    }

    std::string get_window_title() {
        return UMGEBUNG_WINDOW_TITLE;
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
        // SDL_WindowFlags flags = SDL_WINDOW_OPENGL; // TODO this needs to come from subsystem
        flags |= SDL_WINDOW_HIDDEN; // always hide window
        // flags |= SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;
        flags |= fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
        flags |= borderless ? SDL_WINDOW_BORDERLESS : 0;
        flags |= resizable ? SDL_WINDOW_RESIZABLE : 0;
        flags |= retina_support ? SDL_WINDOW_HIGH_PIXEL_DENSITY : 0;
        flags |= always_on_top ? SDL_WINDOW_ALWAYS_ON_TOP : 0;
        return flags;
    }
} // namespace umgebung

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
    subsystem_flags |= SDL_INIT_EVENTS;
    return subsystem_flags;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

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

    /* check graphics subsystem */
    if (umgebung::enable_graphics) {
        if (umgebung::subsystem_graphics == nullptr) {
            if (umgebung::create_subsystem_graphics != nullptr) {
                umgebung::console("+++ creating graphics subsystem with callback.");
                umgebung::subsystem_graphics                = umgebung::create_subsystem_graphics();
                umgebung::handle_subsystem_graphics_cleanup = true;
            } else {
                umgebung::console("+++ no graphics subsystem provided, using default.");
                // umgebung::subsystem_graphics = umgebung_create_subsystem_graphics_sdl2d();
                // umgebung::subsystem_graphics = umgebung_create_subsystem_graphics_openglv20();
                umgebung::subsystem_graphics                = umgebung_create_subsystem_graphics_openglv33();
                umgebung::handle_subsystem_graphics_cleanup = true;
            }
            if (umgebung::subsystem_graphics == nullptr) {
                umgebung::console("+++ did not create graphics subsystem.");
            }
        } else {
            umgebung::console("+++ client provided graphics subsystem.");
        }
        // TODO move graphics subsystem to subsystems vector
        // TODO check if this causes any problem … but it could nicely clean up things!!!
        // umgebung::subsystems.push_back(umgebung::subsystem_graphics);
    } else {
        umgebung::console("+++ graphics disabled.");
    }

    /* check audio subsystem */
    if (umgebung::enable_audio) {
        if (umgebung::subsystem_audio == nullptr) {
            if (umgebung::create_subsystem_audio != nullptr) {
                umgebung::console("+++ creating audio subsystem via callback.");
                umgebung::subsystem_audio                = umgebung::create_subsystem_audio();
                umgebung::handle_subsystem_audio_cleanup = true;
            } else {
                umgebung::console("+++ no audio subsystem provided, using default.");
                umgebung::subsystem_audio                = umgebung_create_subsystem_audio_sdl();
                umgebung::handle_subsystem_audio_cleanup = true;
            }
            if (umgebung::subsystem_audio == nullptr) {
                umgebung::console("+++ did not create audio subsystem.");
            }
        } else {
            umgebung::console("+++ client provided audio subsystem.");
        }
        umgebung::console("+++ adding audio subsystem.");
        umgebung::subsystems.push_back(umgebung::subsystem_audio);
    } else {
        umgebung::console("+++ audio disabled.");
    }

    /* 2. initialize SDL */

    uint32_t subsystem_flags = compile_subsystems_flag();

    // TODO maybe move this to subsytems vector
    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->set_flags != nullptr) {
            umgebung::subsystem_graphics->set_flags(subsystem_flags);
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->set_flags != nullptr) {
                subsystem->set_flags(subsystem_flags);
            }
        }
    }

    if (!SDL_Init(subsystem_flags)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // TODO make this configurable
    SDL_SetAppMetadata("Umgebung Application", "1.0", "de.dennisppaul.umgebung.application");

    /* 3. initialize graphics */
    /* - init */
    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->init != nullptr) {
            if (!umgebung::subsystem_graphics->init()) {
                SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
            }
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->init != nullptr) {
                if (!subsystem->init()) {
                    umgebung::warning("Couldn't initialize subsystem.");
                }
            }
        }
    }

    if (umgebung::enable_graphics) {
        if (umgebung::subsystem_graphics != nullptr) {
            if (umgebung::subsystem_graphics->create_graphics != nullptr) {
                umgebung::g = umgebung::subsystem_graphics->create_graphics(umgebung::render_to_buffer);
            }
        }
    }

    if (umgebung::enable_audio) {
        if (umgebung::subsystem_audio != nullptr) {
            if (umgebung::subsystem_audio->create_audio != nullptr) {
                // NOTE fill in the values from `Umgebung.h`
                umgebung::AudioUnitInfo _audio_unit_info;
                // _audio_unit_info.unique_id       = 0; // NOTE set by subsystem
                _audio_unit_info.input_buffer       = nullptr;
                _audio_unit_info.input_channels     = umgebung::input_channels;
                _audio_unit_info.output_buffer      = nullptr;
                _audio_unit_info.output_channels    = umgebung::output_channels;
                _audio_unit_info.buffer_size        = umgebung::audio_buffer_size;
                _audio_unit_info.sample_rate        = umgebung::sample_rate;
                _audio_unit_info.input_device_id    = umgebung::audio_input_device_id;
                _audio_unit_info.input_device_name  = umgebung::audio_input_device_name;
                _audio_unit_info.output_device_id   = umgebung::audio_output_device_id;
                _audio_unit_info.output_device_name = umgebung::audio_output_device_name;
                umgebung::a                         = umgebung::subsystem_audio->create_audio(&_audio_unit_info);
            }
        }
    }

    umgebung::initialized = true;

    /* - setup_pre */

    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->setup_pre != nullptr) {
            umgebung::subsystem_graphics->setup_pre();
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->setup_pre != nullptr) {
                subsystem->setup_pre();
            }
        }
    }

    // TODO feed back the values to the global variables
    //      NEED TO find a good place to do this … also for audio
    //      values should be updated before `setup()` is called:
    if (umgebung::g != nullptr && umgebung::enable_graphics) {
        umgebung::width              = umgebung::g->width;
        umgebung::height             = umgebung::g->height;
        umgebung::framebuffer_width  = umgebung::g->framebuffer.width;
        umgebung::framebuffer_height = umgebung::g->framebuffer.height;
        // TODO pixelDensity
    }
    if (umgebung::a != nullptr && umgebung::enable_audio) {
        // NOTE copy values back to global variables after initialization … a bit hackish but well.
        umgebung::audio_input_buffer       = umgebung::a->input_buffer;
        umgebung::input_channels           = umgebung::a->input_channels;
        umgebung::audio_output_buffer      = umgebung::a->output_buffer;
        umgebung::output_channels          = umgebung::a->output_channels;
        umgebung::audio_buffer_size        = umgebung::a->buffer_size;
        umgebung::sample_rate              = umgebung::a->sample_rate;
        umgebung::audio_input_device_id    = umgebung::a->input_device_id;
        umgebung::audio_input_device_name  = umgebung::a->input_device_name;
        umgebung::audio_output_device_id   = umgebung::a->output_device_id;
        umgebung::audio_output_device_name = umgebung::a->output_device_name;
    }

    setup();

    /* - setup_post */

    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->setup_post != nullptr) {
            umgebung::subsystem_graphics->setup_post();
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->setup_post != nullptr) {
                subsystem->setup_post();
            }
        }
    }

    umgebung::lastFrameTime = std::chrono::high_resolution_clock::now();

    return SDL_APP_CONTINUE;
}

static void handle_event(const SDL_Event& event, bool& fAppIsRunning, bool& fMouseIsPressed, bool& fWindowIsResized) {
    // imgui_processevent(event);

    // generic sdl event handler
    sdl_event(event);

    switch (event.type) {
        // case SDL_EVENT_WINDOW_EVENT:
        //     if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
        //         fAppIsRunning = false;
        //     } else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && event.window.windowID == 1) {
        //         fWindowIsResized = true;
        //     }
        //     break;
        case SDL_EVENT_WINDOW_RESIZED:
            // TODO implement window resize … how will the subsystems be updated?
            umgebung::warning("TODO window resized subsystem needs to be update …");
            windowResized(-1, -1);
            fWindowIsResized = true;
            break;
        case SDL_EVENT_QUIT:
            fAppIsRunning = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            umgebung::key = static_cast<int>(event.key.key);
            if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                fAppIsRunning = false;
            } else {
                // if (!imgui_is_keyboard_captured()) {
                keyPressed();
                // }
            }
            break;
        case SDL_EVENT_KEY_UP:
            // if (imgui_is_keyboard_captured()) { break; }
            umgebung::key = static_cast<int>(event.key.key);
            keyReleased();

            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            // if (imgui_is_mouse_captured()) { break; }
            umgebung::mouseButton = event.button.button;
            fMouseIsPressed       = true;
            mousePressed();
            umgebung::is_mouse_pressed = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            // if (imgui_is_mouse_captured()) { break; }
            fMouseIsPressed       = false;
            umgebung::mouseButton = -1;
            mouseReleased();
            umgebung::is_mouse_pressed = false;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            // if (imgui_is_mouse_captured()) { break; }
            umgebung::pmouseX = umgebung::mouseX;
            umgebung::pmouseY = umgebung::mouseY;
            umgebung::mouseX  = static_cast<float>(event.motion.x);
            umgebung::mouseY  = static_cast<float>(event.motion.y);

            if (fMouseIsPressed) {
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

// TODO ugly hack … this needs to be handled differently
static bool _app_is_running    = true;
static bool _mouse_is_pressed  = false;
static bool _window_is_resized = false;

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->event != nullptr) {
                subsystem->event(event);
            }
        }
    }

    // if (sdl_event(event)) {
    //     if (event->type == SDL_EVENT_QUIT) {
    //         return SDL_APP_SUCCESS;
    //     }
    //     return SDL_APP_CONTINUE;
    // }

    /*
     * 1. mouse events
     * 2. key events
     * 3. quit events
     */
    handle_event(*event, _app_is_running, _mouse_is_pressed, _window_is_resized);
    if (!_app_is_running) {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

static void handle_draw() {
    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->draw_pre != nullptr) {
            umgebung::subsystem_graphics->draw_pre();
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->draw_pre != nullptr) {
                subsystem->draw_pre();
            }
        }
    }

    draw();

    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->draw_post != nullptr) {
            umgebung::subsystem_graphics->draw_post();
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->draw_post != nullptr) {
                subsystem->draw_post();
            }
        }
    }
}

using namespace std::chrono;

SDL_AppResult SDL_AppIterate(void* appstate) {
    const high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();
    const auto                              frameDuration    = duration_cast<duration<double>>(currentFrameTime - umgebung::lastFrameTime);
    const double                            frame_duration   = frameDuration.count();

    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->loop != nullptr) {
            umgebung::subsystem_graphics->loop();
        }
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->loop != nullptr) {
                subsystem->loop();
            }
        }
    }

    if (frame_duration >= umgebung::target_frame_duration) {
        handle_draw();

        if (frame_duration == 0) {
            umgebung::frameRate = 1;
        } else {
            umgebung::frameRate = static_cast<float>(1.0 / frame_duration);
        }

        umgebung::frameCount++;
        umgebung::lastFrameTime = currentFrameTime;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    // NOTE 1. call `void umgebung::shutdown()`(?)
    //      2. clean up subsytems e.g audio, graphics, ...
    if (umgebung::subsystem_graphics != nullptr) {
        if (umgebung::subsystem_graphics->shutdown != nullptr) {
            if (umgebung::subsystem_graphics != nullptr) {
                umgebung::subsystem_graphics->shutdown();
            }
        }
        if (umgebung::handle_subsystem_graphics_cleanup) {
            delete umgebung::subsystem_graphics;
        }
        umgebung::subsystem_graphics = nullptr;
    }

    for (const umgebung::Subsystem* subsystem: umgebung::subsystems) {
        if (subsystem != nullptr) {
            if (subsystem->shutdown != nullptr) {
                subsystem->shutdown();
            }
            // NOTE custom subsystems must be cleaned by client
            // delete subsystem;
        }
    }

    // NOTE clean up audio subsystem if created internally
    if (umgebung::subsystem_audio != nullptr) {
        if (umgebung::handle_subsystem_audio_cleanup) {
            delete umgebung::subsystem_audio;
        }
        umgebung::subsystem_audio = nullptr;
    }

    umgebung::subsystems.clear();

    shutdown();
    SDL_Quit();
}
