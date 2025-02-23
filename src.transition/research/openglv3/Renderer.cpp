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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Renderer::image(const PImage& img, const float x, const float y, float w, float h) {
    if (w == -1) {
        w = static_cast<int>(img.width);
    }
    if (h == -1) {
        h = static_cast<int>(img.height);
    }
    constexpr int RECT_NUM_VERTICES = 6;
    addVertex(x, y, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
    addVertex(x + w, y, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f);
    addVertex(x + w, y + h, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f);

    addVertex(x + w, y + h, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f);
    addVertex(x, y + h, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 1.0f);
    addVertex(x, y, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);

    numVertices += RECT_NUM_VERTICES;

    if (renderBatches.empty() || renderBatches.back().textureID != img.textureID) {
        renderBatches.emplace_back(numVertices - RECT_NUM_VERTICES, RECT_NUM_VERTICES, img.textureID);
    } else {
        renderBatches.back().numVertices += RECT_NUM_VERTICES;
    }
}
