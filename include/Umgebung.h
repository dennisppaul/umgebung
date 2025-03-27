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

#include <vector>
#include <string>

#include "UmgebungDefines.h"
#include "UmgebungConstants.h"
#include "UmgebungCallbacks.h"
#include "UmgebungFunctions.h"
#include "UmgebungFunctionsGraphics.h"
#include "Subsystems.h"

namespace umgebung {

    /* public variables *for initialization only*  */

    /* --- audio  --- */
    inline bool enable_audio           = false;
    inline int  audio_unique_device_id = 0x0010;
    // inline int        audio_format       = 0; // TODO currently only supporting F32

    /* --- graphics --- */
    inline bool enable_graphics  = false;
    inline bool always_on_top    = false;
    inline int  antialiasing     = DEFAULT;
    inline bool borderless       = false;
    inline int  display          = DEFAULT;
    inline bool fullscreen       = false;
    inline bool resizable        = false;
    inline bool retina_support   = true;
    inline bool vsync            = false;
    inline bool render_to_buffer = false;

    /* --- libraries + events --- */
    inline bool enable_libraries = true;
    inline bool enable_events    = true;

    /* public variables ( updated by system ) */

    /* --- audio  --- */
    inline PAudio*     a                        = nullptr;
    inline float*      audio_input_buffer       = nullptr;
    inline int         input_channels           = DEFAULT_INPUT_CHANNELS;
    inline float*      audio_output_buffer      = nullptr;
    inline int         output_channels          = DEFAULT_OUTPUT_CHANNELS;
    inline int         audio_buffer_size        = 0;
    inline int         sample_rate              = 0;
    inline int         audio_input_device_id    = DEFAULT_AUDIO_DEVICE;
    inline std::string audio_input_device_name  = DEFAULT_AUDIO_DEVICE_NAME;
    inline int         audio_output_device_id   = DEFAULT_AUDIO_DEVICE;
    inline std::string audio_output_device_name = DEFAULT_AUDIO_DEVICE_NAME;

    /* --- graphics --- */
    inline PGraphics* g              = nullptr;
    inline float      width          = DEFAULT_WINDOW_WIDTH;
    inline float      height         = DEFAULT_WINDOW_HEIGHT;
    inline int        frameCount     = 0;
    inline float      frameRate      = DEFAULT_FRAME_RATE;
    inline int        key            = 0; // events
    inline bool       isKeyPressed   = false;
    inline int        mouseButton    = DEFAULT;
    inline bool       isMousePressed = false;
    inline float      mouseX         = 0;
    inline float      mouseY         = 0;
    inline float      pmouseX        = 0;
    inline float      pmouseY        = 0;

    /* public variables *mainly for internal use* */

    inline bool                    use_esc_key_to_quit = true;
    inline std::vector<Subsystem*> subsystems;
    inline SubsystemGraphics*      subsystem_graphics   = nullptr;
    inline SubsystemAudio*         subsystem_audio      = nullptr;
    inline Subsystem*              subsystem_libraries  = nullptr;
    inline Subsystem*              subsystem_hid_events = nullptr;
} // namespace umgebung
