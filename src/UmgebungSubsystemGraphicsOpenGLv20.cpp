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

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include "Umgebung.h"
#include "UmgebungPGraphicsInterface.h"
#include "UmgebungSubsystems.h"
#include "PGraphicsOpenGLv20.h"

UMGEBUNG_NAMESPACE_BEGIN

static SDL_Window*   window     = nullptr;
static SDL_GLContext gl_context = nullptr;

// TODO implement set_resizable and fullscreen() (at runtime) and is_minimized()
//    SDL_SetWindowFullscreen(window?!?, fullscreen_state ? SDL_WINDOW_FULLSCREEN: SDL_WINDOW_FULLSCREEN_DESKTOP);
//    SDL_SetWindowResizable(window?!?, SDL_TRUE);
//    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {}

// TODO these functions below should go into a more generic place
void set_window_position(const int x, const int y) {
    if (window == nullptr) {
        return;
    }
    SDL_SetWindowPosition(window, x, y);
}

void get_window_position(int& x, int& y) {
    if (window == nullptr) {
        return;
    }
    SDL_GetWindowPosition(window, &x, &y);
}

void set_window_size(const int width, const int height) {
    if (window == nullptr) {
        return;
    }
    SDL_SetWindowSize(window, width, height);
}

void get_window_size(int& width, int& height) {
    if (window == nullptr) {
        return;
    }
    SDL_GetWindowSize(window, &width, &height);
}

void center_display() {
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

/* --- private functions --- */

static void setup_pre();
static void setup_post();
static void draw_pre();
static void draw_post();

static void set_default_graphics_state() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static bool init() { // TODO maybe merge v2.0 & v3.3 they are identical except for SDL_GL_CONTEXT_PROFILE_MASK + SDL_GL_CONTEXT_MAJOR_VERSION + SDL_GL_CONTEXT_MINOR_VERSION
    /* setup opengl */

    // see https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_opengl3/main.cpp
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    if (antialiasing > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
    }

    /* window */

    SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
    window                = SDL_CreateWindow(get_window_title().c_str(),
                                             static_cast<int>(umgebung::width),
                                             static_cast<int>(umgebung::height),
                                             get_SDL_WindowFlags(flags));
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

static void setup_pre() {
    int current_framebuffer_width;
    int current_framebuffer_height;
    SDL_GetWindowSizeInPixels(window, &current_framebuffer_width, &current_framebuffer_height);
    framebuffer_width  = static_cast<float>(current_framebuffer_width);
    framebuffer_height = static_cast<float>(current_framebuffer_height);
    console("framebuffer size: ", framebuffer_width, " x ", framebuffer_height);

    pixelHeight = static_cast<int>(framebuffer_height / height);
    pixelWidth  = static_cast<int>(framebuffer_width / width);
    g->init(nullptr, static_cast<int>(framebuffer_width), static_cast<int>(framebuffer_height), 0, false);
    g->width  = static_cast<int>(width);
    g->height = static_cast<int>(height);
    set_default_graphics_state();
    draw_pre();
}

static void setup_post() {
    draw_post();
}

static void draw_pre() {
    set_default_graphics_state(); // TODO this is not nevessary as the FBO is drawn fullscreen

    glBindFramebuffer(GL_FRAMEBUFFER, g->framebuffer.id);

    glViewport(0, 0, g->framebuffer.width, g->framebuffer.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(1, -1, 1);
    glTranslatef(0, static_cast<float>(-height), 0);

    // const char* message = "Hello World!";
    // int         w = 0, h = 0;
    // float       x, y;
    // const float scale = 2.0f;
    //
    // /* Center the message and scale it up */
    // SDL_GetRenderOutputSize(renderer, &w, &h);
    // SDL_SetRenderScale(renderer, scale, scale);
    // x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    // y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;
    //
    // /* Draw the message */
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderClear(renderer);
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_RenderDebugText(renderer, x, y, message);
    // SDL_RenderPresent(renderer);
}

static void draw_post() {
    if (window == nullptr) {
        return;
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Render the FBO to the entire screen
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    glViewport(0, 0, g->framebuffer.width, g->framebuffer.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g->framebuffer.width, 0, g->framebuffer.height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, g->framebuffer.texture);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(0, 0);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(static_cast<float>(g->framebuffer.width), 0);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(static_cast<float>(g->framebuffer.width),
               static_cast<float>(g->framebuffer.height));
    glTexCoord2f(0.0, 1.0);
    glVertex2f(0, static_cast<float>(g->framebuffer.height));
    glEnd();

    // /* with padding of 10px */
    // glBegin(GL_QUADS);
    // glTexCoord2f(0.0, 0.0);
    // glVertex2f(10, 10);
    // glTexCoord2f(1.0, 0.0);
    // glVertex2f(g->framebuffer.width - 10, 10);
    // glTexCoord2f(1.0, 1.0);
    // glVertex2f(g->framebuffer.width - 10, g->framebuffer.height - 10);
    // glTexCoord2f(0.0, 1.0);
    // glVertex2f(10, g->framebuffer.height - 10);
    // glEnd();

    // /* 10% tiny view */
    // glBegin(GL_QUADS);
    // glTexCoord2f(0.0, 0.0);
    // glVertex2f(20, 20);
    // glTexCoord2f(1.0, 0.0);
    // glVertex2f(20 + static_cast<float>(g->framebuffer.width) * 0.1f, 20);
    // glTexCoord2f(1.0, 1.0);
    // glVertex2f(20 + static_cast<float>(g->framebuffer.width) * 0.1f,
    //            20 + static_cast<float>(g->framebuffer.height) * 0.1f);
    // glTexCoord2f(0.0, 1.0);
    // glVertex2f(20, 20 + static_cast<float>(g->framebuffer.height) * 0.1f);
    // glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopAttrib();

    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        umgebung::error("OpenGL error: ", error);
    }

    SDL_GL_SwapWindow(window);
}

static void shutdown() {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
}

static void set_flags(uint32_t& subsystem_flags) {
    subsystem_flags |= SDL_INIT_VIDEO;
}

static PGraphics* create_graphics(const int width, const int height) {
    return new PGraphicsOpenGLv20();
}
UMGEBUNG_NAMESPACE_END

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_openglv20() {
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
