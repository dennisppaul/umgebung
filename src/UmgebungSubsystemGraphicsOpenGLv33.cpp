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

UMGEBUNG_NAMESPACE_BEGIN
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

static void set_default_graphics_state() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

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

    set_default_graphics_state();

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

    int current_framebuffer_width;
    int current_framebuffer_height;
    SDL_GetWindowSizeInPixels(window, &current_framebuffer_width, &current_framebuffer_height);
    int current_framebuffer_width_in_pixel;
    int current_framebuffer_height_in_pixel;
    SDL_GetWindowSizeInPixels(window, &current_framebuffer_width_in_pixel, &current_framebuffer_height_in_pixel);
    framebuffer_width   = static_cast<float>(current_framebuffer_width);
    framebuffer_height  = static_cast<float>(current_framebuffer_height);
    float pixel_density = SDL_GetWindowPixelDensity(window);

    console("main renderer      : ", g->name());
    console("render to offscreen: ", g->render_to_offscreen ? "true" : "false");
    console("framebuffer size   : ", framebuffer_width, " x ", framebuffer_height);
    console("framebuffer size px: ", current_framebuffer_width_in_pixel, " x ", current_framebuffer_height_in_pixel);
    console("graphics size      : ", width, " x ", height);
    console("pixel_density      : ", pixel_density);
    // console("( note that if these do not align the pixel density might not be 1 )");

    pixelHeight = static_cast<int>(framebuffer_height / height);
    pixelWidth  = static_cast<int>(framebuffer_width / width);

    g->init(nullptr, static_cast<int>(framebuffer_width), static_cast<int>(framebuffer_height), 0, false);
    g->width  = static_cast<int>(width);
    g->height = static_cast<int>(height);
    set_default_graphics_state();
    draw_pre();
    // <<< NOTE this is identical with the other OpenGL renderer

    if (g->render_to_offscreen) {
        init_FBO_drawing();
    }
}

static void setup_post() {
    checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGLv33::setup_post");
    draw_post();
}

static void draw_pre() {
    if (g->render_to_offscreen && g->framebuffer.id > 0) {
        // NOTE if `g->framebuffer.id` is `0` i.e not initialized
        //      the bound buffer is the default color buffer … hmmm
        glBindFramebuffer(GL_FRAMEBUFFER, g->framebuffer.id);
    }
    g->beginDraw();
    checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGLv33::draw_pre");
}

static void draw_post() {
    checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGLv33::draw");

    if (window == nullptr) {
        return;
    }

    if (g == nullptr) {
        return;
    }

    g->endDraw();

    if (g->render_to_offscreen && g->framebuffer.id > 0) {
        // NOTE if `g->framebuffer.id` is `0` the framebuffer has not been initialized and nothing needs to be done
        if (blit_framebuffer_object_to_screenbuffer) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);                      // Unbind FBO
            glBindFramebuffer(GL_READ_FRAMEBUFFER, g->framebuffer.id); // Bind the FBO as the source
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);                 // Bind the default framebuffer (screen) as the destination
            glBlitFramebuffer(0, 0, g->framebuffer.width, g->framebuffer.height,
                              0, 0, g->framebuffer.width, g->framebuffer.height,
                              GL_COLOR_BUFFER_BIT, GL_LINEAR);
            // GL_COLOR_BUFFER_BIT, GL_NEAREST); // TODO does this also work? i.e is it good enough?
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        } else {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); // Unbind FBO
                                                           // Disable depth testing and blending
            GL_CALL(glDisable(GL_DEPTH_TEST));
            GL_CALL(glDisable(GL_BLEND));

            // Use shader
            GL_CALL(glUseProgram(shaderProgram));
            GL_CALL(glBindVertexArray(screenVAO));

            // Bind FBO texture and set uniform
            GL_CALL(glActiveTexture(GL_TEXTURE0));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, g->framebuffer.texture_id));
            GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "screenTexture"), 0));

            // Draw fullscreen quad
            GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

            GL_CALL(glBindVertexArray(0));
            GL_CALL(glUseProgram(0));
        }
    }

    checkOpenGLError("SUBSYSTEM_GRAPHICS_OPENGLv33::draw_post");
    SDL_GL_SwapWindow(window);
}

static void shutdown() {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
}

static void set_flags(uint32_t& subsystem_flags) {
    subsystem_flags |= SDL_INIT_VIDEO;
}

static PGraphics* create_graphics(const bool render_to_offscreen) {
    return new PGraphicsOpenGLv33(render_to_offscreen);
}

UMGEBUNG_NAMESPACE_END

umgebung::SubsystemGraphics* umgebung_subsystem_graphics_create_openglv33() {
    auto* graphics            = new umgebung::SubsystemGraphics{};
    graphics->init            = umgebung::init;
    graphics->setup_pre       = umgebung::setup_pre;
    graphics->setup_post      = umgebung::setup_post;
    graphics->draw_pre        = umgebung::draw_pre;
    graphics->draw_post       = umgebung::draw_post;
    graphics->shutdown        = umgebung::shutdown;
    graphics->set_flags       = umgebung::set_flags;
    graphics->create_graphics = umgebung::create_graphics;
    return graphics;
}
