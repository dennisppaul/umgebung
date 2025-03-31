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
 * - [x] float process(float)
 * - [x] void process(AudioSignal&)
 * - [x] void process(float*, uint32_t)
 * - [x] void process(float*, float*, uint32_t)
 */

#pragma once

#include "AudioUtilities.h"
#include "AudioSignal.h"

namespace umgebung {
    class ADSR {
        /*
         *       @description(
         *
         *          |----->|-->|   |-->|
         *          |---A--|-D-|-S-|-R-|
         *          |      /\
         *          |     /  \
         *          |    /    \_____
         *          |   /        ^  \
         *          |  /         |   \
         *          | /          |    \
         *          |/___________|_____\
         *          |
         *          |Press          |Release
         *       )
         *
         */
    public:
        explicit ADSR(const uint32_t sample_rate) : fSampleRate(sample_rate), FADE_TO_ZERO_RATE_SEC(0.01f), USE_FADE_TO_ZERO_STATE(false) {
            fAmp     = 0.0f;
            fAttack  = AudioUtilities::DEFAULT_ATTACK;
            fDecay   = AudioUtilities::DEFAULT_DECAY;
            fDelta   = 0.0f;
            fRelease = AudioUtilities::DEFAULT_RELEASE;
            fSustain = AudioUtilities::DEFAULT_SUSTAIN;
            fState   = ENVELOPE_STATE::IDLE;
            setState(ENVELOPE_STATE::IDLE);
        }

        float process() {
            step();
            return fAmp;
        }

        float process(const float signal) {
            step();
            return signal * fAmp;
        }

        void process(AudioSignal& signal) {
            step();
            signal.left *= fAmp;
            signal.right *= fAmp;
        }

        void process(float*         signal_buffer_left,
                     float*         signal_buffer_right,
                     const uint32_t buffer_length) {
            for (uint32_t i = 0; i < buffer_length; i++) {
                step();
                signal_buffer_left[i] *= fAmp;
                signal_buffer_right[i] *= fAmp;
            }
        }

        void process(float* signal_buffer, const uint32_t buffer_length) {
            for (uint32_t i = 0; i < buffer_length; i++) {
                step();
                signal_buffer[i] *= fAmp;
            }
        }

        void start() {
            check_scheduled_attack_state();
        }

        void stop() {
            check_scheduled_release_state();
        }

        void set_adsr(const float attack, const float decay, const float sustain, const float release) {
            set_attack(attack);
            set_decay(decay);
            set_sustain(sustain);
            set_release(release);
        }

        float get_attack() const {
            return fAttack;
        }

        void set_attack(const float attack) {
            fAttack = attack;
        }

        float get_decay() const {
            return fDecay;
        }

        void set_decay(const float decay) {
            fDecay = decay;
        }

        float get_sustain() const {
            return fSustain;
        }

        void set_sustain(const float sustain) {
            fSustain = sustain;
        }

        float get_release() const {
            return fRelease;
        }

        void set_release(const float release) {
            fRelease = release;
        }

        bool is_idle() const {
            return fState == ENVELOPE_STATE::IDLE;
        }

    private:
        enum class ENVELOPE_STATE {
            IDLE,
            ATTACK,
            DECAY,
            SUSTAIN,
            RELEASE,
            PRE_ATTACK_FADE_TO_ZERO
        };
        const uint32_t fSampleRate;
        const float    FADE_TO_ZERO_RATE_SEC;
        const bool     USE_FADE_TO_ZERO_STATE;
        float          fAmp;
        float          fAttack;
        float          fDecay;
        float          fDelta;
        float          fRelease;
        ENVELOPE_STATE fState;
        float          fSustain;

        void check_scheduled_attack_state() {
            if (fAmp > 0.0f) {
                if (USE_FADE_TO_ZERO_STATE) {
                    if (fState != ENVELOPE_STATE::PRE_ATTACK_FADE_TO_ZERO) {
                        fDelta = compute_delta_fraction(-fAmp, FADE_TO_ZERO_RATE_SEC);
                        setState(ENVELOPE_STATE::PRE_ATTACK_FADE_TO_ZERO);
                    }
                } else {
                    fDelta = compute_delta_fraction(1.0f, fAttack);
                    setState(ENVELOPE_STATE::ATTACK);
                }
            } else {
                fDelta = compute_delta_fraction(1.0f, fAttack);
                setState(ENVELOPE_STATE::ATTACK);
            }
        }

        void check_scheduled_release_state() {
            if (fState != ENVELOPE_STATE::RELEASE) {
                fDelta = compute_delta_fraction(-fAmp, fRelease);
                setState(ENVELOPE_STATE::RELEASE);
            }
        }

        float compute_delta_fraction(const float pDelta, const float pDuration) const {
            return pDuration > 0 ? (pDelta / static_cast<float>(fSampleRate)) / pDuration : pDelta;
        }

        void setState(const ENVELOPE_STATE pState) {
            fState = pState;
        }

        void step() {
            switch (fState) {
                case ENVELOPE_STATE::IDLE:
                case ENVELOPE_STATE::SUSTAIN:
                    break;
                case ENVELOPE_STATE::ATTACK:
                    // increase amp to sustain_level in ATTACK sec
                    fAmp += fDelta;
                    if (fAmp >= 1.0f) {
                        fAmp   = 1.0f;
                        fDelta = compute_delta_fraction(-(1.0f - fSustain), fDecay);
                        setState(ENVELOPE_STATE::DECAY);
                    }
                    break;
                case ENVELOPE_STATE::DECAY:
                    // decrease amp to sustain_level in DECAY sec
                    fAmp += fDelta;
                    if (fAmp <= fSustain) {
                        fAmp = fSustain;
                        setState(ENVELOPE_STATE::SUSTAIN);
                    }
                    break;
                case ENVELOPE_STATE::RELEASE:
                    // decrease amp to 0.0 in RELEASE sec
                    fAmp += fDelta;
                    if (fAmp <= 0.0f) {
                        fAmp = 0.0f;
                        setState(ENVELOPE_STATE::IDLE);
                    }
                    break;
                case ENVELOPE_STATE::PRE_ATTACK_FADE_TO_ZERO:
                    fAmp += fDelta;
                    if (fAmp <= 0.0f) {
                        fAmp   = 0.0f;
                        fDelta = compute_delta_fraction(1.0f, fAttack);
                        setState(ENVELOPE_STATE::ATTACK);
                    }
                    break;
            }
        }
    };
} // namespace umgebung
