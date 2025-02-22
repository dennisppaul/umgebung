#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define __USE_TEXTURE__

// TODO add alpha to vertices

class Renderer {
public:
    Renderer(const int width, const int height) : currentMatrix(glm::mat4(1.0f)) {
        build_shader();
        initBuffers();
        createDummyTexture();

        glViewport(0, 0, width, height);

        aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        // Orthographic projection
        projection2D = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);

        const float fov            = glm::radians(90.0f);
        const float cameraDistance = (height / 2.0f) / tan(fov / 2.0f);

        // Perspective projection
        projection3D = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 10000.0f);

        viewMatrix = glm::lookAt(
            glm::vec3(width / 2.0f, height / 2.0f, -cameraDistance), // Flip Z to fix X-axis
            glm::vec3(width / 2.0f, height / 2.0f, 0.0f),            // Look at the center
            glm::vec3(0.0f, -1.0f, 0.0f)                             // Keep Y-up as normal
        );
    }

    GLuint loadTexture(const char* filePath) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image data
        int            width, height, nrChannels;
        unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 4);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Failed to load texture: " << filePath << std::endl;
        }
        stbi_image_free(data);

        return textureID;
    }

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

    void line(const float x1, const float y1, const float x2, const float y2, const glm::vec3 color) {
        addVertex(x1, y1, color);
        addVertex(x2, y2, color);
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

#ifdef __USE_TEXTURE__
    void rect(const float x, const float y, const float w, const float h, const glm::vec3 color) {
        constexpr int RECT_NUM_VERTICES = 6;
        addVertex(x, y, color);
        addVertex(x + w, y, color);
        addVertex(x + w, y + h, color);

        addVertex(x + w, y + h, color);
        addVertex(x, y + h, color);
        addVertex(x, y, color);

        numVertices += RECT_NUM_VERTICES;

        if (renderBatches.empty() || renderBatches.back().textureID != dummyTexture) {
            renderBatches.emplace_back(numVertices - RECT_NUM_VERTICES, RECT_NUM_VERTICES, dummyTexture);
        } else {
            renderBatches.back().numVertices += RECT_NUM_VERTICES;
        }
    }

    void rect_textured(const float x, const float y, const float w, const float h, GLuint textureID) {
        constexpr int RECT_NUM_VERTICES = 6;
        addVertex(x, y, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
        addVertex(x + w, y, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f);
        addVertex(x + w, y + h, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f);

        addVertex(x + w, y + h, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f);
        addVertex(x, y + h, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 1.0f);
        addVertex(x, y, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);

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
            glDrawArrays(GL_TRIANGLE_FAN, batch.startIndex, batch.numVertices);
        }
        glBindVertexArray(0);

        vertices.clear();
        numVertices = 0;
        renderBatches.clear(); // Clear for the next frame
    }

private:
    // Vertex Shader source
    const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
    vTexCoord = aTexCoord;
}
)";

    // Fragment Shader source
    const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;  // Bound texture

void main() {
    FragColor = texture(uTexture, vTexCoord) * vec4(vColor, 1.0);
}
)";

    void resizeBuffer(const size_t newSize) {
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

    void addVertex(const float x, const float y, const glm::vec3 color, const float u = 0.0f, const float v = 0.0f) {
        // Each vertex consists of 8 floats (x, y, z, r, g, b, u, v)
        constexpr size_t vertexSize = 8 * sizeof(float);
        if ((vertices.size() + 8) * sizeof(float) > maxBufferSize) {
            resizeBuffer(maxBufferSize * 1.5); // Increase size by 1.5x
        }

        const glm::vec4 transformed = currentMatrix * glm::vec4(x, y, 0.0f, 1.0f); // Apply transformation

        vertices.push_back(transformed.x); // Position
        vertices.push_back(transformed.y); // Position
        vertices.push_back(transformed.z); // Position
        vertices.push_back(color.r);       // Color
        vertices.push_back(color.g);       // Color
        vertices.push_back(color.b);       // Color
        vertices.push_back(u);             // Texture
        vertices.push_back(v);             // Texture
    }

    void addVertex(const float x, const float y, const float z,
                   const float r, const float g, const float b, const float a = 1.0f,
                   const float u = 0.0f, const float v = 0.0f) {
        // Each vertex consists of 8 floats (x, y, z, r, g, b, u, v)
        size_t vertexSize = 8 * sizeof(float);
        if ((vertices.size() + 8) * sizeof(float) > maxBufferSize) {
            resizeBuffer(maxBufferSize * 1.5); // Increase size by 1.5x
        }

        const glm::vec4 transformed = currentMatrix * glm::vec4(x, y, z, 1.0f); // Apply transformation

        vertices.push_back(transformed.x); // Position
        vertices.push_back(transformed.y); // Position
        vertices.push_back(transformed.z); // Position
        vertices.push_back(r);             // Color
        vertices.push_back(g);             // Color
        vertices.push_back(b);             // Color
        // vertices.push_back(a);             // Color // TODO add this
        vertices.push_back(u); // Texture
        vertices.push_back(v); // Texture
    }

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Allocate buffer memory (maxBufferSize = 1024 KB) but don't fill it yet
        glBufferData(GL_ARRAY_BUFFER, maxBufferSize, nullptr, GL_DYNAMIC_DRAW);

        // Position Attribute (Location 0) -> 3 floats (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        // Color Attribute (Location 1) -> 3 floats (r, g, b)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture Coordinate Attribute (Location 2) -> 2 floats (u, v)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
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

uniform mat4 uProjection;   // Can be either perspective or ortho
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";

    // Fragment Shader source ( without texture )
    const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;       // Interpolated color from vertex shader
out vec4 FragColor;   // Final fragment color

void main() {
    FragColor = vec4(vColor, 1.0); // Set the fragment color
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
    GLuint                   shaderProgram;
    GLuint                   dummyTexture;
    glm::mat4                currentMatrix;
    std::vector<glm::mat4>   matrixStack;
    std::vector<float>       vertices;
    int                      numVertices = 0;
    float                    aspectRatio;
    glm::mat4                projection2D;
    glm::mat4                projection3D;
    glm::mat4                viewMatrix;
    std::vector<RenderBatch> renderBatches;

    size_t maxBufferSize = 1024 * 1024; // Initial size (1MB)

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

    void build_shader() {
        // Build shaders
        const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        checkShaderCompileStatus(vertexShader);

        const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        checkShaderCompileStatus(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        checkProgramLinkStatus(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
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
