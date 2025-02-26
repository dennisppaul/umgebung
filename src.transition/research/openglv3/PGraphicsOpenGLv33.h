#pragma once

#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define __USE_TEXTURE__

#include "PImage.h"

class PGraphicsOpenGLv33 {
public:
    static PImage* loadImage(const char* file_path);
    static bool    upload_texture(PImage* image);
    static void    delete_texture(GLuint textureID);
    static void    bind_texture(GLuint textureID);

    static bool generate_mipmap;
    int         width;
    int         height;

    PGraphicsOpenGLv33(const int width, const int height) : width(width),
                                                            height(height),
                                                            currentMatrix(glm::mat4(1.0f)) {
        stroke_shader_program = build_shader(vertex_shader_source_simple(), fragment_shader_source_simple());
        init_stroke_vertice_buffers();
        fill_shader_program = build_shader(vertex_shader_source_texture(), fragment_shader_source_texture());
        init_fill_vertice_buffers();
        createDummyTexture();

        glViewport(0, 0, width, height);

        aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        // Orthographic projection
        projection2D = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);

        const float fov            = DEFAULT_FOV;                                           // distance from the camera = screen height
        const float cameraDistance = (static_cast<float>(height) / 2.0f) / tan(fov / 2.0f); // 1 unit = 1 pixel

        // Perspective projection
        projection3D = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 10000.0f);

        viewMatrix = glm::lookAt(
            glm::vec3(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f, -cameraDistance), // Flip Z to fix X-axis
            glm::vec3(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f, 0.0f),            // Look at the center
            glm::vec3(0.0f, -1.0f, 0.0f)                                                                     // Keep Y-up as normal
        );
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

    void line(const float x1, const float y1, const float x2, const float y2) {
        if (stroke_enabled) {
            add_stroke_vertex_xyz_rgba(x1, y1, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
            add_stroke_vertex_xyz_rgba(x2, y2, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        }
    }

    void linse(const float x1, const float y1, const float x2, const float y2) {
        line(x1, y1, x2, y2);
    }

    void fill(const float r, const float g, const float b, const float a = 1.0f) {
        fill_enabled = true;
        fill_color   = glm::vec4(r, g, b, a);
    }

    void noFill() {
        fill_enabled = false;
    }

    void stroke(const float r, const float g, const float b, const float a = 1.0f) {
        stroke_enabled = true;
        stroke_color   = glm::vec4(r, g, b, a);
    }

    void noStroke() {
        stroke_enabled = false;
    }

    void print_matrix() {
        printMatrix(currentMatrix);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &stroke_VAO_xyz_rgba);
        glDeleteBuffers(1, &stroke_VBO_xyz_rgba);
        glDeleteProgram(stroke_shader_program);
        glDeleteVertexArrays(1, &fill_VAO_xyz_rgba_uv);
        glDeleteBuffers(1, &fill_VBO_xyz_rgba_uv);
        glDeleteProgram(fill_shader_program);
    }

    void image(PImage* image, float x, float y, float w = -1, float h = -1);

    void rect(const float x, const float y, const float w, const float h) {
        if (stroke_enabled) {
            add_stroke_vertex_xyz_rgba(x, y, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
            add_stroke_vertex_xyz_rgba(x + w, y, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);

            add_stroke_vertex_xyz_rgba(x + w, y, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
            add_stroke_vertex_xyz_rgba(x + w, y + h, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);

            add_stroke_vertex_xyz_rgba(x + w, y + h, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
            add_stroke_vertex_xyz_rgba(x, y + h, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);

            add_stroke_vertex_xyz_rgba(x, y + h, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
            add_stroke_vertex_xyz_rgba(x, y, 0, stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a);
        }
        if (fill_enabled) {
            add_fill_vertex_xyz_rgba_uv(x, y, 0, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            add_fill_vertex_xyz_rgba_uv(x + w, y, 0, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, fill_color.r, fill_color.g, fill_color.b, fill_color.a);

            add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            add_fill_vertex_xyz_rgba_uv(x, y + h, 0, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            add_fill_vertex_xyz_rgba_uv(x, y, 0, fill_color.r, fill_color.g, fill_color.b, fill_color.a);

            constexpr int       RECT_NUM_VERTICES               = 6;
            const unsigned long fill_vertices_count_xyz_rgba_uv = fill_vertices_xyz_rgba_uv.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
            if (renderBatches.empty() || renderBatches.back().textureID != dummyTexture) {
                renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - RECT_NUM_VERTICES, RECT_NUM_VERTICES, dummyTexture);
            } else {
                renderBatches.back().numVertices += RECT_NUM_VERTICES;
            }
        }
    }

    void flush_stroke() {
        if (stroke_vertices_xyz_rgba.empty()) {
            return;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindBuffer(GL_ARRAY_BUFFER, stroke_VBO_xyz_rgba);
        const unsigned long size = stroke_vertices_xyz_rgba.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), stroke_vertices_xyz_rgba.data());

        glUseProgram(stroke_shader_program);

        // Upload matrices
        const GLint projLoc = glGetUniformLocation(stroke_shader_program, "uProjection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection3D)); // or projection2D

        const GLint viewLoc = glGetUniformLocation(stroke_shader_program, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        const GLint matrixLoc = glGetUniformLocation(stroke_shader_program, "uModelMatrix");
        glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(currentMatrix));

        glBindVertexArray(stroke_VAO_xyz_rgba);
        glDrawArrays(GL_LINES, 0, stroke_vertices_xyz_rgba.size() / NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA);
        glBindVertexArray(0);

        stroke_vertices_xyz_rgba.clear();
    }

    void flush_fill() {
        if (fill_vertices_xyz_rgba_uv.empty()) {
            return;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
        const unsigned long size = fill_vertices_xyz_rgba_uv.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), fill_vertices_xyz_rgba_uv.data());

        glUseProgram(fill_shader_program);

        // Upload matrices
        const GLint projLoc = glGetUniformLocation(fill_shader_program, "uProjection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection3D));

        const GLint viewLoc = glGetUniformLocation(fill_shader_program, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        const GLint modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currentMatrix));

        // Bind textures per batch
        glBindVertexArray(fill_VAO_xyz_rgba_uv);
        for (const auto& batch: renderBatches) {
            glBindTexture(GL_TEXTURE_2D, batch.textureID);
            glDrawArrays(GL_TRIANGLES, batch.startIndex, batch.numVertices);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        fill_vertices_xyz_rgba_uv.clear();
        renderBatches.clear();
    }

private:
    const uint8_t NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV = 9;
    const uint8_t NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA  = 7;

    static const char* vertex_shader_source_texture();
    static const char* fragment_shader_source_texture();
    static const char* vertex_shader_source_simple();
    static const char* fragment_shader_source_simple();

    void fill_resize_buffer(const uint32_t newSize) {
        // Create a new buffer
        GLuint newVBO;
        glGenBuffers(1, &newVBO);
        glBindBuffer(GL_ARRAY_BUFFER, newVBO);
        glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW); // Allocate new size

        // Copy old data to new buffer
        glBindBuffer(GL_COPY_READ_BUFFER, fill_VBO_xyz_rgba_uv);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, fill_max_buffer_size);

        // Delete old buffer and update references
        glDeleteBuffers(1, &fill_VBO_xyz_rgba_uv);
        fill_VBO_xyz_rgba_uv = newVBO;
        fill_max_buffer_size = newSize;

        glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
    }

    void add_fill_vertex_xyz_rgba_uv(const glm::vec3 position,
                                     const glm::vec4 color,
                                     const glm::vec2 tex_coords) {
        add_fill_vertex_xyz_rgba_uv(position.x, position.y, position.z,
                                    color.r, color.g, color.b, color.a,
                                    tex_coords.x, tex_coords.y);
    }

    void add_fill_vertex_xyz_rgba_uv(const float x, const float y, const float z,
                                     const float r, const float g, const float b, const float a = 1.0f,
                                     const float u = 0.0f, const float v = 0.0f) {
        // Each vertex consists of 9 floats (x, y, z, r, g, b, u, v)
        // uint32_t vertexSize = NUM_VERTEX_ATTRIBUTES * sizeof(float);
        if ((fill_vertices_xyz_rgba_uv.size() + NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV) * sizeof(float) > fill_max_buffer_size) {
            fill_resize_buffer(fill_max_buffer_size + VBO_BUFFER_CHUNK_SIZE);
        }

        const glm::vec4 transformed = currentMatrix * glm::vec4(x, y, z, 1.0f); // Apply transformation
        fill_vertices_xyz_rgba_uv.push_back(transformed.x);                     // Position
        fill_vertices_xyz_rgba_uv.push_back(transformed.y);                     // Position
        fill_vertices_xyz_rgba_uv.push_back(transformed.z);                     // Position
        fill_vertices_xyz_rgba_uv.push_back(r);                                 // Color
        fill_vertices_xyz_rgba_uv.push_back(g);                                 // Color
        fill_vertices_xyz_rgba_uv.push_back(b);                                 // Color
        fill_vertices_xyz_rgba_uv.push_back(a);                                 // Color
        fill_vertices_xyz_rgba_uv.push_back(u);                                 // Texture
        fill_vertices_xyz_rgba_uv.push_back(v);                                 // Texture
    }

    void add_stroke_vertex_xyz_rgba(const float x, const float y, const float z,
                                    const float r, const float g, const float b, const float a = 1.0f) {
        // Each vertex consists of 7 floats (x, y, z, r, g, b, u, v)
        // uint32_t vertexSize = NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA * sizeof(float);
        if ((stroke_vertices_xyz_rgba.size() + NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA) * sizeof(float) > stroke_max_buffer_size) {
            std::cerr << "Stroke buffer is full!" << std::endl;
            // TODO create version for stroke buffer
            // stroke_resize_buffer(stroke_max_buffer_size + VBO_BUFFER_CHUNK_SIZE);
        }

        const glm::vec4 transformed = currentMatrix * glm::vec4(x, y, z, 1.0f); // Apply transformation
        stroke_vertices_xyz_rgba.push_back(transformed.x);                      // Position
        stroke_vertices_xyz_rgba.push_back(transformed.y);                      // Position
        stroke_vertices_xyz_rgba.push_back(transformed.z);                      // Position
        stroke_vertices_xyz_rgba.push_back(r);                                  // Color
        stroke_vertices_xyz_rgba.push_back(g);                                  // Color
        stroke_vertices_xyz_rgba.push_back(b);                                  // Color
        stroke_vertices_xyz_rgba.push_back(a);                                  // Color
    }

    void init_stroke_vertice_buffers() {
        glGenVertexArrays(1, &stroke_VAO_xyz_rgba);
        glGenBuffers(1, &stroke_VBO_xyz_rgba);

        glBindVertexArray(stroke_VAO_xyz_rgba);

        glBindBuffer(GL_ARRAY_BUFFER, stroke_VBO_xyz_rgba);
        glBufferData(GL_ARRAY_BUFFER, stroke_max_buffer_size, nullptr, GL_DYNAMIC_DRAW);

        const size_t STRIDE = NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA * sizeof(float);

        // Position Attribute (Location 0) -> 3 floats (x, y, z)
        constexpr int NUM_POSITION_ATTRIBUTES = 3;
        glVertexAttribPointer(0, NUM_POSITION_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // Color Attribute (Location 1) -> 4 floats (r, g, b, a)
        constexpr int NUM_COLOR_ATTRIBUTES = 4;
        glVertexAttribPointer(1, NUM_COLOR_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, (void*) (NUM_POSITION_ATTRIBUTES * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void init_fill_vertice_buffers() {
        glGenVertexArrays(1, &fill_VAO_xyz_rgba_uv);
        glGenBuffers(1, &fill_VBO_xyz_rgba_uv);

        glBindVertexArray(fill_VAO_xyz_rgba_uv);
        glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);

        // Allocate buffer memory (maxBufferSize = 1024 KB) but don't fill it yet
        glBufferData(GL_ARRAY_BUFFER, fill_max_buffer_size, nullptr, GL_DYNAMIC_DRAW);

        const size_t STRIDE = NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV * sizeof(float);

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

    struct RenderBatch {
        int    startIndex;
        int    numVertices;
        GLuint textureID;

        RenderBatch(const int start, const int count, const GLuint texID)
            : startIndex(start), numVertices(count), textureID(texID) {}
    };

    const uint32_t VBO_BUFFER_CHUNK_SIZE = 1024 * 1024;       // 1MB
    const float    DEFAULT_FOV           = 2.0f * atan(0.5f); // = 53.1301f;

    GLuint             fill_shader_program{};
    GLuint             fill_VAO_xyz_rgba_uv = 0;
    GLuint             fill_VBO_xyz_rgba_uv = 0;
    std::vector<float> fill_vertices_xyz_rgba_uv;
    uint32_t           fill_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)

    GLuint             stroke_shader_program{};
    GLuint             stroke_VAO_xyz_rgba = 0;
    GLuint             stroke_VBO_xyz_rgba = 0;
    std::vector<float> stroke_vertices_xyz_rgba;
    uint32_t           stroke_max_buffer_size = VBO_BUFFER_CHUNK_SIZE; // Initial size (1MB)

    GLuint                   dummyTexture{};
    std::vector<RenderBatch> renderBatches;
    glm::mat4                currentMatrix;
    std::vector<glm::mat4>   matrixStack;
    float                    aspectRatio;
    glm::mat4                projection2D{};
    glm::mat4                projection3D{};
    glm::mat4                viewMatrix{};
    glm::vec4                fill_color     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool                     fill_enabled   = true;
    glm::vec4                stroke_color   = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    bool                     stroke_enabled = true;

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

    static void printMatrix(const glm::mat4& matrix) {
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

        const GLuint mShaderProgram = glCreateProgram();
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
