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

#include <chrono>

class StopWatch {
public:
    using Clock = std::chrono::high_resolution_clock;

    void start() {
        start_time = Clock::now();
    }

    double elapsed_seconds() const {
        return std::chrono::duration<double>(Clock::now() - start_time).count();
    }

    double elapsed_milliseconds() const {
        return std::chrono::duration<double, std::milli>(Clock::now() - start_time).count();
    }

    double elapsed_microseconds() const {
        return std::chrono::duration<double, std::micro>(Clock::now() - start_time).count();
    }

    double elapsed_nanoseconds() const {
        return std::chrono::duration<double, std::nano>(Clock::now() - start_time).count();
    }

private:
    Clock::time_point start_time = Clock::now();
};