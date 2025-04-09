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

namespace umfeld {
    class AudioSignal {
    public:
        float left; /* aka MONO */
        float right;

        AudioSignal() : left(0.0f), right(0.0f) {}
        explicit AudioSignal(float signal_mono) : left(signal_mono), right(signal_mono) {}
        AudioSignal(float signal_left, float signal_right) : left(signal_left), right(signal_right) {}

        void set(float signal_mono) {
            left  = signal_mono;
            right = signal_mono;
        }

        void set(float signal_left, float signal_right) {
            left  = signal_left;
            right = signal_right;
        }
    };
} // namespace umfeld