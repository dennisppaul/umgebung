#pragma once

#include <string>

class PImage {
public:
    int textureID;
    int width;
    int height;
    int channels;

    explicit PImage(const std::string& filename);

    void bind() const;

    ~PImage();
};