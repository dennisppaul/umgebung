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

// TODO consider using namespace here `namespace umgebung {}`

namespace umgebung {
    static const float PI = 3.14159265358979323846f;

    static const float HALF_PI    = PI / 2;
    static const float QUARTER_PI = PI / 4;
    static const float TWO_PI     = PI * 2;
    static const float TAU        = TWO_PI;

    static const int TRIANGLES      = 0;
    static const int TRIANGLE_STRIP = 1;
    static const int TRIANGLE_FAN   = 2;
    static const int QUADS          = 3;
    static const int QUAD_STRIP     = 4;
    static const int POLYGON        = 5;
    static const int POINTS         = 6;
    static const int LINES          = 7;
    static const int LINE_STRIP     = 8;

    static const int LEFT   = 0;
    static const int RIGHT  = 1;
    static const int MIDDLE = 2;
} // namespace umgebung