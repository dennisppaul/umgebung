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

#include "Umgebung.h"
#include "PGraphicsOpenGLv33.h"

UMGEBUNG_NAMESPACE_BEGIN
static void       setup_pre();
static void       setup_post();
static void       draw_pre();
static void       draw_post();
static void       shutdown();
static void       set_flags(uint32_t& subsystem_flags);
static PGraphics* create_graphics();

static SDL_Window*   window     = nullptr;
static SDL_GLContext gl_context = nullptr;

static SDL_WindowFlags get_SDL_WindowFlags() {
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
    SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
    flags |= SDL_WINDOW_HIDDEN; // always hide window
    // flags |= SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;
    flags |= fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    flags |= borderless ? SDL_WINDOW_BORDERLESS : 0;
    flags |= resizable ? SDL_WINDOW_RESIZABLE : 0;
    flags |= retina_support ? SDL_WINDOW_HIGH_PIXEL_DENSITY : 0;
    flags |= always_on_top ? SDL_WINDOW_ALWAYS_ON_TOP : 0;
    return flags;
}

static void set_default_graphics_state() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void center_display() {
    int mDisplayLocation;
    if (display == DEFAULT) {
        mDisplayLocation = SDL_WINDOWPOS_CENTERED;
    } else {
        int mNumDisplays = 0;
        SDL_GetDisplays(&mNumDisplays);
        if (display >= mNumDisplays) {
            error("display index '", display, "' out of range. ",
                  mNumDisplays, " display", mNumDisplays > 1 ? "s are" : " is",
                  " available. using default display.");
            mDisplayLocation = SDL_WINDOWPOS_CENTERED;
        } else {
            mDisplayLocation = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
        }
    }
    SDL_SetWindowPosition(window, mDisplayLocation, mDisplayLocation);
}

static bool init() {
    /* setup opengl */

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    if (antialiasing > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
    }

    /* window */

    window = SDL_CreateWindow(get_window_title().c_str(),
                              umgebung::width,
                              umgebung::height,
                              get_SDL_WindowFlags());
    if (window == nullptr) {
        error("Couldn't create window: ", SDL_GetError());
        return false;
    }

    center_display();

    /* create opengl context */

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        error("Couldn't create OpenGL context: ", SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);

    /* display window */

    SDL_ShowWindow(window);
    SDL_RaiseWindow(window); // TODO see if this causes any issues

    set_default_graphics_state();

    /* initialize GLEW */

    glewExperimental            = GL_TRUE;
    const GLenum glewInitResult = glewInit();
    if (GLEW_OK != glewInitResult) {
        error("problem initializing GLEW: ", glewGetErrorString(glewInitResult));
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        return false;
    }

    return true;
}

static void setup_pre() {}

static void setup_post() {}

static void draw_pre() {}

static void draw_post() {
    SDL_GL_SwapWindow(window);
}

static void shutdown() {}

static void set_flags(uint32_t& subsystem_flags) {}

static PGraphics* create_graphics() {
    return new PGraphicsOpenGLv33();
}
UMGEBUNG_NAMESPACE_END

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_openglv33() {
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
