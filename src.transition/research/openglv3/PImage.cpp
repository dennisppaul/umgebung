#include <iostream>

#include "PImage.h"
#include "PGraphicsOpenGLv33.h"

PImage::PImage() : textureID(-1),
                   width(0),
                   height(0),
                   channels(0),
                   pixels(nullptr) {
}

void PImage::bind() const {
    PGraphicsOpenGLv33::bind_texture(textureID);
}

PImage::~PImage() {
    PGraphicsOpenGLv33::delete_texture(textureID);
}
