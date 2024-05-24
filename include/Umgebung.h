/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2023 Dennis P Paul.
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

// TODO make audio settings configurable
// TODO what if `audio_input_channels` equals `0`? is microphone access still requested?
// TODO fix warnings on RPI
// TODO consider using namespace here `namespace umgebung {}` for entire project

#include <iostream>
#include <cmath>

#if !defined(DISABLE_GRAPHICS) || !defined(DISABLE_AUDIO)

#include <SDL2/SDL.h>

#endif

#ifndef DISABLE_GRAPHICS
#define APP_WINDOW SDL_Window
#else
// @TODO maybe find a better solution then `void`
#define APP_WINDOW void
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

#define USE_CURRENT_OPENGL FALSE // on macOS currently VERSION 3.3 // TODO does not work currently
#ifndef RENDER_INTO_FRAMEBUFFER
#define RENDER_INTO_FRAMEBUFFER TRUE // this is required when not clearing the framebuffer each frame
#endif                               // RENDER_INTO_FRAMEBUFFER

namespace umgebung {
#define DEFAULT_AUDIO_SAMPLE_RATE 48000     // TODO make this configurable
#define DEFAULT_FRAMES_PER_BUFFER 2048      // TODO make this configurable
#define DEFAULT_NUMBER_OF_INPUT_CHANNELS 1  // TODO make this configurable
#define DEFAULT_NUMBER_OF_OUTPUT_CHANNELS 2 // TODO make this configurable
    //#define DEFAULT_AUDIO_DEVICE              (-1)
    static const int DEFAULT_AUDIO_DEVICE = -1;
#define DEFAULT (-1)
#define DEFAULT_WINDOW_WIDTH 1024
#define DEFAULT_WINDOW_HEIGHT 768
#define DEFAULT_WINDOW_TITLE "Umgebung"
#ifndef UMGEBUNG_WINDOW_TITLE // can be set in `CMakeLists.txt`
#define UMGEBUNG_WINDOW_TITLE DEFAULT_WINDOW_TITLE
#endif

    extern int  audio_input_device;
    extern int  audio_output_device;
    extern int  audio_input_channels;
    extern int  audio_output_channels;
    extern int  monitor; // @development TOOD this always switches to fullscreen
    extern int  antialiasing;
    extern bool resizable;
    extern bool enable_retina_support; // @development maybe implement as `HINT(ENABLE_RETINA_SUPPORT, true)`
    extern bool headless;
    extern bool no_audio;
} // namespace umgebung

#include "UmgebungConstants.h"

#include "PVector.h"
#include "PFont.h"
#include "PImage.h"
#include "PGraphics.h"
#include "PShape.h"

#include "UmgebungFunctions.h"
#include "PApplet.h"

/* Graphics Prototypes */
namespace umgebung {
    void set_graphics_context(PApplet* applet);

    APP_WINDOW* init_graphics(int width, int height, const char* title);

    void handle_setup(APP_WINDOW* window);

    void handle_draw(APP_WINDOW* window);

    void handle_event(const SDL_Event& event, bool& fAppIsRunning, bool& fMouseIsPressed);

    void handle_shutdown(APP_WINDOW* window);
} // namespace umgebung
