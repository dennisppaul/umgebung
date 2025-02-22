#include <SDL3/SDL.h>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "Renderer.h"

// Window dimensions
static const int width  = 800;
static const int height = 600;

// Vertex Shader source
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 uProjection;   // Can be either perspective or ortho
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";

// Fragment Shader source
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;       // Interpolated color from vertex shader
out vec4 FragColor;   // Final fragment color

void main() {
    FragColor = vec4(vColor, 1.0); // Set the fragment color
}
)";

void checkShaderCompileStatus(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void checkProgramLinkStatus(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

glm::mat4 translate(const glm::mat4& model, const glm::vec3& offset) { return glm::translate(model, offset); }

glm::mat4 scale(const glm::mat4& model, const glm::vec3& factors) { return glm::scale(model, factors); }

glm::mat4 rotate(const glm::mat4& model, float angleInDegrees, const glm::vec3& axis) {
    float angleInRadians = glm::radians(angleInDegrees);
    return glm::rotate(model, angleInRadians, axis);
}

// void resizeBuffer(GLuint buffer, size_t newSize, std::vector<float>& vertexData) {
//     glBindBuffer(GL_ARRAY_BUFFER, buffer);
//
//     // Create a new buffer with the larger size
//     GLuint newBuffer;
//     glGenBuffers(1, &newBuffer);
//     glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
//     glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
//
//     // Copy the old data
//     glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), vertexData.data());
//
//     // Delete the old buffer
//     glDeleteBuffers(1, &buffer);
//
//     // Update buffer reference
//     buffer = newBuffer;
// }
//
// void resizeBuffer(size_t newSize) {
//     std::vector<float> newVertices;
//     newVertices.reserve(newSize);
//
//     // Copy existing vertices to the new buffer
//     newVertices.insert(newVertices.end(), vertices.begin(), vertices.end());
//
//     // Swap the old buffer with the new buffer
//     vertices.swap(newVertices);
//     maxBufferSize = newSize * sizeof(float);
// }
//
// void addVertex(float x, float y, glm::vec3 color) {
//     if (vertices.size() + 6 > maxBufferSize / sizeof(float)) {
//         resizeBuffer(vertices.size() * 1.5); // Resize to 1.5x current size
//     }
//
//     vertices.push_back(x);
//     vertices.push_back(y);
//     vertices.push_back(0.0f); // z
//     vertices.push_back(color.r);
//     vertices.push_back(color.g);
//     vertices.push_back(color.b);
//     numVertices++;
// }
//
// void addVertex(float x, float y, glm::vec3 color) {
//     if (vertices.size() + 6 > maxBufferSize / sizeof(float)) {
//         flush(); // Flush when buffer is full
//     }
//
//     vertices.push_back(x);
//     vertices.push_back(y);
//     vertices.push_back(0.0f); // z
//     vertices.push_back(color.r);
//     vertices.push_back(color.g);
//     vertices.push_back(color.b);
//     numVertices++;
// }
//
// void flush() {
//     if (numVertices == 0) return;
//
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
//
//     glUseProgram(shaderProgram);
//     GLint matrixLoc = glGetUniformLocation(shaderProgram, "uModelMatrix");
//     glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(currentMatrix));
//
//     glBindVertexArray(VAO);
//     glDrawArrays(GL_LINES, 0, numVertices);
//     glBindVertexArray(0);
//
//     vertices.clear();
//     numVertices = 0;
// }

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set OpenGL attributes
    //     SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //     SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

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

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glViewport(0, 0, width, height);

    // Build shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinkStatus(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Renderer renderer(shaderProgram, width, height);

    //     // Triangle data
    //     float vertices[] = {
    //         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
    //        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
    //         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    //     };
    //
    //     // Setup VAO and VBO
    //     GLuint VAO, VBO;
    //     glGenVertexArrays(1, &VAO);
    //     glGenBuffers(1, &VBO);
    //
    //     glBindVertexArray(VAO);
    //
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //
    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //     glEnableVertexAttribArray(0);
    //
    //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //     glEnableVertexAttribArray(1);
    //
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     glBindVertexArray(0);
    //
    //     // Main loop
    //     glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
    //     modelMatrix = translate(modelMatrix, glm::vec3(0.5f, 0.0f, 0.0f));
    //     modelMatrix = rotate(modelMatrix, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    //     modelMatrix = scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    bool      running = true;
    int       mouseX  = 0;
    int       mouseY  = 0;
    SDL_Event event;
    float     rotations = 0.0f;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN) { running = false; }
            if (SDL_EVENT_MOUSE_MOTION) {
                mouseX = static_cast<float>(event.motion.x);
                mouseY = static_cast<float>(event.motion.y);
            }
        }

        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start a frame
        // float aspectRatio = (float)width / (float)height;
        // renderer.pushMatrix();
        // renderer.translate(
        //     ((float)mouseX / (float)width) * 2 - 1,
        //     (1.0 - ((float)mouseY / (float)height)) * 2 - 1);
        // renderer.scale(1.0f / aspectRatio, 1.0f); // Fix distortion
        // rotations += 0.01f;
        // renderer.rotate(rotations);
        // renderer.rect(-0.5f, -0.5f, 1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        // renderer.popMatrix();

        renderer.pushMatrix();

        renderer.translate(mouseX, mouseY);
        // renderer.translate(
        //     ((float)mouseX / (float)width) * 2 - 1,
        //     (1.0 - ((float)mouseY / (float)height)) * 2 - 1);
        rotations += 0.01f;
        renderer.rotate(glm::radians(rotations));
        // renderer.print_matrix();
        renderer.line(-20, -20, 20, 20, glm::vec3(1.0f, 0.0f, 0.0f));
        renderer.rect(-30, -30, 60, 60, glm::vec3(0.0f, 1.0f, 0.0f));
        renderer.rotate(3.141f / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        renderer.translate(30, 0, 30);
        renderer.rect(-30, -30, 60, 60, glm::vec3(0.0f, 0.0f, 1.0f));
        renderer.line(-20, -20, 20, 20, glm::vec3(1.0f, 0.0f, 0.0f));
        renderer.popMatrix();

        // Render everything
        renderer.flush();

        //         // Render the triangle
        //         glUseProgram(shaderProgram);
        //         GLuint modelLoc = glGetUniformLocation(shaderProgram, "uModelMatrix");
        //         glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        //
        //         glBindVertexArray(VAO);
        //         glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    //     glDeleteVertexArrays(1, &VAO);
    //     glDeleteBuffers(1, &VBO);
    renderer.cleanup();
    glDeleteProgram(shaderProgram);
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}