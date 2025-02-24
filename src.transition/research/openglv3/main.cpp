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

#include "PVector.h"
#include "PMatrix2D.h"
#include "PMatrix3D.h"

PVector   v(10, 20, 30);
PMatrix2D m2;
PMatrix3D m3;

static constexpr int width  = 800;
static constexpr int height = 600;
int                  mouseX = 0;
int                  mouseY = 0;

PImage* image;
float   rotations = 0.0f;

void testPVector() {
    std::cout << "=== Testing PVector ===" << std::endl;

    PVector v1(1.0f, 2.0f, 3.0f);
    std::cout << "v1: (" << v1.x << ", " << v1.y << ", " << v1.z << ")" << std::endl;

    PVector v2 = v1.copy();
    std::cout << "v2 (copy of v1): (" << v2.x << ", " << v2.y << ", " << v2.z << ")" << std::endl;

    v2.set(4.0f, 5.0f, 6.0f);
    std::cout << "v2 after set(): (" << v2.x << ", " << v2.y << ", " << v2.z << ")" << std::endl;

    PVector v3;
    PVector::add(v1, v2, v3);
    std::cout << "v3 (v1 + v2): (" << v3.x << ", " << v3.y << ", " << v3.z << ")" << std::endl;

    PVector v4;
    PVector::mult(v1, 2.0f, v4);
    std::cout << "v4 (v1 * 2): (" << v4.x << ", " << v4.y << ", " << v4.z << ")" << std::endl;

    v1.add(v2);
    std::cout << "v1 after += v2: (" << v1.x << ", " << v1.y << ", " << v1.z << ")" << std::endl;

    std::cout << std::endl;
}

void testPMatrix2D() {
    std::cout << "=== Testing PMatrix2D ===" << std::endl;

    PMatrix2D m1;
    std::cout << "m1 (default, should be identity matrix):\n";
    std::cout << m1.m00 << " " << m1.m01 << " " << m1.m02 << "\n";
    std::cout << m1.m10 << " " << m1.m11 << " " << m1.m12 << "\n";

    PMatrix2D m2(1, 2, 3, 4, 5, 6);
    std::cout << "m2 (custom values):\n";
    std::cout << m2.m00 << " " << m2.m01 << " " << m2.m02 << "\n";
    std::cout << m2.m10 << " " << m2.m11 << " " << m2.m12 << "\n";

    PMatrix2D m3(m2);
    std::cout << "m3 (copy of m2):\n";
    std::cout << m3.m00 << " " << m3.m01 << " " << m3.m02 << "\n";
    std::cout << m3.m10 << " " << m3.m11 << " " << m3.m12 << "\n";

    m3.reset();
    std::cout << "m3 after reset (should be identity matrix):\n";
    std::cout << m3.m00 << " " << m3.m01 << " " << m3.m02 << "\n";
    std::cout << m3.m10 << " " << m3.m11 << " " << m3.m12 << "\n";

    std::cout << std::endl;
}

void testPMatrix3D() {
    std::cout << "=== Testing PMatrix3D ===" << std::endl;

    PMatrix3D m1;
    std::cout << "m1 (default, should be identity matrix):\n";
    std::cout << m1.m00 << " " << m1.m01 << " " << m1.m02 << " " << m1.m03 << "\n";
    std::cout << m1.m10 << " " << m1.m11 << " " << m1.m12 << " " << m1.m13 << "\n";
    std::cout << m1.m20 << " " << m1.m21 << " " << m1.m22 << " " << m1.m23 << "\n";
    std::cout << m1.m30 << " " << m1.m31 << " " << m1.m32 << " " << m1.m33 << "\n";

    PMatrix3D m2(m1);
    std::cout << "m2 (copy of m1):\n";
    std::cout << m2.m00 << " " << m2.m01 << " " << m2.m02 << " " << m2.m03 << "\n";
    std::cout << m2.m10 << " " << m2.m11 << " " << m2.m12 << " " << m2.m13 << "\n";
    std::cout << m2.m20 << " " << m2.m21 << " " << m2.m22 << " " << m2.m23 << "\n";
    std::cout << m2.m30 << " " << m2.m31 << " " << m2.m32 << " " << m2.m33 << "\n";

    m2.reset();
    std::cout << "m2 after reset (should be identity matrix):\n";
    std::cout << m2.m00 << " " << m2.m01 << " " << m2.m02 << " " << m2.m03 << "\n";
    std::cout << m2.m10 << " " << m2.m11 << " " << m2.m12 << " " << m2.m13 << "\n";
    std::cout << m2.m20 << " " << m2.m21 << " " << m2.m22 << " " << m2.m23 << "\n";
    std::cout << m2.m30 << " " << m2.m31 << " " << m2.m32 << " " << m2.m33 << "\n";

    std::cout << std::endl;
}

void setup() {
    // testPVector();
    // testPMatrix2D();
    // testPMatrix3D();
    image = PGraphicsOpenGLv33::loadImage("../256.png");
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

    PGraphicsOpenGLv33 renderer(width, height);
    SDL_Event          event;
    bool               running = true;

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