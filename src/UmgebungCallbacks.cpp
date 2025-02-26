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

#include <SDL3/SDL.h> // TODO remove this at some point

#include "UmgebungCallbacks.h"

WEAK void arguments(const std::vector<std::string>& args) { SDL_Log("default arguments"); }
WEAK void settings() { SDL_Log("default settings"); }
WEAK void setup() { SDL_Log("default setup"); }
WEAK void draw() { SDL_Log("default draw"); }
WEAK void shutdown() { SDL_Log("default shutdown"); }

WEAK void keyPressed() { SDL_Log("default keyPressed"); }
WEAK void keyReleased() { SDL_Log("default keyReleased"); }
WEAK void mousePressed() { SDL_Log("default mousePressed"); }
WEAK void mouseReleased() { SDL_Log("default mouseReleased"); }
WEAK void mouseDragged() { SDL_Log("default mouseDragged"); }
WEAK void mouseMoved() { SDL_Log("default mouseMoved"); }
WEAK void mouseWheel(const float x, const float y) { SDL_Log("default mouseWheel: %f, %f", x, y); }

WEAK void dropped(const char* dropped_filedir) { SDL_Log("default dropped: %s", dropped_filedir); }
WEAK bool sdl_event(SDL_Event* event) { return false; }