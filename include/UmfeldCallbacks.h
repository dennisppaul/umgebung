/*
 * Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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
#include <vector>

#include "UmfeldDefines.h"
#include "Subsystems.h"
#include "PAudio.h"

WEAK void arguments(const std::vector<std::string>& args);
WEAK void settings();
WEAK void setup();
WEAK void draw();

WEAK void keyPressed();
WEAK void keyReleased();
WEAK void mousePressed();
WEAK void mouseReleased();
WEAK void mouseDragged();
WEAK void mouseMoved();
WEAK void mouseWheel(float x, float y);

WEAK void windowResized(int width, int height);

/* --- additional callbacks --- */

WEAK void audioEvent();
WEAK void audioEvent(const umfeld::PAudio& device);

WEAK void post();
WEAK void shutdown();
WEAK void dropped(const char* dropped_filedir);
WEAK bool sdl_event(const SDL_Event& event);
