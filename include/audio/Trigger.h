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
 * - [ ] float process()
 * - [x] float process(float)
 * - [ ] void process(AudioSignal&)
 * - [x] void process(float*, uint32_t) *no overwrites*
 * - [ ] void process(float*, float*, uint32_t)
 */

#pragma once

#include <stdint.h>
#include <vector>

/**
 * generates an event from an oscillating input signal. can be used to generate something like a beat
 */
namespace umgebung {
    enum TriggerEvent {
        EVENT_FALLING_EDGE = -1,
        EVENT_RISING_EDGE  = 1,
    };

    class TriggerListener {
    public:
        virtual ~TriggerListener()      = default;
        virtual void trigger(int event) = 0;
    };

    class Trigger {
    public:
        Trigger() : previous_signal(0.0), enable_falling_edge(true), enable_rising_edge(true) {}

        void add_listener(TriggerListener* listener) {
            listeners.push_back(listener);
        }

        bool remove_listener(const TriggerListener* listener) {
            for (auto it = listeners.begin(); it != listeners.end(); ++it) {
                if (*it == listener) {
                    listeners.erase(it);
                    return true;
                }
            }
            return false;
        }

        void trigger_rising_edge(const bool enable_trigger_rising_edge) {
            enable_rising_edge = enable_trigger_rising_edge;
        }

        void trigger_falling_edge(const bool enable_trigger_falling_edge) {
            enable_falling_edge = enable_trigger_falling_edge;
        }

        float process(const float signal) {
            if (enable_rising_edge && (previous_signal <= 0 && signal > 0)) {
                fireEvent(EVENT_RISING_EDGE);
            }
            if (enable_falling_edge && (previous_signal >= 0 && signal < 0)) {
                fireEvent(EVENT_FALLING_EDGE);
            }
            previous_signal = signal;
            return signal;
        }

        void process(const float* signal_buffer, const uint32_t buffer_length) {
            for (uint16_t i = 0; i < buffer_length; i++) {
                process(signal_buffer[i]);
            }
        }

    private:
        typedef void (*CallbackType1_I)(int);
        CallbackType1_I callback_event = nullptr;

    public:
        void set_callback(const CallbackType1_I callback) {
            callback_event = callback;
        }

        void remove_callback() {
            callback_event = nullptr;
        }

    private:
        float                         previous_signal;
        bool                          enable_falling_edge;
        bool                          enable_rising_edge;
        std::vector<TriggerListener*> listeners;

        void call_trigger(const int event) const {
            if (callback_event) {
                callback_event(event);
            }
        }

        void fireEvent(const int event) const {
            for (TriggerListener* l: listeners) {
                l->trigger(event);
            }
            call_trigger(event);
        }
    };
} // namespace umgebung
