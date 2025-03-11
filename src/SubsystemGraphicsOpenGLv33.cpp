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

#include "Umgebung.h"
#include "PGraphicsOpenGL.h"
#include "PGraphicsOpenGLv33.h"

namespace umgebung {
    static void       setup_pre();
    static void       setup_post();
    static void       draw_pre();
    static void       draw_post();
    static void       shutdown();
    static void       set_flags(uint32_t& subsystem_flags);
    static PGraphics* create_graphics(bool render_to_offscreen);

    static bool blit_framebuffer_object_to_screenbuffer = true; // NOTE FBO is BLITted directly into the color buffer instead of rendered with a textured quad

    static SDL_Window*   window     = nullptr;
    static SDL_GLContext gl_context = nullptr;

    static void center_display() {
        int mDisplayLocation;
        if (display == DEFAULT) {
            mDisplayLocation = SDL_WINDOWPOS_CENTERED;
        } else {
            int mNumDisplays = 0;
            SDL_GetDisplays(&mNumDisplays);
            if (display >= mNumDisplays) {
                error("display index '", display, "' out of range. ",
                      mNumDisplays, " display", mNumDisplays > 1 ? "s are" : " is",
                      " available. using default display.");
                mDisplayLocation = SDL_WINDOWPOS_CENTERED;
            } else {
                mDisplayLocation = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
            }
        }
        SDL_SetWindowPosition(window, mDisplayLocation, mDisplayLocation);
    }

    /* --- draw FBO --- */

    static auto vertexShaderSrc = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

    static auto fragmentShaderSrc = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D screenTexture;
    void main() {
        FragColor = texture(screenTexture, TexCoord);
    }
)";

    static GLuint screenVAO, screenVBO;
    static GLuint shaderProgram;

    GLuint compileShader(const GLenum type, const char* source) {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            error("Shader Compilation Error: ", infoLog);
        }
        return shader;
    }

    static void init_FBO_drawing() {
        // Compile shaders
        const GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
        const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

        // Link shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Shader Linking Error: " << infoLog << std::endl;
        }

        // Clean up shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Define fullscreen quad vertices with texture coordinates
        constexpr float quadVertices[] = {
            // Pos      // Tex Coords
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};

        // Generate VAO and VBO
        glGenVertexArrays(1, &screenVAO);
        glGenBuffers(1, &screenVBO);

        glBindVertexArray(screenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    static bool init() { // TODO maybe merge v2.0 & v3.3 they are identical except for SDL_GL_CONTEXT_PROFILE_MASK + SDL_GL_CONTEXT_MAJOR_VERSION + SDL_GL_CONTEXT_MINOR_VERSION
        // NOTE this is identical with the other OpenGL renderer >>>
        /* setup opengl */

        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // always required on Mac?
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        if (antialiasing > 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing);
        }

        /* window */

        SDL_WindowFlags flags = SDL_WINDOW_OPENGL;
        window                = SDL_CreateWindow(get_window_title().c_str(),
                                                 static_cast<int>(umgebung::width),
                                                 static_cast<int>(umgebung::height),
                                                 get_SDL_WindowFlags(flags));
        if (window == nullptr) {
            error("Couldn't create window: ", SDL_GetError());
            return false;
        }

        center_display();

        /* create opengl context */

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr) {
            error("Couldn't create OpenGL context: ", SDL_GetError());
            SDL_DestroyWindow(window);
            return false;
        }

        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);

        /* display window */

        SDL_ShowWindow(window);

        PGraphicsOpenGL::set_default_graphics_state();

        /* initialize GLEW */

        glewExperimental            = GL_TRUE;
        const GLenum glewInitResult = glewInit();
        if (GLEW_OK != glewInitResult) {
            error("problem initializing GLEW: ", glewGetErrorString(glewInitResult));
            SDL_GL_DestroyContext(gl_context);
            SDL_DestroyWindow(window);
            return false;
        }

        query_opengl_capabilities(open_gl_capabilities);

        return true;
        // <<< NOTE this is identical with the other OpenGL renderer
    }

    static void setup_pre() {
        // NOTE this is identical with the other OpenGL renderer >>>
        if (g == nullptr) {
            return;
        }

        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_pre(begin)");

        int current_framebuffer_width;
        int current_framebuffer_height;
        SDL_GetWindowSizeInPixels(window, &current_framebuffer_width, &current_framebuffer_height);
        int current_framebuffer_width_in_pixel;
        int current_framebuffer_height_in_pixel;
        SDL_GetWindowSizeInPixels(window, &current_framebuffer_width_in_pixel, &current_framebuffer_height_in_pixel);
        framebuffer_width         = static_cast<float>(current_framebuffer_width);
        framebuffer_height        = static_cast<float>(current_framebuffer_height);
        const float pixel_density = SDL_GetWindowPixelDensity(window);

        console("main renderer      : ", g->name());
        console("render to offscreen: ", g->render_to_offscreen ? "true" : "false");
        console("framebuffer size   : ", framebuffer_width, " x ", framebuffer_height);
        console("framebuffer size px: ", current_framebuffer_width_in_pixel, " x ", current_framebuffer_height_in_pixel);
        console("graphics size      : ", width, " x ", height);
        console("pixel_density      : ", pixel_density);
        g->pixelDensity(pixel_density); // NOTE setting pixel density from configuration

        pixelHeight = static_cast<int>(framebuffer_height / height);
        pixelWidth  = static_cast<int>(framebuffer_width / width);

        g->init(nullptr, static_cast<int>(framebuffer_width), static_cast<int>(framebuffer_height), 0, false);
        g->width  = static_cast<int>(width);
        g->height = static_cast<int>(height);
        g->lock_init_properties(true);

        PGraphicsOpenGL::set_default_graphics_state();
        draw_pre();
        // <<< NOTE this is identical with the other OpenGL renderer

        if (g->render_to_offscreen && !blit_framebuffer_object_to_screenbuffer) {
            init_FBO_drawing(); // NOTE this is only necessary if FBO is not blitted
        }

        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_pre(end)");
    }

    static void setup_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::setup_post");
        draw_post();
    }

    static void draw_pre() {
        if (g == nullptr) {
            return;
        }

        g->beginDraw();
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_pre");
    }

    static void draw_post() {
        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw");

        if (window == nullptr || g == nullptr) {
            return;
        }

        g->endDraw();

        if (g->render_to_offscreen && g->framebuffer.id > 0) {
            g->render_framebuffer_to_screen(blit_framebuffer_object_to_screenbuffer);
        }

        checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGL::draw_post");
        SDL_GL_SwapWindow(window);
    }

    static void shutdown() {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
    }

    static void set_flags(uint32_t& subsystem_flags) {
        subsystem_flags |= SDL_INIT_VIDEO;
    }

    static void event(SDL_Event* event) {}

    static PGraphics* create_graphics(const bool render_to_offscreen) {
        return new PGraphicsOpenGLv33(render_to_offscreen);
    }

} // namespace umgebung

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_openglv33() {
    auto* graphics            = new umgebung::SubsystemGraphics{};
    graphics->set_flags       = umgebung::set_flags;
    graphics->init            = umgebung::init;
    graphics->setup_pre       = umgebung::setup_pre;
    graphics->setup_post      = umgebung::setup_post;
    graphics->draw_pre        = umgebung::draw_pre;
    graphics->draw_post       = umgebung::draw_post;
    graphics->shutdown        = umgebung::shutdown;
    graphics->event           = umgebung::event;
    graphics->create_graphics = umgebung::create_graphics;
    return graphics;
}
