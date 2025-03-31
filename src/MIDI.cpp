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

#include "MIDI.h"

WEAK void midi_message(const std::vector<unsigned char>& message) {}
WEAK void note_off(int channel, int note) {}
WEAK void note_on(int channel, int note, int velocity) {}
WEAK void control_change(int channel, int control, int value) {}
WEAK void program_change(int channel, int program) {}
WEAK void pitch_bend(int channel, int value) {}
WEAK void sys_ex(const std::vector<unsigned char>& message) {}