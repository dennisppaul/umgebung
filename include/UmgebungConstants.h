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

#include <string>

// ReSharper disable once CppUnusedIncludeDirective
#include "UmgebungDefines.h"

namespace umgebung {
    static constexpr int   DEFAULT                       = -1;
    static constexpr int   DEFAULT_WINDOW_WIDTH          = 1024;
    static constexpr int   DEFAULT_WINDOW_HEIGHT         = 768;
    static constexpr int   DEFAULT_FRAME_RATE            = 60;
    static constexpr int   DEFAULT_AUDIO_DEVICE          = -1;
    static constexpr auto  DEFAULT_AUDIO_DEVICE_NAME     = "";
    static constexpr auto  DEFAULT_AUDIO_DEVICE_NOT_USED = "NOOP";
    static constexpr int   DEFAULT_SAMPLE_RATE           = 48000;
    static constexpr int   DEFAULT_AUDIO_BUFFER_SIZE     = 1024;
    static constexpr int   DEFAULT_INPUT_CHANNELS        = 1;
    static constexpr int   DEFAULT_OUTPUT_CHANNELS       = 2;
    static constexpr int   DEFAULT_BYTES_PER_PIXELS      = 4;
    static constexpr int   AUDIO_DEVICE_FIND_BY_NAME     = -2;
    static constexpr int   AUDIO_DEVICE_NOT_FOUND        = -3;
    static constexpr int   AUDIO_UNIT_NOT_INITIALIZED    = -4;
    static constexpr int   BUFFER_SIZE_UNDEFINED         = -1;
    static constexpr int   TEXTURE_NOT_GENERATED         = -1;
    static constexpr int   TEXTURE_NOT_UPDATED           = -2;
    static constexpr int   TEXTURE_VALID_ID              = 1;
    static constexpr int   TEXTURE_NONE                  = 0;
    static constexpr float PI                            = 3.14159265358979323846f;
    static constexpr float HALF_PI                       = PI / 2;
    static constexpr float QUARTER_PI                    = PI / 4;
    static constexpr float TWO_PI                        = PI * 2;
    static constexpr float TAU                           = TWO_PI;
    static constexpr int   TRIANGLES                     = 0x00;
    static constexpr int   TRIANGLE_STRIP                = 0x01;
    static constexpr int   TRIANGLE_FAN                  = 0x02;
    static constexpr int   QUADS                         = 0x03;
    static constexpr int   QUAD_STRIP                    = 0x04;
    static constexpr int   POLYGON                       = 0x05;
    static constexpr int   POINTS                        = 0x06;
    static constexpr int   LINES                         = 0x07;
    static constexpr int   LINE_STRIP                    = 0x08;
    static constexpr int   LEFT                          = 1;
    static constexpr int   RIGHT                         = 2;
    static constexpr int   MIDDLE                        = 3;
    static constexpr bool  CLOSE                         = true;
    static constexpr bool  NOT_CLOSED                    = false;
    static constexpr int   CORNER                        = 0x00; // rectMode
    static constexpr int   CORNERS                       = 0x01;
    static constexpr int   CENTER                        = 0x02;
    static constexpr int   RADIUS                        = 0x03;
    enum StrokeJoin {
        BEVEL = 0x20,
        MITER,
        ROUND,
        NONE,
        BEVEL_FAST,
        MITER_FAST
    };
    static constexpr int SQUARE  = 0x26; // strokeCap
    static constexpr int PROJECT = 0x27;
    static constexpr int POINTED = 0x28;
    enum StrokeRenderMode {
        STROKE_RENDER_MODE_NATIVE = 0x30,
        STROKE_RENDER_MODE_TRIANGULATE_2D,
        STROKE_RENDER_MODE_TUBE_3D,
        STROKE_RENDER_MODE_BARYCENTRIC_SHADER,
        STROKE_RENDER_MODE_GEOMETRY_SHADER
    };
    static constexpr int POINT_RENDER_MODE_NATIVE         = 0x40;
    static constexpr int POINT_RENDER_MODE_TRIANGULATE    = 0x41;
    static constexpr int POLYGON_TRIANGULATION_FASTER     = 0x10;
    static constexpr int POLYGON_TRIANGULATION_BETTER     = 0x11;
    static constexpr int POLYGON_TRIANGULATION_MID        = 0x12;
    const std::string    SHADER_UNIFORM_MODEL_MATRIX      = "uModelMatrix";
    const std::string    SHADER_UNIFORM_VIEW_MATRIX       = "uViewMatrix";
    const std::string    SHADER_UNIFORM_PROJECTION_MATRIX = "uProjection";
    enum BlendMode {
        BLEND = 0xB0,
        ADD,
        SUBTRACT,
        LIGHTEST,
        DARKEST,
        MULTIPLY,
        SCREEN,
        EXCLUSION,
        REPLACE,
        DIFFERENCE, // not implemented
        OVERLAY,    // not implemented
        HARD_LIGHT, // not implemented
        SOFT_LIGHT, // not implemented
        DODGE,      // not implemented
        BURN        // not implemented
    };
    enum Hint {
        ENABLE_SMOOTH_LINES = 0xA0,
        DISABLE_SMOOTH_LINES,
        ENABLE_DEPTH_TEST,
        DISABLE_DEPTH_TEST
    };
} // namespace umgebung
