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
#ifndef DISABLE_GRAPHICS

#include <GL/glew.h>

namespace umgebung {
    static PApplet*      fApplet   = nullptr;
    static SDL_GLContext glContext = nullptr;

    void set_graphics_context(PApplet* applet) {
        fApplet = applet;
    }

    static void set_default_graphics_state() {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    //    static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    //        fApplet->width  = width;
    //        fApplet->height = height;
    //
    //        int framebufferWidth, framebufferHeight;
    //        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    //        fApplet->framebuffer_width  = framebufferWidth;
    //        fApplet->framebuffer_height = framebufferHeight;
    //    }
    //
    //    static void error_callback(int error, const char *description) {
    //        std::cerr << "GLFW Error: " << description << std::endl;
    //    }
    //
    //    static int wndPos[2];
    //    static int wndSize[2];
    //
    //    static void SetFullScreen(GLFWwindow *_wnd, GLFWmonitor *_monitor, bool fullscreen) {
    //        // see https://stackoverflow.com/questions/47402766/switching-between-windowed-and-full-screen-in-opengl-glfw-3-2
    //        if (fullscreen) {
    //            // backup window position and window size
    //            glfwGetWindowPos(_wnd, &wndPos[0], &wndPos[1]);
    //            glfwGetWindowSize(_wnd, &wndSize[0], &wndSize[1]);
    //
    //            // get resolution of monitor
    //            const GLFWvidmode *mode = glfwGetVideoMode(_monitor);
    //
    //            // switch to full screen
    //            glfwSetWindowMonitor(_wnd, _monitor, 0, 0, mode->width, mode->height, 0);
    //        } else {
    //            // restore last window size and position
    //            glfwSetWindowMonitor(_wnd, nullptr, wndPos[0], wndPos[1], wndSize[0], wndSize[1], 0);
    //        }
    //    }

    APP_WINDOW* init_graphics(int width, int height, const char* title) {
        fApplet->width  = width;
        fApplet->height = height;

        //        /* monitors */
        //        GLFWmonitor *desiredMonitor = nullptr;
        //        if (monitor != DEFAULT) {
        //            int         monitorCount;
        //            GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
        //            for (int    i          = 0; i < monitorCount; ++i) {
        //                const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
        //                std::cout << "+++ monitor " << i << ": " << mode->width << "x" << mode->height << " (" << mode->refreshRate
        //                          << "Hz)"
        //                          << std::endl;
        //            }
        //            if (monitor < monitorCount) {
        //                desiredMonitor = monitors[monitor];
        //            }
        //        }

        /* anti aliasing */
        if (antialiasing > 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
        }

        int mWindowFlags = SDL_WINDOW_OPENGL;
        if (enable_retina_support) {
            mWindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
        }

        //        int major, minor, revision;
        //        glfwGetVersion(&major, &minor, &revision);
        //        std::cout << "+++ OpenGL version: " << major << "." << minor << "." << revision << std::endl;

        //#if USE_CURRENT_OPENGL
        //        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        //        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
        //        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //#endif // USE_CURRENT_OPENGL
        //        glfwWindowHint(GLFW_FOCUSED, true);
        //        glfwWindowHint(GLFW_DECORATED, true);
        //        glfwWindowHint(GLFW_RESIZABLE, resizable);

        APP_WINDOW* window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            fApplet->width,
            fApplet->height,
            SDL_WINDOW_OPENGL);

        if (!window) {
            std::cerr << "+++ error: could not create window: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        glContext = SDL_GL_CreateContext(window);
        if (glContext == nullptr) {
            std::cerr << "+++ error: could not create OpenGL context: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            return nullptr;
        }

        int framebufferWidth, framebufferHeight;
        SDL_GL_GetDrawableSize(window, &framebufferWidth, &framebufferHeight);
        if (fApplet->width != framebufferWidth || fApplet->height != framebufferHeight) {
            std::cout << "+++ retina display detected" << std::endl;
        }
        fApplet->framebuffer_width  = framebufferWidth;
        fApplet->framebuffer_height = framebufferHeight;

        set_default_graphics_state();

        /* initialize GLEW */
        glewExperimental            = GL_TRUE;
        const GLenum glewInitResult = glewInit();
        if (GLEW_OK != glewInitResult) {
            std::cerr << "ERROR: " << glewGetErrorString(glewInitResult) << std::endl;
            return nullptr;
        }

        return window;
    }

    //    /* implement scroll_callback */
    //
    //    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    //        // TODO add `void mouseWheel(MouseEvent event) { ... }`
    //        std::cout << "Scroll: " << xoffset << ", " << yoffset << std::endl;
    //    }


    void handle_shutdown(APP_WINDOW* window) {
        if (!headless) {
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
        }
    }

    void handle_setup(APP_WINDOW* window) {
        if (!headless) {
            if (window != nullptr) {
                SDL_GL_SetSwapInterval(1); // Enable vsync (1 means on, 0 means off)
            }
        }
        fApplet->init();
        if (!headless) {
            fApplet->pre_draw();
            set_default_graphics_state();
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

        if (headless) {
            fApplet->draw();
        } else {
            /* draw */
            fApplet->pre_draw();
            fApplet->draw();
            fApplet->post_draw();

            /* swap front and back buffers */
            SDL_GL_SwapWindow(window);
        }

        /* timer end */
        endTime                                           = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> frameDuration = std::chrono::duration_cast<std::chrono::duration<double>>(
            endTime - startTime);
        const double frameTime = frameDuration.count();
        fApplet->frameRate     = static_cast<float>(1.0 / frameTime);
        fApplet->frameCount++;
        startTime = std::chrono::high_resolution_clock::now();
    }

    void handle_event(const SDL_Event& event, bool& fAppIsRunning, bool& fMouseIsPressed) {
        switch (event.type) {
            case SDL_QUIT:
                fAppIsRunning = false;
                break;
            case SDL_KEYDOWN:
                fApplet->key = event.key.keysym.sym;
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    fAppIsRunning = false;
                } else {
                    fApplet->keyPressed();
                }
                break;
            case SDL_KEYUP:
                fApplet->key = event.key.keysym.sym;
                fApplet->keyReleased();
                break;
            case SDL_MOUSEBUTTONDOWN:
                fApplet->mouseButton = event.button.button;
                fMouseIsPressed      = true;
                fApplet->mousePressed();
                break;
            case SDL_MOUSEBUTTONUP:
                fMouseIsPressed = false;
                fApplet->mouseReleased();
                break;
            case SDL_MOUSEMOTION:
                fApplet->mouseX = static_cast<float>(event.motion.x);
                fApplet->mouseY = static_cast<float>(event.motion.y);
                if (fMouseIsPressed) {
                    fApplet->mouseDragged();
                } else {
                    fApplet->mouseMoved();
                }
                fApplet->pmouseX = fApplet->mouseX;
                fApplet->pmouseY = fApplet->mouseY;
                break;
            case SDL_DROPFILE: {
                char* dropped_filedir = event.drop.file;
                fApplet->dropped(dropped_filedir);
                SDL_free(dropped_filedir);
                break;
            }
            default: break;
        }
    }
} // namespace umgebung
#endif // DISABLE_GRAPHICS
