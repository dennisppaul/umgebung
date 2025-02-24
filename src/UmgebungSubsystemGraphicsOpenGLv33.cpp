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

UMGEBUNG_NAMESPACE_BEGIN
static bool       init(int width, int height);
static void       setup_pre();
static void       setup_post();
static void       draw_pre();
static void       draw_post();
static void       shutdown();
static void       set_flags(uint32_t& subsystem_flags);
static PGraphics* create_graphics();

static bool init(int width, int height) {
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("SDL3 + OpenGL",
                                          width, height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    SDL_ShowWindow(window);

    const SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        return false;
    }
    return true;
}

static void setup_pre() {}

static void setup_post() {}

static void draw_pre() {}

static void draw_post() {}

static void shutdown() {}

static void set_flags(uint32_t& subsystem_flags) {}

static PGraphics* create_graphics() {
    return nullptr;
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
