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
    static constexpr float DEFAULT_CAMERA_FOV_RADIANS    = 1.04719755f; // glm::radians(60.f));
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
    static constexpr int   LEFT                          = 0x01;
    static constexpr int   RIGHT                         = 0x02;
    static constexpr int   MIDDLE                        = 0x03;
    static constexpr int   CENTER                        = 0x04;
    static constexpr bool  CLOSE                         = true;
    static constexpr bool  NOT_CLOSED                    = false;
    enum ShapeKind {
        TRIANGLES = 0x10,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        QUADS,
        QUAD_STRIP,
        POLYGON,
        POINTS,
        LINES,
        LINE_STRIP
    };
    enum RectMode {
        CORNER = 0x20,
        CORNERS,
        // CENTER,
        RADIUS
    };
    enum TextAlign {
        // LEFT = 0xB0, // x
        // CENTER,
        // RIGHT,
        TOP = 0xB0, // y
        BOTTOM,
        // CENTER,
        BASELINE
    };
    enum StrokeJoin {
        BEVEL = 0x30,
        MITER,
        ROUND,
        NONE,
        BEVEL_FAST,
        MITER_FAST
    };
    enum StrokeCap {
        SQUARE = 0x40,
        PROJECT,
        POINTED
    };
    enum StrokeRenderMode {
        STROKE_RENDER_MODE_NATIVE = 0x50,
        STROKE_RENDER_MODE_TRIANGULATE_2D,
        STROKE_RENDER_MODE_TUBE_3D,
        STROKE_RENDER_MODE_BARYCENTRIC_SHADER,
        STROKE_RENDER_MODE_GEOMETRY_SHADER
    };
    enum PointRenderMode {
        POINT_RENDER_MODE_NATIVE = 0x60,
        POINT_RENDER_MODE_TRIANGULATE
    };
    enum PolygonTriangulation {
        POLYGON_TRIANGULATION_FASTER = 0x70,
        POLYGON_TRIANGULATION_BETTER,
        POLYGON_TRIANGULATION_MID
    };
    enum BlendMode {
        BLEND = 0x80,
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
    enum RenderMode {
        RENDER_MODE_IMMEDIATE = 0x90,
        RENDER_MODE_BUFFERED,
        RENDER_MODE_SHAPE
    };
    enum Hint {
        ENABLE_SMOOTH_LINES = 0xA0,
        DISABLE_SMOOTH_LINES,
        ENABLE_DEPTH_TEST,
        DISABLE_DEPTH_TEST
    };
    enum Renderer {
        OPENGL_2_0 = 0xB0,
        OPENGL_3_3,
        OPENGL,
        OPENGL_ES_3_0, // iOS
        OPENGL_ES_3_1  // RPI4+5
    };

    const std::string SHADER_UNIFORM_MODEL_MATRIX      = "uModelMatrix";
    const std::string SHADER_UNIFORM_VIEW_MATRIX       = "uViewMatrix";
    const std::string SHADER_UNIFORM_PROJECTION_MATRIX = "uProjection";
} // namespace umgebung
