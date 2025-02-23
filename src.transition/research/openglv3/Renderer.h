#pragma once

#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define __USE_TEXTURE__

// TODO add alpha to vertices

#include "PImage.h"

class Renderer {
public:
    Renderer(const int width, const int height) : currentMatrix(glm::mat4(1.0f)) {
        shaderProgram = build_shader(vertexShaderSource(), fragmentShaderSource());
        init_buffers_vertex_xyz_rgba_uv();
        createDummyTexture();

        glViewport(0, 0, width, height);

        aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        // Orthographic projection
        projection2D = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);

        const float fov            = DEFAULT_FOV;                       // distance from the camera = screen height
        const float cameraDistance = (height / 2.0f) / tan(fov / 2.0f); // 1 unit = 1 pixel

        // Perspective projection
        projection3D = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 10000.0f);

        viewMatrix = glm::lookAt(
            glm::vec3(width / 2.0f, height / 2.0f, -cameraDistance), // Flip Z to fix X-axis
            glm::vec3(width / 2.0f, height / 2.0f, 0.0f),            // Look at the center
            glm::vec3(0.0f, -1.0f, 0.0f)                             // Keep Y-up as normal
        );
    }

    static void loadTexture(const char* file_path,
                            GLuint&     textureID,
                            int&        width,
                            int&        height,
                            int&        channels);


    void pushMatrix() {
        matrixStack.push_back(currentMatrix);
    }

    void popMatrix() {
        if (!matrixStack.empty()) {
            currentMatrix = matrixStack.back();
            matrixStack.pop_back();
        }
    }

    void translate(const float x, const float y, const float z = 0) {
        currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, z));
    }

    void scale(const float sx, const float sy, const float sz = 1) {
        currentMatrix = glm::scale(currentMatrix, glm::vec3(sx, sy, sz));
    }

    void rotate(const float angle) {
        currentMatrix = glm::rotate(currentMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void rotate(const float angle, const glm::vec3 axis) {
        currentMatrix = glm::rotate(currentMatrix, angle, axis);
    }

    void line(const float x1, const float y1, const float x2, const float y2, const glm::vec4 color) {
        addVertex(x1, y1, 0, color.r, color.g, color.b, color.a);
        addVertex(x2, y2, 0, color.r, color.g, color.b, color.a);
        numVertices += 2;
    }

    void print_matrix() {
        printMatrix(currentMatrix);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }

    void image(const PImage& img, float x, float y, float w = -1, float h = -1);

#ifdef __USE_TEXTURE__
    void rect(const float x, const float y, const float w, const float h, const glm::vec4 color) {
        constexpr int RECT_NUM_VERTICES = 6;
        addVertex(x, y, 0, color.r, color.g, color.b, color.a);
        addVertex(x + w, y, 0, color.r, color.g, color.b, color.a);
        addVertex(x + w, y + h, 0, color.r, color.g, color.b, color.a);

        addVertex(x + w, y + h, 0, color.r, color.g, color.b, color.a);
        addVertex(x, y + h, 0, color.r, color.g, color.b, color.a);
        addVertex(x, y, 0, color.r, color.g, color.b, color.a);

        numVertices += RECT_NUM_VERTICES;

        if (renderBatches.empty() || renderBatches.back().textureID != dummyTexture) {
            renderBatches.emplace_back(numVertices - RECT_NUM_VERTICES, RECT_NUM_VERTICES, dummyTexture);
        } else {
            renderBatches.back().numVertices += RECT_NUM_VERTICES;
        }
    }

    void rect_textured(const float x, const float y, const float w, const float h, GLuint textureID) {
        constexpr int  RECT_NUM_VERTICES = 6;
        constexpr auto color             = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        addVertex(x, y, 0, color.r, color.g, color.b, color.a, 0.0f, 0.0f);
        addVertex(x + w, y, 0, color.r, color.g, color.b, color.a, 1.0f, 0.0f);
        addVertex(x + w, y + h, 0, color.r, color.g, color.b, color.a, 1.0f, 1.0f);

        addVertex(x + w, y + h, 0, color.r, color.g, color.b, color.a, 1.0f, 1.0f);
        addVertex(x, y + h, 0, color.r, color.g, color.b, color.a, 0.0f, 1.0f);
        addVertex(x, y, 0, color.r, color.g, color.b, color.a, 0.0f, 0.0f);

        numVertices += RECT_NUM_VERTICES;

        if (renderBatches.empty() || renderBatches.back().textureID != textureID) {
            renderBatches.emplace_back(numVertices - RECT_NUM_VERTICES, RECT_NUM_VERTICES, textureID);
        } else {
            renderBatches.back().numVertices += RECT_NUM_VERTICES;
        }
    }

    void flush() {
        if (numVertices == 0) {
            return;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

        glUseProgram(shaderProgram);

        // Upload matrices
        const GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection3D));

        const GLint viewLoc = glGetUniformLocation(shaderProgram, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        const GLint modelLoc = glGetUniformLocation(shaderProgram, "uModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currentMatrix));

        // Bind textures per batch
        glBindVertexArray(VAO);
        for (const auto& batch: renderBatches) {
            glBindTexture(GL_TEXTURE_2D, batch.textureID);
            glDrawArrays(GL_TRIANGLES, batch.startIndex, batch.numVertices);
            // glDrawArrays(GL_TRIANGLE_FAN, batch.startIndex, batch.numVertices);
        }
        glBindVertexArray(0);

        vertices.clear();
        numVertices = 0;
        renderBatches.clear();
    }

private:
    const char* vertexShaderSource();
    const char* fragmentShaderSource();

    void resizeBuffer(const uint32_t newSize) {
        // Create a new buffer
        GLuint newVBO;
        glGenBuffers(1, &newVBO);
        glBindBuffer(GL_ARRAY_BUFFER, newVBO);
        glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW); // Allocate new size

        // Copy old data to new buffer
        glBindBuffer(GL_COPY_READ_BUFFER, VBO);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, maxBufferSize);

        // Delete old buffer and update references
        glDeleteBuffers(1, &VBO);
        VBO           = newVBO;
        maxBufferSize = newSize;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    const uint8_t NUM_VERTEX_ATTRIBUTES = 9;

    void addVertex(const glm::vec3 position,
                   const glm::vec4 color,
                   const glm::vec2 tex_coords) {
        addVertex(position.x, position.y, position.z,
                  color.r, color.g, color.b, color.a,
                  tex_coords.x, tex_coords.y);
    }

    void addVertex(const float x, const float y, const float z,
                   const float r, const float g, const float b, const float a = 1.0f,
                   const float u = 0.0f, const float v = 0.0f) {
        // Each vertex consists of 9 floats (x, y, z, r, g, b, u, v)
        // uint32_t vertexSize = NUM_VERTEX_ATTRIBUTES * sizeof(float);
        if ((vertices.size() + NUM_VERTEX_ATTRIBUTES) * sizeof(float) > maxBufferSize) {
            resizeBuffer(maxBufferSize + VBO_BUFFER_CHUNK_SIZE);
        }

        const glm::vec4 transformed = currentMatrix * glm::vec4(x, y, z, 1.0f); // Apply transformation

        vertices.push_back(transformed.x); // Position
        vertices.push_back(transformed.y); // Position
        vertices.push_back(transformed.z); // Position
        vertices.push_back(r);             // Color
        vertices.push_back(g);             // Color
        vertices.push_back(b);             // Color
        vertices.push_back(a);             // Color
        vertices.push_back(u);             // Texture
        vertices.push_back(v);             // Texture
    }

    void init_buffers_vertex_xyz_rgba_uv() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Allocate buffer memory (maxBufferSize = 1024 KB) but don't fill it yet
        glBufferData(GL_ARRAY_BUFFER, maxBufferSize, nullptr, GL_DYNAMIC_DRAW);

        const int STRIDE = NUM_VERTEX_ATTRIBUTES * sizeof(float);

        // Position Attribute (Location 0) -> 3 floats (x, y, z)
        constexpr int NUM_POSITION_ATTRIBUTES = 3;
        glVertexAttribPointer(0, NUM_POSITION_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // Color Attribute (Location 1) -> 4 floats (r, g, b, a)
        constexpr int NUM_COLOR_ATTRIBUTES = 4;
        glVertexAttribPointer(1, NUM_COLOR_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, (void*) (NUM_POSITION_ATTRIBUTES * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture Coordinate Attribute (Location 2) -> 2 floats (u, v)
        constexpr int NUM_TEXTURE_ATTRIBUTES = 2;
        glVertexAttribPointer(2, NUM_TEXTURE_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, (void*) ((NUM_POSITION_ATTRIBUTES + NUM_COLOR_ATTRIBUTES) * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
#else
    void flush() {
        if (numVertices == 0) {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

        glUseProgram(shaderProgram);

        // Upload the projection matrix (set this once)
        const GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
        //         glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection2D));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection3D));
        const GLint viewLoc = glGetUniformLocation(shaderProgram, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Upload the model matrix per shape
        const GLint matrixLoc = glGetUniformLocation(shaderProgram, "uModelMatrix");
        glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(currentMatrix));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, numVertices);
        glBindVertexArray(0);

        vertices.clear();
        numVertices = 0;
    }

    void rect(float x, float y, float w, float h, glm::vec3 color) {
        addVertex(x, y, color);
        addVertex(x + w, y, color);
        addVertex(x + w, y, color);
        addVertex(x + w, y + h, color);
        addVertex(x + w, y + h, color);
        addVertex(x, y + h, color);
        addVertex(x, y + h, color);
        addVertex(x, y, color);
        numVertices += 8;
    }

private:
    // Vertex Shader source ( without texture )
    const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";

    // Fragment Shader source ( without texture )
    const char* fragmentShaderSource = R"(
#version 330 core

in vec4 vColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor);
}
)";

    void addVertex(float x, float y, glm::vec3 color) {
        const glm::vec4 transformed = currentMatrix * glm::vec4(x, y, 0.0f, 1.0f); // Apply transformation
        vertices.push_back(transformed.x);
        vertices.push_back(transformed.y);
        vertices.push_back(transformed.z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
#endif

private:
    struct RenderBatch {
        int    startIndex;
        int    numVertices;
        GLuint textureID;

        RenderBatch(const int start, const int count, const GLuint texID)
            : startIndex(start), numVertices(count), textureID(texID) {}
    };

    GLuint                   VAO = 0;
    GLuint                   VBO = 0;
    GLuint                   shaderProgram{};
    GLuint                   dummyTexture{};
    glm::mat4                currentMatrix;
    std::vector<glm::mat4>   matrixStack;
    std::vector<float>       vertices;
    int                      numVertices = 0;
    float                    aspectRatio;
    glm::mat4                projection2D{};
    glm::mat4                projection3D{};
    glm::mat4                viewMatrix{};
    std::vector<RenderBatch> renderBatches;
    glm::vec4                fill_color     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool                     fill_enabled   = true;
    glm::vec4                stroke_color   = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    bool                     stroke_enabled = true;

    const uint32_t VBO_BUFFER_CHUNK_SIZE = 1024 * 1024;           // 1MB
    const float    DEFAULT_FOV           = 2.0f * atan(0.5f);     // = 53.1301f;
    uint32_t       maxBufferSize         = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)

    void createDummyTexture() {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        constexpr unsigned char whitePixel[4] = {255, 255, 255, 255}; // RGBA: White
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        dummyTexture = textureID;
    }

    void printMatrix(const glm::mat4& matrix) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::cout << matrix[j][i] << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    GLuint build_shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
        // Build shaders
        const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        checkShaderCompileStatus(vertexShader);

        const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        checkShaderCompileStatus(fragmentShader);

        GLuint mShaderProgram = glCreateProgram();
        glAttachShader(mShaderProgram, vertexShader);
        glAttachShader(mShaderProgram, fragmentShader);
        glLinkProgram(mShaderProgram);
        checkProgramLinkStatus(mShaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return mShaderProgram;
    }

    void checkShaderCompileStatus(const GLuint shader) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    void checkProgramLinkStatus(const GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
    }
};
