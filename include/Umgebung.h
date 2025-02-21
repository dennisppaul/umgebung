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
#include "UmgebungFunctionsAdditional.h"
#include "UmgebungFunctions.h"
#include "UmgebungPGraphicsInterface.h"

UMGEBUNG_NAMESPACE_BEGIN
typedef struct {
    bool (*init)(int width, int height);
    void (*setup_pre)();
    void (*setup_post)();
    void (*draw_pre)();
    void (*draw_post)();
    void (*shutdown)();
} SubsystemGraphics;

inline int                audio_input_device  = DEFAULT_AUDIO_DEVICE;
inline int                audio_output_device = DEFAULT_AUDIO_DEVICE;
inline int                width               = 1024;
inline int                height              = 768;
inline int                key                 = 0;
inline SubsystemGraphics* subsystem_graphics  = nullptr;
inline bool               initilized          = false;
UMGEBUNG_NAMESPACE_END
