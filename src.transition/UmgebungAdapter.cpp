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

#include <iostream>
#include "Umgebung.h"
#include "UmgebungAdapter.h"

using namespace umgebung;

#ifdef UMGEBUNG_ENABLE_ADAPTER

#define DEBUG_UMGEBUNG_ADAPTER 1

extern "C" {

void die() {
#if DEBUG_UMGEBUNG_ADAPTER
    std::cout << "UmgebungAdapter: application is nullptr" << std::endl;
#endif
}

PApplet*    create_umgebung() { return instance(); }
void        destroy_umgebung(PApplet* application) { delete application; }
void        settings(PApplet* application) { application != nullptr ? application->settings() : die(); }
void        setup(PApplet* application) { application != nullptr ? application->setup() : die(); }
void        draw(PApplet* application) { application != nullptr ? application->draw() : die(); }
void        beat(PApplet* application, uint32_t beat_count) { application != nullptr ? application->beat(beat_count) : die(); }
void        audioblock(PApplet* application, float** input, float** output, const int length) { application != nullptr ? application->audioblock(input, output, length) : die(); }
const char* name(PApplet* application) { return application != nullptr ? application->name() : "null"; }
void        event(PApplet* application, float* data, uint32_t length) {application != nullptr ? application->event(data, length) : die();}
}

#endif // UMGEBUNG_ENABLE_ADAPTER