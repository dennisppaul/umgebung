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

/**
 * PROCESSOR INTERFACE
 *
 * - [ ] float process()
 * - [x] float process(float)
 * - [ ] void process(AudioSignal&)
 * - [x] void process(float*, uint32_t)
 * - [ ] void process(float*, float*, uint32_t)
 */

#pragma once

#include <stdint.h>

#include "AudioUtilities.h"

namespace umfeld {
    class Filter {
    public:
        /* filter types. */
        static constexpr uint8_t LPF              = 0; /* low pass filter */
        static constexpr uint8_t HPF              = 1; /* High pass filter */
        static constexpr uint8_t BPF              = 2; /* band pass filter */
        static constexpr uint8_t NOTCH            = 3; /* Notch Filter */
        static constexpr uint8_t PEQ              = 4; /* Peaking band EQ filter */
        static constexpr uint8_t LSH              = 5; /* Low shelf filter */
        static constexpr uint8_t HSH              = 6; /* High shelf filter */
        static constexpr uint8_t NUM_FILTER_TYPES = 7;

        Filter(float sample_rate, bool use_fast_math = true) : __USE_FAST_TRIG(use_fast_math) {
            set(LPF, 0.0, 1000, 100, sample_rate);
        }

        Filter(uint8_t type,
               float   dbGain,
               float   center_frequency,
               float   bandwidth,
               float   sample_rate,
               bool    use_fast_math = true) : __USE_FAST_TRIG(use_fast_math) {
            set(type, dbGain, center_frequency, bandwidth, sample_rate);
        }

        float process(float sample) {
            const float r0     = biquad_a0 * sample;
            const float r1     = biquad_a1 * biquad_x1;
            const float r2     = biquad_a2 * biquad_x2;
            const float r3     = biquad_a3 * biquad_y1;
            const float r4     = biquad_a4 * biquad_y2;
            const float r5     = r0 + r1;
            const float r6     = r2 - r3 - r4;
            const float result = r5 + r6;

            /* shift x1 to x2, sample to x1. */
            biquad_x2 = biquad_x1;
            biquad_x1 = sample;

            /* shift y1 to y2, result to y1. */
            biquad_y2 = biquad_y1;
            biquad_y1 = result;

            return result;
        }

        void process(float*         signal_buffer,
                     const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                signal_buffer[i] = process(signal_buffer[i]);
            }
        }

        void set(uint8_t type,
                 float   dbGain, /* gain of filter */
                 float   center_frequency,
                 float   bandwidth, /* bandwidth in octaves */
                 float   sample_rate) {
            // float A, omega, sn, cs, alpha, beta;
            float a0, a1, a2, b0, b1, b2;

            const float A     = AudioUtilities::pow(10, dbGain / 40.0f);
            const float omega = (2.0 * FILTER_PI * center_frequency / sample_rate);
            float       sn;
            float       cs;
            float       alpha;
            float       beta;
            if (__USE_FAST_TRIG) {
                sn    = AudioUtilities::fast_sin(omega);
                cs    = AudioUtilities::fast_cos(omega);
                alpha = sn * AudioUtilities::fast_sinh(FILTER_LN2 / 2 * bandwidth * omega / sn);
                beta  = AudioUtilities::fast_sqrt(A + A);
            } else {
                sn    = AudioUtilities::sin(omega);
                cs    = AudioUtilities::cos(omega);
                alpha = sn * AudioUtilities::sinh(FILTER_LN2 / 2 * bandwidth * omega / sn);
                beta  = AudioUtilities::sqrt(A + A);
            }
            switch (type) {
                case LPF:
                    b0 = (1 - cs) / 2;
                    b1 = 1 - cs;
                    b2 = (1 - cs) / 2;
                    a0 = 1 + alpha;
                    a1 = -2 * cs;
                    a2 = 1 - alpha;
                    break;
                case HPF:
                    b0 = (1 + cs) / 2;
                    b1 = -(1 + cs);
                    b2 = (1 + cs) / 2;
                    a0 = 1 + alpha;
                    a1 = -2 * cs;
                    a2 = 1 - alpha;
                    break;
                case BPF:
                    b0 = alpha;
                    b1 = 0;
                    b2 = -alpha;
                    a0 = 1 + alpha;
                    a1 = -2 * cs;
                    a2 = 1 - alpha;
                    break;
                case NOTCH:
                    b0 = 1;
                    b1 = -2 * cs;
                    b2 = 1;
                    a0 = 1 + alpha;
                    a1 = -2 * cs;
                    a2 = 1 - alpha;
                    break;
                case PEQ:
                    b0 = 1 + (alpha * A);
                    b1 = -2 * cs;
                    b2 = 1 - (alpha * A);
                    a0 = 1 + (alpha / A);
                    a1 = -2 * cs;
                    a2 = 1 - (alpha / A);
                    break;
                case LSH:
                    b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
                    b1 = 2 * A * ((A - 1) - (A + 1) * cs);
                    b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
                    a0 = (A + 1) + (A - 1) * cs + beta * sn;
                    a1 = -2 * ((A - 1) + (A + 1) * cs);
                    a2 = (A + 1) + (A - 1) * cs - beta * sn;
                    break;
                case HSH:
                    b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
                    b1 = -2 * A * ((A - 1) + (A + 1) * cs);
                    b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
                    a0 = (A + 1) - (A - 1) * cs + beta * sn;
                    a1 = 2 * ((A - 1) - (A + 1) * cs);
                    a2 = (A + 1) - (A - 1) * cs - beta * sn;
                    break;
                default:
                    return;
            }

            /* precompute the coefficients. */
            biquad_a0 = b0 / a0;
            biquad_a1 = b1 / a0;
            biquad_a2 = b2 / a0;
            biquad_a3 = a1 / a0;
            biquad_a4 = a2 / a0;
        }

        void reset() {
            biquad_x1 = biquad_x2 = 0.0f;
            biquad_y1 = biquad_y2 = 0.0f;
        }

    private:
        static constexpr float FILTER_LN2 = 0.69314718055994530942;
        static constexpr float FILTER_PI  = 3.14159265358979323846;
        float                  biquad_a0, biquad_a1, biquad_a2, biquad_a3, biquad_a4 = 0;
        float                  biquad_x1, biquad_x2, biquad_y1, biquad_y2 = 0;
        const bool             __USE_FAST_TRIG;
    };

} // namespace umfeld
