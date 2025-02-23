#include <iostream>
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Renderer::loadTexture(const char* file_path,
                           GLuint&     textureID,
                           int&        width,
                           int&        height,
                           int&        channels) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    unsigned char* data = stbi_load(file_path, &width, &height, &channels, 4);
    if (data) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load image: " << file_path << std::endl;
    }
    stbi_image_free(data);
}

void Renderer::image(const PImage& img, const float x, const float y, float w, float h) {
    // PImage img = *image;
    if (w == -1) {
        w = img.width;
    }
    if (h == -1) {
        h = img.height;
    }
    constexpr auto color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    add_vertex(x, y, 0, color.r, color.g, color.b, color.a, 0.0f, 0.0f);
    add_vertex(x + w, y, 0, color.r, color.g, color.b, color.a, 1.0f, 0.0f);
    add_vertex(x + w, y + h, 0, color.r, color.g, color.b, color.a, 1.0f, 1.0f);

    add_vertex(x + w, y + h, 0, color.r, color.g, color.b, color.a, 1.0f, 1.0f);
    add_vertex(x, y + h, 0, color.r, color.g, color.b, color.a, 0.0f, 1.0f);
    add_vertex(x, y, 0, color.r, color.g, color.b, color.a, 0.0f, 0.0f);

    constexpr int RECT_NUM_VERTICES = 6;
    if (renderBatches.empty() || renderBatches.back().textureID != img.textureID) {
        renderBatches.emplace_back(numVertices - RECT_NUM_VERTICES, RECT_NUM_VERTICES, img.textureID);
    } else {
        renderBatches.back().numVertices += RECT_NUM_VERTICES;
    }
}


const char* Renderer::vertexShaderSource() {
    const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vColor;
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
    return vertexShaderSource;
}

const char* Renderer::fragmentShaderSource() {
    const char* fragmentShaderSource = R"(
#version 330 core

in vec4 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    FragColor = texture(uTexture, vTexCoord) * vColor;
}
)";
    return fragmentShaderSource;
}