#include <iostream>
#include "PGraphicsOpenGLv33.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool PGraphicsOpenGLv33::generate_texture_mipmapped = true;

void PGraphicsOpenGLv33::load_texture(const char* file_path,
                            GLuint&     textureID,
                            int&        width,
                            int&        height,
                            int&        channels) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    if (generate_texture_mipmapped) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

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

void PGraphicsOpenGLv33::image(const PImage* image, const float x, const float y, float w, float h) {
    const PImage& img = *image;
    if (w == -1) {
        w = static_cast<float>(img.width);
    }
    if (h == -1) {
        h = static_cast<float>(img.height);
    }
    const auto color = fill_color;
    add_fill_vertex_xyz_rgba_uv(x, y, 0, color.r, color.g, color.b, color.a, 0.0f, 0.0f);
    add_fill_vertex_xyz_rgba_uv(x + w, y, 0, color.r, color.g, color.b, color.a, 1.0f, 0.0f);
    add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, color.r, color.g, color.b, color.a, 1.0f, 1.0f);

    add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, color.r, color.g, color.b, color.a, 1.0f, 1.0f);
    add_fill_vertex_xyz_rgba_uv(x, y + h, 0, color.r, color.g, color.b, color.a, 0.0f, 1.0f);
    add_fill_vertex_xyz_rgba_uv(x, y, 0, color.r, color.g, color.b, color.a, 0.0f, 0.0f);

    constexpr int       RECT_NUM_VERTICES               = 6;
    const unsigned long fill_vertices_count_xyz_rgba_uv = fill_vertices_xyz_rgba_uv.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
    if (renderBatches.empty() || renderBatches.back().textureID != img.textureID) {
        renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - RECT_NUM_VERTICES, RECT_NUM_VERTICES, img.textureID);
    } else {
        renderBatches.back().numVertices += RECT_NUM_VERTICES;
    }
}

const char* PGraphicsOpenGLv33::vertex_shader_source_texture() {
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

const char* PGraphicsOpenGLv33::fragment_shader_source_texture() {
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

const char* PGraphicsOpenGLv33::vertex_shader_source_simple() {
    // Vertex Shader source ( without texture )
    const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

out vec4 vColor;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";
    return vertexShaderSource;
}

const char* PGraphicsOpenGLv33::fragment_shader_source_simple() {
    // Fragment Shader source ( without texture )
    const char* fragmentShaderSource = R"(
#version 330 core

in vec4 vColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor);
}
)";
    return fragmentShaderSource;
}
