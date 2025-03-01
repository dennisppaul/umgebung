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
#include "UmgebungSubsystems.h"
#include "UmgebungFunctionsAdditional.h"
#include "UmgebungFunctions.h"
#include "UmgebungFunctionsPGraphics.h"

UMGEBUNG_NAMESPACE_BEGIN

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

/* public variables */

inline float width              = 1024;
inline float height             = 768;
inline float framebuffer_width  = width;
inline float framebuffer_height = height;
inline float frameCount         = 0;
inline float frameRate          = 60;
inline int   key                = 0;
inline int   mouseButton        = DEFAULT;
inline bool  isMousePressed     = false;
inline float mouseX             = 0;
inline float mouseY             = 0;
inline float pmouseX            = 0;
inline float pmouseY            = 0;
inline int   pixelHeight        = 1;
inline int   pixelWidth         = 1;

/* public variables *mainly for internal use* */

inline SubsystemGraphics*      subsystem_graphics = nullptr;
inline SubsystemAudio*         subsystem_audio    = nullptr;
inline std::vector<Subsystem*> subsystems;

// TODO should this go into `UmgebungFunctionsAdditional`

bool            is_initialized();
std::string     get_window_title();
void            set_frame_rate(float fps);
SDL_WindowFlags get_SDL_WindowFlags(SDL_WindowFlags& flags);

UMGEBUNG_NAMESPACE_END
