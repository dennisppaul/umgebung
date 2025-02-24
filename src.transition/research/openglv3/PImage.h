#pragma once

#include <string>

/**
 * a PImage ( in SDL terms ) is a surface. a texture is pixel data available to the GPU i.e
 */
class PImage {
public:
    // TODO maybe the textureID reference should be handled by the renderer i.e PGraphics object
    // TODO including a `dirty` flag for deferred uploading.
    int textureID;
    int       width;
    int       height;
    int       channels;
    uint32_t* pixels;

    explicit PImage();
    void bind() const;
    ~PImage();
};