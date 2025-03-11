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

#pragma once

#include "UmgebungDefines.h"
#include "UmgebungConstants.h"
#include "UmgebungCallbacks.h"
#include "UmgebungFunctions.h"
#include "UmgebungFunctionsGraphics.h"
#include "UmgebungSubsystems.h"

namespace umgebung {

    /* public variables *for initialization only*  */

    inline bool always_on_top       = false;
    inline int  antialiasing        = DEFAULT;
    inline int  audio_input_device  = DEFAULT_AUDIO_DEVICE;
    inline int  audio_output_device = DEFAULT_AUDIO_DEVICE;
    inline bool borderless          = false;
    inline int  display             = DEFAULT;
    inline bool fullscreen          = false;
    inline bool resizable           = false;
    inline bool retina_support      = true;
    inline bool vsync               = false;
    inline bool render_to_buffer    = false;

    /* public variables */

    inline PGraphics* g                  = nullptr; // graphics
    inline bool       enable_graphics    = true;
    inline float      width              = 1024;
    inline float      height             = 768;
    inline float      framebuffer_width  = width; // TODO maybe i can clean this up at some point … it s always `width*pixelDensity`
    inline float      framebuffer_height = height;
    inline float      frameCount         = 0;
    inline float      frameRate          = 60;
    inline int        key                = 0; // events
    inline int        mouseButton        = DEFAULT;
    inline bool       is_mouse_pressed   = false;
    inline float      mouseX             = 0;
    inline float      mouseY             = 0;
    inline float      pmouseX            = 0;
    inline float      pmouseY            = 0;
    inline int        pixelHeight        = 1;
    inline int        pixelWidth         = 1;
    inline PAudio*    a                  = nullptr; // audio
    inline bool       enable_audio       = true;
    inline int        audio_device_id    = DEFAULT;
    inline int        input_channels     = 0; // TODO populate with default values
    inline int        output_channels    = 0;
    inline int        audio_buffer_size  = 0;
    inline int        sample_rate        = 0;
    inline int        audio_format       = 0;

    // NOTE just a quick reflection on the above: `PGraphics* g` as well as `PAudio* a` represent
    //      the default or main device infrastructure. the properties ( e.g `width` or `sample_rate` )
    //      represent values used for configuration at application start. they should be up to date
    //      while the application is running but are not guaranteed to be. also the properties are
    //      are redundant as the *actual* values are stored in the respective device objects. it is
    //      not good design but reflects the processing.org idiom.

    /* public variables *mainly for internal use* */

    inline SubsystemGraphics*      subsystem_graphics = nullptr;
    inline SubsystemAudio*         subsystem_audio    = nullptr;
    inline std::vector<Subsystem*> subsystems;

    // TODO should this go into `UmgebungFunctionsAdditional`

    bool            is_initialized();
    std::string     get_window_title();
    void            set_frame_rate(float fps);
    SDL_WindowFlags get_SDL_WindowFlags(SDL_WindowFlags& flags);

} // namespace umgebung
