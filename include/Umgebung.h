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
#include "UmgebungPGraphicsInterface.h"

UMGEBUNG_NAMESPACE_BEGIN

inline bool                    always_on_top       = false;
inline int                     antialiasing        = DEFAULT;
inline int                     audio_input_device  = DEFAULT_AUDIO_DEVICE;
inline int                     audio_output_device = DEFAULT_AUDIO_DEVICE;
inline bool                    borderless          = false;
inline bool                    retina_support      = true;
inline int                     width               = 1024;
inline int                     height              = 768;
inline int                     framebuffer_width   = width;
inline int                     framebuffer_height  = height;
inline bool                    fullscreen          = false;
inline int                     frameCount          = 0;
inline float                   frameRate           = 60;
inline int                     key                 = 0;
inline int                     display             = DEFAULT;
inline int                     mouseButton         = DEFAULT;
inline bool                    isMousePressed      = false;
inline float                   mouseX              = 0;
inline float                   mouseY              = 0;
inline float                   pmouseX             = 0;
inline float                   pmouseY             = 0;
inline int                     pixelHeight         = 1;
inline int                     pixelWidth          = 1;
inline bool                    resizable           = false;
inline bool                    vsync               = false;
inline SubsystemGraphics*      subsystem_graphics  = nullptr;
inline SubsystemAudio*         subsystem_audio     = nullptr;
inline std::vector<Subsystem*> subsystems;

bool        is_initialized();
std::string get_window_title();
void        set_frame_rate(float fps);

UMGEBUNG_NAMESPACE_END
