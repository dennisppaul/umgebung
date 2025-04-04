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

/**
 * PROCESSOR INTERFACE
 *
 * - [x] float process()
 * - [ ] float process(float)
 * - [ ] void process(AudioSignal&)
 * - [x] void process(float*, uint32_t) *overwrite*
 * - [ ] void process(float*, float*, uint32_t)
 */

#pragma once

#include <cstdint>
#include <cmath>

#include "AudioUtilities.h"

#ifndef PI
#define PI M_PI
#endif

#ifndef TWO_PI
#define TWO_PI (M_PI * 2)
#endif

#ifndef HALF_PI
#define HALF_PI (M_PI / 2)
#endif

/**
 * plays back a chunk of samples ( i.e arbitrary, single-cycle waveform like sine, triangle, saw or square waves ) at
 * different frequencies and amplitudes.
 */
namespace umgebung {
    class Wavetable {
    public:
        Wavetable(const uint32_t wavetable_size, const float sample_rate) : Wavetable(new float[wavetable_size], wavetable_size, sample_rate) {
            fDeleteWavetable = true;
        }

        Wavetable(float* wavetable, const uint32_t wavetable_size, const float sample_rate) : mWavetableSize(wavetable_size),
                                                                                              sample_rate(sample_rate),
                                                                                              mFrequency(0),
                                                                                              fInterpolationType(AudioUtilities::WAVESHAPE_INTERPOLATE_NONE) {
            mWavetable                = wavetable;
            fDeleteWavetable          = false;
            mArrayPtr                 = 0;
            mJitterRange              = 0.0f;
            mAmplitude                = M_DEFAULT_AMPLITUDE;
            mPhaseOffset              = 0.0f;
            mDesiredAmplitude         = 0.0f;
            mDesiredAmplitudeFraction = 0.0f;
            mDesiredAmplitudeSteps    = 0;
            set_frequency(M_DEFAULT_FREQUENCY);
        }

        ~Wavetable() {
            if (fDeleteWavetable) {
                delete[] mWavetable;
            }
        }

        static void fill(float* wavetable, const uint32_t wavetable_size, const uint8_t waveform) {
            switch (waveform) {
                case WAVEFORM_SINE:
                    sine(wavetable, wavetable_size);
                    break;
                case WAVEFORM_TRIANGLE:
                    triangle(wavetable, wavetable_size);
                    break;
                case WAVEFORM_SQUARE:
                    square(wavetable, wavetable_size);
                    break;
                case WAVEFORM_SAWTOOTH:
                    sawtooth(wavetable, wavetable_size, false);
                    break;
                default:
                    sine(wavetable, wavetable_size);
            }
        }

        static void noise(float* wavetable, const int length) {
            for (int i = 0; i < length; ++i) {
                wavetable[i] = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
            }
        }

        static void pulse(float* wavetable, const uint32_t wavetable_size, const float pulse_width) {
            const auto threshold = static_cast<uint32_t>(static_cast<float>(wavetable_size) * pulse_width);
            for (uint32_t i = 0; i < wavetable_size; i++) {
                if (i < threshold) {
                    wavetable[i] = 1.0f;
                } else {
                    wavetable[i] = -1.0f;
                }
            }
        }

        static void sawtooth(float* wavetable, const uint32_t wavetable_size, const bool is_ramp_up = true) {
            const float mSign = is_ramp_up ? -1.0f : 1.0f;
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = mSign * (2.0f * (static_cast<float>(i) / static_cast<float>(wavetable_size - 1)) - 1.0f);
            }
        }

        static void sawtooth(float* wavetable, const int length, const int harmonics) {
            std::fill_n(wavetable, length, 0.0f);
            const auto amps = new float[harmonics];
            for (int i = 0; i < harmonics; ++i) {
                amps[i] = 1.f / (i + 1);
            }
            fourier_table(wavetable, length, harmonics, amps, -0.25f);
            delete[] amps;
        }

        static void sine(float* wavetable, const uint32_t wavetable_size) {
            for (uint32_t i = 0; i < wavetable_size; i++) {
                wavetable[i] = sin(2.0f * PIf * (static_cast<float>(i) / static_cast<float>(wavetable_size)));
            }
        }


        static void square(float* wavetable, const int length, const int harmonics) {
            std::fill_n(wavetable, length, 0.0f);
            const auto amps = new float[harmonics]();
            for (int i = 0; i < harmonics; i += 2) {
                amps[i] = 1.f / (i + 1);
            }
            fourier_table(wavetable, length, harmonics, amps, -0.25f);
            delete[] amps;
        }

        static void square(float* wavetable, const int length) {
            const int half = length / 2;
            for (int i = 0; i < half; ++i) {
                wavetable[i]        = 1.0f;
                wavetable[i + half] = -1.0f;
            }
        }

        static void triangle(float* wavetable, const uint32_t wavetable_size) {
            for (uint32_t i = 0; i < wavetable_size; ++i) {
                const float phase = static_cast<float>(i) / wavetable_size; // 0.0 to <1.0
                float       value = 0.0f;
                if (phase < 0.25f) {
                    value = 4.0f * phase;
                } else if (phase < 0.75f) {
                    value = 2.0f - 4.0f * phase;
                } else {
                    value = -4.0f + 4.0f * phase;
                }
                wavetable[i] = value;
            }
        }

        static void triangle(float* wavetable, const int length, const int harmonics) {
            std::fill_n(wavetable, length, 0.0f);
            const auto amps = new float[harmonics]();
            for (int i = 0; i < harmonics; i += 2) {
                amps[i] = 1.f / ((i + 1) * (i + 1));
            }
            fourier_table(wavetable, length, harmonics, amps, 0.0f);
            delete[] amps;
        }

        static void normalise_table(float* wavetable, const int length) {
            float max_val = 0.f;
            for (int i = 0; i < length; ++i) {
                max_val = std::max(std::abs(wavetable[i]), max_val);
            }
            if (max_val > 0.f) {
                for (int i = 0; i < length; ++i) {
                    wavetable[i] /= max_val;
                }
            }
        }

        static void fourier_table(float* wavetable, const int length, const int harmonics, const float* amps, float phase) {
            phase *= 2.0f * PI;
            for (int i = 0; i < harmonics; ++i) {
                const float a = (amps != nullptr) ? amps[i] : 1.f;
                for (int n = 0; n < length; ++n) {
                    const double w = (i + 1) * (n * 2.0 * PI / length);
                    wavetable[n] += static_cast<float>(a * std::cos(w + phase));
                }
            }
            normalise_table(wavetable, length);
        }

        void set_waveform(const uint8_t waveform) const {
            fill(mWavetable, mWavetableSize, waveform);
        }

        float get_frequency() const {
            return mFrequency;
        }

        void set_frequency(const float frequency) {
            const float mNewFrequency = fabs(frequency);
            if (mFrequency != mNewFrequency) {
                mFrequency = mNewFrequency;
                mStepSize  = computeStepSize();
            }
        }

        /**
         * alternative version of `set_frequency` which takes a second paramter that specifies the duration in samples from
         * current to new value. this can prevent unwanted artifacts ( e.g when quickly changing values ). it can also be
         * used to create glissando or portamento effects.
         *
         * @param frequency                         destination frequency
         * @param interpolation_duration_in_samples duration of interpolation in samples
         */
        void set_frequency(const float frequency, const uint16_t interpolation_duration_in_samples) {
            if (interpolation_duration_in_samples > 0) {
                mDesiredFrequency           = frequency;
                mDesiredFrequencySteps      = interpolation_duration_in_samples;
                const float mFrequencyDelta = mDesiredFrequency - mFrequency;
                mDesiredFrequencyFraction   = mFrequencyDelta / interpolation_duration_in_samples;
            } else {
                set_frequency(frequency);
            }
        }

        float get_offset() const {
            return mOffset;
        }

        void set_offset(const float offset) {
            mOffset = offset;
        }

        float get_amplitude() const {
            return mAmplitude;
        }

        /**
         * @param amplitude amplitude
         */

        void set_amplitude(const float amplitude) {
            mAmplitude             = amplitude;
            mDesiredAmplitudeSteps = 0;
        }

        /**
         * alternative version of `set_amplitude` which takes a second paramter that specifies the duration in samples from
         * current to new value. this can prevent unwanted artifacts ( e.g crackling noise when changing amplitude quickly
         * especially on smoother wave shape like sine waves ). it can also be used to create glissando or portamento
         * effects.
         *
         * @param amplitude                         destination amplitude
         * @param interpolation_duration_in_samples duration of interpolation in samples
         */
        void set_amplitude(const float amplitude, const uint32_t interpolation_duration_in_samples) {
            if (interpolation_duration_in_samples > 0) {
                mDesiredAmplitude           = amplitude;
                mDesiredAmplitudeSteps      = interpolation_duration_in_samples;
                const float mAmplitudeDelta = mDesiredAmplitude - mAmplitude;
                mDesiredAmplitudeFraction   = mAmplitudeDelta / interpolation_duration_in_samples;
            } else {
                set_amplitude(amplitude);
            }
        }

        float* get_wavetable() const {
            return mWavetable;
        }

        uint32_t get_wavetable_size() const {
            return mWavetableSize;
        }

        float get_phase_offset() const {
            return mPhaseOffset;
        }

        void set_phase_offset(const float phase_offset) {
            mPhaseOffset = phase_offset < 0 ? 1 + phase_offset : phase_offset;
        }

        float get_jitter_range() const {
            return mJitterRange;
        }

        void set_jitter_range(const float jitter_range) {
            mJitterRange = jitter_range;
        }

        void reset() {
            mSignal   = 0.0f;
            mArrayPtr = 0.0f;
        }

        float current() const {
            return mSignal;
        }

        /**
         * set speed of oscillator in seconds per phase
         *
         * @param frequency oscillation speed on seconds per phase ( or Hz )
         */
        void set_oscillation_speed(const float frequency) {
            set_frequency(1.0f / frequency);
        }

        /**
         * set output value range of oscillator in minimum and maximum. this method affects the oscillator’s amplitude and
         * offset. this methods can be particularily helpful when using an oscillator for non-audio applications ( e.g as an
         * LFO that controlls the frequency ).
         *
         * @param min minimum output value of oscillator
         * @param max maximum output value of oscillator
         */
        void set_oscillation_range(const float min, const float max) {
            const float mDelta = max - min;
            set_amplitude(mDelta * 0.5f);
            set_offset(mDelta * 0.5f + min);
        }

        void set_interpolation(const int interpolation_type) {
            fInterpolationType = interpolation_type;
        }

        float process() {
            if (mDesiredAmplitudeSteps > 0) {
                mDesiredAmplitudeSteps--;
                if (mDesiredAmplitudeSteps == 0) {
                    mAmplitude = mDesiredAmplitude;
                } else {
                    mAmplitude += mDesiredAmplitudeFraction;
                }
            }

            if (mDesiredFrequencySteps > 0) {
                mDesiredFrequencySteps--;
                if (mDesiredFrequencySteps == 0) {
                    set_frequency(mDesiredFrequency);
                } else {
                    set_frequency(mFrequency + mDesiredFrequencyFraction);
                }
            }
#define AudioUtilities_WAVETABLE_INTERPOLATE_SAMPLES 0
#if AudioUtilities_WAVETABLE_INTERPOLATE_SAMPLES == 0
            mSignal = next_sample();
#else
            switch (fInterpolationType) {
                case AudioUtilities::WAVESHAPE_INTERPOLATE_LINEAR:
                    mSignal = next_sample_interpolate_linear();
                    break;
                case AudioUtilities::WAVESHAPE_INTERPOLATE_CUBIC:
                    mSignal = next_sample_interpolate_cubic();
                    break;
                default:
                    mSignal = next_sample();
                    break;
            }
#endif // AudioUtilities_WAVETABLE_INTERPOLATE_SAMPLES
            mSignal *= mAmplitude;
            mSignal += mOffset;
            return mSignal;
        }

        void process(float* signal_buffer, const uint32_t buffer_length) {
            for (uint16_t i = 0; i < buffer_length; i++) {
                signal_buffer[i] = process();
            }
        }

    private:
        static constexpr float PIf                 = (float) PI;
        static constexpr float TWO_PIf             = (float) TWO_PI;
        static constexpr float M_DEFAULT_AMPLITUDE = 0.75f;
        static constexpr float M_DEFAULT_FREQUENCY = 220.0f;
        float*                 mWavetable;
        const uint32_t         mWavetableSize;
        const float            sample_rate;
        bool                   fDeleteWavetable;
        float                  mAmplitude;
        float                  mArrayPtr;
        float                  mDesiredAmplitude;
        float                  mDesiredAmplitudeFraction;
        uint16_t               mDesiredAmplitudeSteps;
        float                  mDesiredFrequency{};
        float                  mDesiredFrequencyFraction{};
        uint16_t               mDesiredFrequencySteps{};
        float                  mFrequency;
        float                  mJitterRange;
        float                  mOffset{};
        float                  mPhaseOffset;
        float                  mSignal{};
        float                  mStepSize{};
        uint8_t                fInterpolationType;

        void advance_array_ptr() {
            // mArrayPtr += mStepSize * (mEnableJitter ? (AudioUtilities::AudioUtilities::random() * mJitterRange + 1.0f) : 1.0f);
            // mArrayPtr += mStepSize;
            mArrayPtr += mStepSize;
            while (mArrayPtr >= mWavetableSize) {
                mArrayPtr -= mWavetableSize;
            }
            while (mArrayPtr < 0) {
                mArrayPtr += mWavetableSize;
            }
        }

        float computeStepSize() const {
            return mFrequency * (static_cast<float>(mWavetableSize) / static_cast<float>(sample_rate));
        }

        float next_sample() {
            const float mOutput = mWavetable[static_cast<int>(mArrayPtr)];
            advance_array_ptr();
            return mOutput;
        }

        float next_sample_interpolate_cubic() {
            const uint32_t mSampleOffset   = static_cast<int>(mPhaseOffset * mWavetableSize) % mWavetableSize;
            const float    mArrayPtrOffset = mArrayPtr + mSampleOffset;
            /* cubic interpolation */
            const float    frac    = mArrayPtrOffset - static_cast<int>(mArrayPtrOffset);
            const float    a       = static_cast<int>(mArrayPtrOffset) > 0 ? mWavetable[static_cast<int>(mArrayPtrOffset) - 1] : mWavetable[mWavetableSize - 1];
            const float    b       = mWavetable[static_cast<int>(mArrayPtrOffset) % mWavetableSize];
            const uint32_t p1      = static_cast<uint32_t>(mArrayPtrOffset) + 1;
            const float    c       = mWavetable[p1 >= mWavetableSize ? p1 - mWavetableSize : p1];
            const uint32_t p2      = static_cast<uint32_t>(mArrayPtrOffset) + 2;
            const float    d       = mWavetable[p2 >= mWavetableSize ? p2 - mWavetableSize : p2];
            const float    tmp     = d + 3.0f * b;
            const float    fracsq  = frac * frac;
            const float    fracb   = frac * fracsq;
            const float    mOutput = (fracb * (-a - 3.f * c + tmp) / 6.f + fracsq * ((a + c) / 2.f - b) + frac * (c + (-2.f * a - tmp) / 6.f) + b);
            advance_array_ptr();
            return mOutput;
        }

        float next_sample_interpolate_linear() {
            const uint32_t mSampleOffset   = static_cast<uint32_t>(mPhaseOffset * mWavetableSize) % mWavetableSize;
            const float    mArrayPtrOffset = mArrayPtr + mSampleOffset;
            /* linear interpolation */
            const float    mFrac   = mArrayPtrOffset - static_cast<int>(mArrayPtrOffset);
            const float    a       = mWavetable[static_cast<int>(mArrayPtrOffset)];
            const uint32_t p1      = static_cast<uint32_t>(mArrayPtrOffset) + 1;
            const float    b       = mWavetable[p1 >= mWavetableSize ? p1 - mWavetableSize : p1];
            const float    mOutput = a + mFrac * (b - a);
            advance_array_ptr();
            return mOutput;
        }
    };
} // namespace umgebung