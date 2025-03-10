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

namespace umgebung {
    static constexpr int   DEFAULT_AUDIO_DEVICE                          = -1;
    static constexpr int   TEXTURE_NOT_GENERATED                         = -1;
    static constexpr int   TEXTURE_NOT_UPDATED                           = -2;
    static constexpr int   TEXTURE_VALID_ID                              = 1;
    static constexpr float PI                                            = 3.14159265358979323846f;
    static constexpr float HALF_PI                                       = PI / 2;
    static constexpr float QUARTER_PI                                    = PI / 4;
    static constexpr float TWO_PI                                        = PI * 2;
    static constexpr float TAU                                           = TWO_PI;
    static constexpr int   TRIANGLES                                     = 0x00;
    static constexpr int   TRIANGLE_STRIP                                = 0x01;
    static constexpr int   TRIANGLE_FAN                                  = 0x02;
    static constexpr int   QUADS                                         = 0x03;
    static constexpr int   QUAD_STRIP                                    = 0x04;
    static constexpr int   POLYGON                                       = 0x05;
    static constexpr int   POINTS                                        = 0x06;
    static constexpr int   LINES                                         = 0x07;
    static constexpr int   LINE_STRIP                                    = 0x08;
    static constexpr int   LEFT                                          = 0;
    static constexpr int   RIGHT                                         = 1;
    static constexpr int   MIDDLE                                        = 2;
    static constexpr bool  CLOSE                                         = true;
    static constexpr bool  NOT_CLOSED                                    = false;
    static constexpr int   CORNER                                        = 0x00; // rectMode
    static constexpr int   CORNERS                                       = 0x01;
    static constexpr int   CENTER                                        = 0x02;
    static constexpr int   RADIUS                                        = 0x03;
    static constexpr int   BEVEL                                         = 0x20; // strokeJoin
    static constexpr int   MITER                                         = 0x21;
    static constexpr int   ROUND                                         = 0x22;
    static constexpr int   NONE                                          = 0x23;
    static constexpr int   BEVEL_FAST                                    = 0x24;
    static constexpr int   MITER_FAST                                    = 0x25;
    static constexpr int   SQUARE                                        = 0x26; // strokeCap
    static constexpr int   PROJECT                                       = 0x27;
    static constexpr int   POINTED                                       = 0x28;
    static constexpr int   LINE_RENDER_MODE_NATIVE                       = 0x30; // line render mode
    static constexpr int   LINE_RENDER_MODE_TRIANGLES               = 0x31;
    static constexpr int   POLYGON_TRIANGULATION_FASTER                  = 0x10;
    static constexpr int   POLYGON_TRIANGULATION_GOOD                    = 0x11;
    static constexpr int   POLYGON_TRIANGULATION_BETTER                  = 0x12;
    static constexpr int   RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_NONE   = 0x00; // TODO deprecated, remove these
    static constexpr int   RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_ROUND  = 0x01;
    static constexpr int   RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_MITER  = 0x02;
    static constexpr int   RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_BEVEL  = 0x03;
    static constexpr int   RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_NATIVE = 0x04;
    static constexpr float RENDER_LINE_STRIP_AS_QUADS_MAX_ANGLE          = 0.01;
    static constexpr int   HINT_ENABLE_SMOOTH_LINES                      = 0xA0;
    static constexpr int   HINT_DISABLE_SMOOTH_LINES                     = 0xA1;
    static constexpr int   HINT_ENABLE_LINE_RENDERING_MODE_NATIVE        = 0xA2;
    static constexpr int   HINT_DISABLE_LINE_RENDERING_MODE_NATIVE       = 0xA3;
} // namespace umgebung
