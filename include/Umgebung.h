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

#pragma once

#include <iostream>

#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 2048
#define NUMBER_OF_INPUT_CHANNELS 1
#define NUMBER_OF_OUTPUT_CHANNELS 2
#define DEFAULT_AUDIO_DEVICE (-1)
#define DEFAULT (-1)
#define DEFAULT_WINDOW_WIDTH 1024
#define DEFAULT_WINDOW_HEIGHT 768
#define DEFAULT_WINDOW_TITLE "Umgebung"
#ifndef UMGEBUNG_WINDOW_TITLE
#define UMGEBUNG_WINDOW_TITLE DEFAULT_WINDOW_TITLE
#endif

extern int audio_input_device;
extern int audio_output_device;
extern int monitor; // @development TOOD this always switches to fullscreen
extern int antialiasing;

extern int width;
extern int height;
extern float mouseX;
extern float mouseY;
extern float pmouseX;
extern float pmouseY;
#define LEFT 0;
#define RIGHT 1;
#define MIDDLE 2;
extern int mouseButton;
extern int key;
extern int frameCount;
extern float frameRate;
extern bool enable_retina_support; // @development maybe implement as `HINT(ENABLE_RETINA_SUPPORT, true)`

#include "UmgebungDraw.h"
#include "UmgebungSketch.h"
#include "UmgebungFunctions.h"