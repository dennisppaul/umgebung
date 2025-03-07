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

#include <SDL3/SDL.h>

#include "UmgebungDefines.h"
#include "PGraphics.h"

namespace umgebung {

struct Subsystem {
    bool (*init)();
    void (*setup_pre)();
    void (*setup_post)();
    void (*draw_pre)();
    void (*draw_post)();
    void (*shutdown)();
    void (*event)(SDL_Event* event);
};

struct SubsystemGraphics : Subsystem {
    void (*set_flags)(uint32_t& subsystem_flags);
    PGraphics* (*create_graphics)(bool render_to_offscreen);
};

struct SubsystemAudio : Subsystem {
    bool (*init)(int input_device,
                 int output_device,
                 int sample_rate,
                 int buffer_size,
                 int input_channels,
                 int output_channels,
                 int sample_format);
    void (*set_flags)(uint32_t& subsystem_flags);
};

inline SubsystemGraphics* (*create_subsystem_graphics)() = nullptr;
inline SubsystemAudio* (*create_subsystem_audio)()       = nullptr;

} // umgebung

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_default_2D();
umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_openglv20();
umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_openglv33();
