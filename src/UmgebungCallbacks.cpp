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

#define ENABLE_UMGEBUNG_CALLBACK_LOGGING 0

#if ENABLE_UMGEBUNG_CALLBACK_LOGGING
#define LOG_MSG(msg) SDL_Log(msg)
#else
#define LOG_MSG(msg) ((void) 0) // Does nothing when logging is disabled
#endif

WEAK void arguments(const std::vector<std::string>& args) { LOG_MSG("default arguments"); }
WEAK void settings() { LOG_MSG("default settings"); }
WEAK void setup() { LOG_MSG("default setup"); }
WEAK void draw() { LOG_MSG("default draw"); }
WEAK void shutdown() { LOG_MSG("default shutdown"); }

WEAK void keyPressed() { LOG_MSG("default keyPressed"); }
WEAK void keyReleased() { LOG_MSG("default keyReleased"); }
WEAK void mousePressed() { LOG_MSG("default mousePressed"); }
WEAK void mouseReleased() { LOG_MSG("default mouseReleased"); }
WEAK void mouseDragged() { LOG_MSG("default mouseDragged"); }
WEAK void mouseMoved() { LOG_MSG("default mouseMoved"); }
WEAK void mouseWheel(const float x, const float y) { LOG_MSG("default mouseWheel"); }

WEAK void dropped(const char* dropped_filedir) { LOG_MSG("default dropped"); }
WEAK bool sdl_event(const SDL_Event& event) { LOG_MSG("sdl event"); return false; }
WEAK void windowResized(int width, int height) {}
WEAK void audioEvent(const umgebung::AudioUnitInfo& device) {}
WEAK void audioEvent() { /* NOTE same as above but for default audio device */ }