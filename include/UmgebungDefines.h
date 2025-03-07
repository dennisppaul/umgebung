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

#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* --- DEFAULTS --- */

#define DEFAULT                           (-1)
#define DEFAULT_NUMBER_OF_INPUT_CHANNELS  1 // TODO make this configurable
#define DEFAULT_NUMBER_OF_OUTPUT_CHANNELS 2 // TODO make this configurable
#define DEFAULT_WINDOW_WIDTH              1024
#define DEFAULT_WINDOW_HEIGHT             768
#define DEFAULT_WINDOW_TITLE              "Umgebung"

#ifndef DEFAULT_AUDIO_SAMPLE_RATE
#define DEFAULT_AUDIO_SAMPLE_RATE 48000 // TODO make this configurable
#endif

#ifndef DEFAULT_FRAMES_PER_BUFFER
#define DEFAULT_FRAMES_PER_BUFFER 2048 // TODO make this configurable
#endif

/* --- UMGEBUNG_WINDOW_TITLE --- */

#ifndef UMGEBUNG_WINDOW_TITLE // can be set in `CMakeLists.txt` // TODO consider setting this in settings?
#define UMGEBUNG_WINDOW_TITLE DEFAULT_WINDOW_TITLE
#endif

#ifndef UMGEBUNG_PRINT_ERRORS
#define UMGEBUNG_PRINT_ERRORS TRUE
#endif
#ifndef UMGEBUNG_PRINT_WARNINGS
#define UMGEBUNG_PRINT_WARNINGS TRUE
#endif
#ifndef UMGEBUNG_PRINT_CONSOLE
#define UMGEBUNG_PRINT_CONSOLE TRUE
#endif

/* --- TOOLS --- */

#ifndef RGBA
#define RGBA(r, g, b, a) (((uint32_t) (a) << 24) | ((uint32_t) (b) << 16) | ((uint32_t) (g) << 8) | ((uint32_t) (r)))
#endif