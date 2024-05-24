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

#include "Umgebung.h"

#ifdef DISABLE_GRAPHICS
namespace umgebung {
    static PApplet* fApplet = nullptr;

    void set_graphics_context(PApplet* applet) {
        fApplet = applet;
    }

    APP_WINDOW* init_graphics(int width, int height, const char* title) {
        return nullptr;
    }

    void handle_setup(APP_WINDOW* window) {
        fApplet->init();
        if (!headless) {
            fApplet->pre_draw();
        }
        fApplet->setup();
        if (!headless) {
            fApplet->post_draw();
        }
    }

    void handle_draw(APP_WINDOW* window) {
        /* timer begin  */
        static std::chrono::high_resolution_clock::time_point
            startTime = std::chrono::high_resolution_clock::now(),
            endTime;

        fApplet->draw();

        /* timer end */
        endTime                                     = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
            endTime - startTime);
        double frameTime   = frameDuration.count();
        fApplet->frameRate = (float) (1.0 / frameTime);
        fApplet->frameCount++;
        startTime = std::chrono::high_resolution_clock::now();
    }

    void handle_event(const SDL_Event& event, bool& fAppIsRunning, bool& fMouseIsPressed) {
    }

    void handle_shutdown(APP_WINDOW* window) {
    }
} // namespace umgebung

#endif // DISABLE_GRAPHICS
