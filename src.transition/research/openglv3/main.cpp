#include <vector>
#include <iostream>

// #define IMGUI_IMPL_OPENGL_LOADER_GLEW
// #include "imgui.h"
// #include "imgui_impl_sdl3.h"
// #include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include "glm/glm.hpp"

#include "PGraphicsOpenGLv33.h"
#include "PImage.h"

static constexpr int width  = 800;
static constexpr int height = 600;
int                  mouseX = 0;
int                  mouseY = 0;

PImage* image;
float   rotations = 0.0f;

void setup() {
    image = new PImage("../256.png");
}

void pre_draw() {
    // Clear screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw(PGraphicsOpenGLv33& renderer) {
    renderer.noStroke();
    renderer.fill(1.0f, 1.0f, 1.0f, 0.5f);
    renderer.image(image, 10, 10);

    renderer.pushMatrix();
    renderer.translate(mouseX, mouseY);

    // rotations += 0.01f;
    // renderer.rotate(glm::radians(rotations));
    // renderer.fill(1.0f, 0.5f, 0.0f, 0.5f);
    // renderer.rect(-20, -20, 40, 40);
    // renderer.fill(1.0f, 1.0f, 1.0f, 1.0f);
    // renderer.image(image, -15, -15, 30, 30);

    renderer.noFill();
    renderer.stroke(1.0f, 0.0f, 0.0f);
    renderer.line(-20, -20, 20, 20);
    renderer.line(-20, 20, 20, -20);
    renderer.rect(-30, -30, 60, 60);

    renderer.stroke(0.0f, 0.0f, 1.0f);
    renderer.rotate(3.141f / 2.0f, glm::vec3(0, 1, 0));
    renderer.translate(15, 0, 0);
    renderer.line(-20, -20, 20, 20);
    renderer.line(-20, 20, 20, -20);
    renderer.rect(-30, -30, 60, 60);

    renderer.popMatrix();
}

void post_draw(PGraphicsOpenGLv33& renderer) {
    renderer.flush_fill();
    renderer.flush_stroke();
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // @TODO check number of buffers
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("SDL3 + OpenGL",
                                          width, height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_ShowWindow(window);

    const SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    //
    // // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    //
    // // // Initialize SDL3 + OpenGL backend
    // ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    // ImGui_ImplOpenGL3_Init("#version 330 core");

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    PGraphicsOpenGLv33  renderer(width, height);
    SDL_Event event;
    bool      running = true;

    setup();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN) { running = false; }
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                mouseX = static_cast<int>(event.motion.x);
                mouseY = static_cast<int>(event.motion.y);
            }
        }

        pre_draw();
        draw(renderer);
        post_draw(renderer);

        // // Start ImGui frame
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplSDL3_NewFrame();
        // ImGui::NewFrame();
        //
        // // Draw ImGui UI
        // ImGui::Begin("Debug Window");
        // ImGui::Text("Hello, SDL3 + OpenGL 3.3!");
        // ImGui::End();
        //
        // // Render ImGui
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplSDL3_Shutdown();
    // ImGui::DestroyContext();
    renderer.cleanup();
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}