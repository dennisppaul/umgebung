#include <iostream>
#include <GL/glew.h>

#include "PImage.h"
#include "PGraphicsOpenGLv33.h"

PImage::PImage(const std::string& filename) {
    GLuint mTextureID;
    PGraphicsOpenGLv33::load_texture(filename.c_str(), mTextureID, width, height, channels);
    textureID = mTextureID;
}

void PImage::bind() const {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

PImage::~PImage() {
    if (textureID) {
        const GLuint mTextureID = textureID;
        glDeleteTextures(1, &mTextureID);
    }
}
