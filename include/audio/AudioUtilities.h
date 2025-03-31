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

#include <cmath>
#include <math.h>
#include <stdint.h>
#include <limits>

#ifndef PI
#define PI M_PI
#endif

#ifndef TWO_PI
#define TWO_PI (M_PI * 2)
#endif

#ifndef HALF_PI
#define HALF_PI (M_PI / 2)
#endif

#ifndef KLANGWELLEN_WAVETABLE_INTERPOLATE_SAMPLES
#define KLANGWELLEN_WAVETABLE_INTERPOLATE_SAMPLES 1
#endif

namespace umgebung {
    enum Waveform {
        WAVEFORM_SINE,
        WAVEFORM_TRIANGLE,
        WAVEFORM_SAWTOOTH,
        WAVEFORM_SQUARE,
        WAVEFORM_NOISE,
        WAVEFORM_PULSE,
    };

    class AudioUtilities {
    public:
        static constexpr uint8_t BITS_PER_SAMPLE_16                    = 16;
        static constexpr uint8_t BITS_PER_SAMPLE_24                    = 24;
        static constexpr uint8_t BITS_PER_SAMPLE_32                    = 32;
        static constexpr uint8_t BITS_PER_SAMPLE_8                     = 8;
        static constexpr float   DEFAULT_ATTACK                        = 0.005f;
        static constexpr int     DEFAULT_AUDIO_DEVICE                  = -1;
        static constexpr uint8_t DEFAULT_BITS_PER_SAMPLE               = BITS_PER_SAMPLE_16;
        static constexpr float   DEFAULT_DECAY                         = 0.01f;
        static constexpr float   DEFAULT_FILTER_BANDWIDTH              = 100.0f;
        static constexpr float   DEFAULT_FILTER_FREQUENCY              = 1000.0f;
        static constexpr float   DEFAULT_RELEASE                       = 0.075f;
        static constexpr float   DEFAULT_SUSTAIN                       = 0.5f;
        static constexpr int     DEFAULT_WAVETABLE_SIZE                = 512;
        static constexpr uint8_t DISTORTION_HARD_CLIPPING              = 0;
        static constexpr uint8_t DISTORTION_FOLDBACK                   = 1;
        static constexpr uint8_t DISTORTION_FOLDBACK_SINGLE            = 2;
        static constexpr uint8_t DISTORTION_FULL_WAVE_RECTIFICATION    = 3;
        static constexpr uint8_t DISTORTION_HALF_WAVE_RECTIFICATION    = 4;
        static constexpr uint8_t DISTORTION_INFINITE_CLIPPING          = 5;
        static constexpr uint8_t DISTORTION_SOFT_CLIPPING_CUBIC        = 6;
        static constexpr uint8_t DISTORTION_SOFT_CLIPPING_ARC_TANGENT  = 7;
        static constexpr uint8_t DISTORTION_BIT_CRUSHING               = 8;
        static constexpr int     EVENT_UNDEFINED                       = -1;
        static constexpr int     EVENT_CHANNEL                         = 0;
        static constexpr int     EVENT_NOTE_ON                         = 0;
        static constexpr int     EVENT_NOTE                            = 1;
        static constexpr int     EVENT_NOTE_OFF                        = 1;
        static constexpr int     EVENT_CONTROLCHANGE                   = 2;
        static constexpr int     EVENT_PITCHBEND                       = 3;
        static constexpr int     EVENT_PROGRAMCHANGE                   = 4;
        static constexpr int     EVENT_VELOCITY                        = 2;
        static constexpr uint8_t FILTER_MODE_LOW_PASS                  = 0;
        static constexpr uint8_t FILTER_MODE_HIGH_PASS                 = 1;
        static constexpr uint8_t FILTER_MODE_BAND_PASS                 = 2;
        static constexpr uint8_t FILTER_MODE_NOTCH                     = 3;
        static constexpr uint8_t FILTER_MODE_PEAK                      = 4;
        static constexpr uint8_t FILTER_MODE_LOWSHELF                  = 5;
        static constexpr uint8_t FILTER_MODE_HIGHSHELF                 = 6;
        static constexpr uint8_t FILTER_MODE_BAND_REJECT               = 7;
        static constexpr int     LOOP_INFINITE                         = std::numeric_limits<int>::max();
        static constexpr uint8_t MONO                                  = 1;
        static constexpr uint8_t NOISE_WHITE                           = 0;
        static constexpr uint8_t NOISE_WHITE_FAST                      = 1;
        static constexpr uint8_t NOISE_PINK                            = 2;
        static constexpr uint8_t NOISE_GAUSSIAN_WHITE_FAST             = 3;
        static constexpr uint8_t NOISE_GAUSSIAN_WHITE                  = 4;
        static constexpr uint8_t NOISE_SIMPLEX                         = 5;
        static constexpr float   NOTE_WHOLE                            = 0.25f;
        static constexpr float   NOTE_HALF                             = 0.5f;
        static constexpr uint8_t NOTE_QUARTER                          = 1;
        static constexpr uint8_t NOTE_EIGHTH                           = 2;
        static constexpr uint8_t NOTE_SIXTEENTH                        = 4;
        static constexpr uint8_t NOTE_THIRTYSECOND                     = 8;
        static constexpr int     NO_AUDIO_DEVICE                       = -2;
        static constexpr int     NO_CHANNELS                           = 0;
        static constexpr int     NO_EVENT                              = -1;
        static constexpr int     NO_INPOINT                            = 0;
        static constexpr int     NO_LOOP                               = -2;
        static constexpr int     NO_LOOP_COUNT                         = -1;
        static constexpr int     NO_OUTPOINT                           = -1;
        static constexpr int     NO_POSITION                           = -1;
        static constexpr int     NO_VALUE                              = -1;
        static constexpr uint8_t PAN_LINEAR                            = 0;
        static constexpr uint8_t PAN_SINE_LAW                          = 2;
        static constexpr uint8_t PAN_SQUARE_LAW                        = 1;
        static constexpr uint8_t SIGNAL_LEFT                           = 0;
        static constexpr float   SIGNAL_MAX                            = 1.0f;
        static constexpr float   SIGNAL_MIN                            = -1.0f;
        static constexpr int     SIGNAL_MONO                           = 1;
        static constexpr int     SIGNAL_PROCESSING_IGNORE_IN_OUTPOINTS = -3;
        static constexpr int     SIGNAL_RIGHT                          = 1;
        static constexpr int     SIGNAL_STEREO                         = 2;
        static constexpr uint8_t SIG_INT16_BIG_ENDIAN                  = 2;
        static constexpr uint8_t SIG_INT16_LITTLE_ENDIAN               = 3;
        static constexpr uint8_t SIG_INT24_3_BIG_ENDIAN                = 4;
        static constexpr uint8_t SIG_INT24_3_LITTLE_ENDIAN             = 5;
        static constexpr uint8_t SIG_INT24_4_BIG_ENDIAN                = 6;
        static constexpr uint8_t SIG_INT24_4_LITTLE_ENDIAN             = 7;
        static constexpr uint8_t SIG_INT32_BIG_ENDIAN                  = 8;
        static constexpr uint8_t SIG_INT32_LITTLE_ENDIAN               = 9;
        static constexpr uint8_t SIG_INT8                              = 0;
        static constexpr uint8_t SIG_UINT8                             = 1;
        static constexpr uint8_t STEREO                                = 2;
        static constexpr uint8_t VERSION_MAJOR                         = 0;
        static constexpr uint8_t VERSION_MINOR                         = 8;
        static constexpr uint8_t WAVESHAPE_INTERPOLATE_NONE            = 0;
        static constexpr uint8_t WAVESHAPE_INTERPOLATE_LINEAR          = 1;
        static constexpr uint8_t WAVESHAPE_INTERPOLATE_CUBIC           = 2;
        static constexpr uint8_t WAV_FORMAT_PCM                        = 1;
        static constexpr uint8_t WAV_FORMAT_IEEE_FLOAT_32BIT           = 3;

        /* --- sound --- */

        static float dB_to_volume(const float dB) {
            return pow(10.0f, dB / 20.0f);
        }

        static float volume_to_dB(const float volume) {
            return 20.0f * log10(volume);
        }

        static constexpr float   MIDI_NOTE_CONVERSION_BASE_FREQUENCY = 440.0;
        static constexpr uint8_t NOTE_OFFSET                         = 69;

        static float midi_note_to_frequency(const uint8_t midi_note) {
            return MIDI_NOTE_CONVERSION_BASE_FREQUENCY * pow(2, (midi_note - NOTE_OFFSET) / 12.0);
        }

        static void normalize(float* buffer, const uint32_t numSamples) {
            if (buffer == nullptr || numSamples == 0) {
                return;
            }

            float peakValue = 0.0f;

            // find the peak value in the buffer
            for (uint32_t i = 0; i < numSamples; i++) {
                const float sample = std::abs(buffer[i]);
                if (sample > peakValue) {
                    peakValue = sample;
                }
            }

            // avoid division by zero
            if (peakValue == 0.0f) {
                return;
            }

            const float normalizationFactor = 1.0f / peakValue;

            // normalize the buffer
            for (uint32_t i = 0; i < numSamples; i++) {
                buffer[i] *= normalizationFactor;
            }
        }

        static void peak(const float* buffer, const uint32_t length, float& min, float& max) {
            if (buffer == nullptr || length == 0) {
                return;
            }

            min = 0.0f;
            max = 0.0f;

            for (uint32_t i = 0; i < length; i++) {
                const float sample = buffer[i];
                if (sample < min) {
                    min = sample;
                }
                if (sample > max) {
                    max = sample;
                }
            }
        }

        /* --- math --- */

        // static uint32_t millis_to_samples(float pMillis, float pSampleRate);
        // static uint32_t millis_to_samples(float pMillis);
        // static float    random_normalized();
        // static uint32_t x32Seed;
        // static uint32_t xorshift32();
        // static float    random();
        inline static uint32_t x32Seed = 23;

        static uint32_t millis_to_samples(const float pMillis, const float pSampleRate) {
            return static_cast<uint32_t>(pMillis / 1000.0f * pSampleRate);
        }

        /* xorshift32 ( ref: https://en.wikipedia.org/wiki/Xorshift ) */
        // static uint32_t x32Seed = 23;
        static uint32_t xorshift32() {
            x32Seed ^= x32Seed << 13;
            x32Seed ^= x32Seed >> 17;
            x32Seed ^= x32Seed << 5;
            return x32Seed;
        }

        static float constexpr UINT32_MAX_INV = 1.0f / static_cast<float>(UINT32_MAX);

        /**
         * returns a random number between 0.0 ... 1.0
         */
        static float random_normalized() {
            return static_cast<float>(xorshift32()) * UINT32_MAX_INV;
        }

        /**
         * returns a random number between -1.0 ... 1.0
         */
        static float random() {
            return static_cast<float>(xorshift32()) * UINT32_MAX_INV * 2 - 1;
        }

        static float clamp(const float value,
                           const float minimum,
                           const float maximum) {
            return value > maximum ? maximum : value < minimum ? minimum
                                                               : value;
        }

        static float clip(const float value) {
            return clamp(value);
        }

        static float clamp(const float value) {
            return value > SIGNAL_MAX ? SIGNAL_MAX : value < SIGNAL_MIN ? SIGNAL_MIN
                                                                        : value;
        }

        static uint8_t clamp127(const uint8_t value) {
            return value > 127 ? 127 : value;
        }

        template<typename T>
        static T clamp(T value, T minimum, T maximum) {
            return value > maximum ? maximum : value < minimum ? minimum
                                                               : value;
        }

        static float pow(const float base, const float exponent) {
            return powf(base, exponent);
        }

        static float lerp(const float v0, const float v1, const float t) {
            return v0 + t * (v1 - v0);
        }

        static float max(const float a, const float b) {
            return a > b ? a : b;
        }

        static float min(const float a, const float b) {
            return a < b ? a : b;
        }

        static float abs(const float value) {
            return value > 0 ? value : -value;
        }

        template<typename T>
        static T abs(T value) {
            return value > 0 ? value : -value;
        }

        template<typename T>
        static int sign(T val) {
            return (T(0) < val) - (val < T(0));
        }

        static float mod(const float a, const float b) {
            return a >= b ? a - b * static_cast<int>(a / b) : a;
            // return a >= b ? fmodf(a, b) : a;
            // return input >= ceil ? input % ceil : input;
            // from https://stackoverflow.com/questions/33333363/built-in-mod-vs-custom-mod-function-improve-the-performance-of-modulus-op
        }

        static float map(const float value,
                         const float inputMin,
                         const float inputMax,
                         const float outputMin,
                         const float outputMax) {
            const float a = value - inputMin;
            const float b = inputMax - inputMin;
            const float c = outputMax - outputMin;
            const float d = a / b;
            const float e = d * c;
            return e + outputMin;
        }

        static int16_t map_i16(const int16_t value,
                               const int16_t inputMin,
                               const int16_t inputMax,
                               const int16_t outputMin,
                               const int16_t outputMax) {
            return (value - inputMin) * (outputMax - outputMin) / (inputMax - inputMin) + outputMin;
        }

        static float sin(const float r) {
            return sinf(r);
        }

        static float cos(const float r) {
            return cosf(r);
        }

        static float sinh(const float r) {
            return sinhf(r);
        }

        static float cosh(const float r) {
            return coshf(r);
        }

        static float sqrt(const float r) {
            return sqrtf(r);
        }

        static float fast_sqrt(const float x) {
            return 1.0f / fast_inv_sqrt(x);
        }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wuninitialized"
        static float fast_inv_sqrt(float x) {
            const float xhalf = 0.5f * x;
            int         i     = *reinterpret_cast<int*>(&x);
            i                 = 0x5f3759df - (i >> 1);
            x                 = *reinterpret_cast<float*>(&i);
            return x * (1.5f - xhalf * x * x);
        }
#pragma GCC diagnostic pop

        static constexpr float PIf     = (float) PI;
        static constexpr float TWO_PIf = (float) TWO_PI;

        static float fast_sin(float x) {
            if (x < -PIf) {
                while (x < -PIf) {
                    x += TWO_PIf;
                }
            } else if (x > PIf) {
                while (x > PIf) {
                    x -= TWO_PIf;
                }
            }
            const float x2          = x * x;
            const float numerator   = -x * (static_cast<float>(-11511339840l) + x2 * (static_cast<float>(1640635920) + x2 * (-52785432 + x2 * 479249)));
            const float denominator = static_cast<float>(11511339840l) + x2 * (static_cast<float>(277920720) + x2 * (3177720 + x2 * 18361));
            return numerator / denominator;
        }

        static float fast_sin3(const float X) {
            // Valid on interval [-PI, PI]
            // Sine approximation using Bhaskara I technique discovered in 7th century.
            // https://en.wikipedia.org/wiki/Bh%C4%81skara_I
            const float AbsX        = abs(X);
            const float Numerator   = 16.0f * X * (PIf - AbsX);
            const float Denominator = 5.0f * PIf * PIf - 4.0f * AbsX * (PIf - AbsX);
            return Numerator / Denominator;
        }

        static float fast_cos(float x) {
            if (x < -PIf) {
                while (x < -PIf) {
                    x += TWO_PIf;
                }
            } else if (x > PIf) {
                while (x > PIf) {
                    x -= TWO_PIf;
                }
            }
            const float x2          = x * x;
            const float numerator   = -(-39251520 + x2 * (18471600 + x2 * (-1075032 + 14615 * x2)));
            const float denominator = 39251520 + x2 * (1154160 + x2 * (16632 + x2 * 127));
            return numerator / denominator;
        }

        static float fast_sinh(const float x) {
            const auto x2          = x * x;
            const auto numerator   = -x * (static_cast<float>(11511339840) + x2 * (static_cast<float>(1640635920) + x2 * (52785432 + x2 * 479249)));
            const auto denominator = static_cast<float>(-11511339840) + x2 * (static_cast<float>(277920720) + x2 * (-3177720 + x2 * 18361));
            return numerator / denominator;
        }

        static float fast_cosh(const float x) {
            const auto x2          = x * x;
            const auto numerator   = -(39251520 + x2 * (18471600 + x2 * (1075032 + 14615 * x2)));
            const auto denominator = -39251520 + x2 * (1154160 + x2 * (-16632 + 127 * x2));
            return numerator / denominator;
        }

        static float fast_tan(const float x) {
            const auto x2          = x * x;
            const auto numerator   = x * (-135135 + x2 * (17325 + x2 * (-378 + x2)));
            const auto denominator = -135135 + x2 * (62370 + x2 * (-3150 + 28 * x2));
            return numerator / denominator;
        }

        static float fast_tanh(const float x) {
            const auto x2          = x * x;
            const auto numerator   = x * (135135 + x2 * (17325 + x2 * (378 + x2)));
            const auto denominator = 135135 + x2 * (62370 + x2 * (3150 + 28 * x2));
            return numerator / denominator;
        }

        static float exp_j(const float x) {
            const auto numerator   = 1680 + x * (840 + x * (180 + x * (20 + x)));
            const auto denominator = 1680 + x * (-840 + x * (180 + x * (-20 + x)));
            return numerator / denominator;
        }

        static float fast_atan(const float x) {
            static constexpr float PI_FOURTH = PIf / 4.0f;
            return PI_FOURTH * x - x * (fabs(x) - 1) * (0.2447f + 0.0663f * fabs(x));
        }

        static float fast_atan2(const float x) {
            static constexpr float PI_FOURTH = PIf / 4.0f;
            static constexpr float A         = 0.0776509570923569;
            static constexpr float B         = -0.287434475393028;
            static constexpr float C         = PI_FOURTH - A - B;
            const float            xx        = x * x;
            return ((A * xx + B) * xx + C) * x;
        }

        /* --- interpolation --- */

        static float linear_interpolate(const float y1, const float y2, const float mu) {
            return y1 * (1.0f - mu) + y2 * mu;
        }

        static float interpolate_samples_linear(const float* buffer, const size_t bufferSize, const float position) {
            const int   posInt = static_cast<int>(position);
            const float mu     = position - posInt;

            // Ensure that the positions are valid
            const int y1Pos = posInt;
            const int y2Pos = posInt + 1 >= bufferSize ? bufferSize - 1 : posInt + 1;

            // Get the samples for interpolation
            const float y1 = buffer[y1Pos];
            const float y2 = buffer[y2Pos];

            // Return the interpolated sample
            return linear_interpolate(y1, y2, mu);
        }

        static float cubic_interpolate(const float y0, const float y1, const float y2, const float y3, const float mu) {
            const float mu2 = mu * mu;
            const float a0  = y3 - y2 - y0 + y1;
            const float a1  = y0 - y1 - a0;
            const float a2  = y2 - y0;
            const float a3  = y1;

            return a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3;
        }

        static float interpolate_samples_cubic(const float* buffer, const uint32_t bufferSize, const float position) {
            const int   posInt = static_cast<int>(position);
            const float mu     = position - posInt;

            // Ensure that the positions are valid
            const int y0Pos = posInt - 1 < 0 ? 0 : posInt - 1;
            const int y1Pos = posInt;
            const int y2Pos = posInt + 1 >= bufferSize ? bufferSize - 1 : posInt + 1;
            const int y3Pos = posInt + 2 >= bufferSize ? bufferSize - 1 : posInt + 2;

            // Get the samples for interpolation
            const float y0 = buffer[y0Pos];
            const float y1 = buffer[y1Pos];
            const float y2 = buffer[y2Pos];
            const float y3 = buffer[y3Pos];

            // Return the interpolated sample
            return cubic_interpolate(y0, y1, y2, y3, mu);
        }

        /* --- buffer --- */

        static void fill(float* buffer, const float value, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer[i] = value;
            }
        }

        /**
         * copies src into dst. dst will be overwritten.
         */
        static void copy(float* src, float* dst, const uint32_t length) {
            std::copy_n(src, length, dst);
        }

        /**
         * adds buffer_b to buffer_a. result will be stored in buffer_a, buffer_b will not be changed.
         */
        static void add(float* buffer_a, const float* buffer_b, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] += buffer_b[i];
            }
        }

        static void add(float* buffer_a, const float scalar, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] += scalar;
            }
        }

        /**
         * subtracts buffer_b from buffer_a. result will be stored in buffer_a, buffer_b will not be changed.
         */
        static void sub(float* buffer_a, const float* buffer_b, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] -= buffer_b[i];
            }
        }

        static void sub(float* buffer_a, const float scalar, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] -= scalar;
            }
        }

        /**
         * multiplies buffer_b with buffer_a. result will be stored in buffer_a, buffer_b will not be changed.
         */
        static void mult(float* buffer_a, const float* buffer_b, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] *= buffer_b[i];
            }
        }

        static void mult(float* buffer_a, const float scalar, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] *= scalar;
            }
        }

        /**
         * divides buffer_b from buffer_a. result will be stored in buffer_a, buffer_b will not be changed.
         */
        static void div(float* buffer_a, const float* buffer_b, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] /= buffer_b[i];
            }
        }

        static void div(float* buffer_a, const float scalar, const uint32_t length) {
            for (uint32_t i = 0; i < length; i++) {
                buffer_a[i] /= scalar;
            }
        }
    };
} // namespace umgebung
