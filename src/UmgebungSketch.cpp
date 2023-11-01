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

#include "Umgebung.h"
#include "UmgebungSketch.h"

WEAK void settings() {
    size(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    audio_devices(DEFAULT_AUDIO_DEVICE, DEFAULT_AUDIO_DEVICE);
    monitor = DEFAULT;
    antialiasing = DEFAULT;
}

WEAK void setup() {}

WEAK void draw() {}

WEAK void finish() {}

WEAK void audioblock(const float *input, float *output, unsigned long length) {}

WEAK void mouseMoved() {}

WEAK void mouseDragged() {}

WEAK void mousePressed() {}

WEAK void mouseReleased() {}

WEAK void keyPressed() {}

WEAK void keyReleased() {}
