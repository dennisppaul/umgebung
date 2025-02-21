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

#include "PApplet.h"

using namespace umgebung;

// TODO add `void settings()`
// TODO maybe add some events like mouse or key events

extern "C" {
PApplet*    create_umgebung();
void        destroy_umgebung(PApplet* application);
void        settings(PApplet* application);
void        setup(PApplet* application);
void        draw(PApplet* application);
void        beat(PApplet* application, uint32_t beat_count);
void        audioblock(PApplet* application, float** input, float** output, int length);
const char* name(PApplet* application);
void        event(PApplet* application, float*, uint32_t);
}
