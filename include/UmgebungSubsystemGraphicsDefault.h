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

#include "PGraphics.h"

bool umgebung_subsystem_graphics_default_init(int width, int height);

inline void umgebung_subsystem_graphics_default_setup_pre() { printf("Setup Pre\n"); }

inline void umgebung_subsystem_graphics_default_setup_post() { printf("Setup Post\n"); }

void umgebung_subsystem_graphics_default_draw_pre();

void umgebung_subsystem_graphics_draw_default_post();

inline void umgebung_subsystem_graphics_default_shutdown() { printf("Shutdown\n"); }

umgebung::PGraphics* create_graphics();

inline umgebung::SubsystemGraphics* create_graphics_subsystem() {
    auto* graphics       = new umgebung::SubsystemGraphics{};
    graphics->init       = umgebung_subsystem_graphics_default_init;
    graphics->setup_pre  = umgebung_subsystem_graphics_default_setup_pre;
    graphics->setup_post = umgebung_subsystem_graphics_default_setup_post;
    graphics->draw_pre   = umgebung_subsystem_graphics_default_draw_pre;
    graphics->draw_post  = umgebung_subsystem_graphics_draw_default_post;
    graphics->shutdown   = umgebung_subsystem_graphics_default_shutdown;
    return graphics;
}
