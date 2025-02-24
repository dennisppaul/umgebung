#include <GL/glew.h>
#include "PGraphicsOpenGLv33.h"

using namespace umgebung;

PGraphicsOpenGLv33::PGraphicsOpenGLv33() : PImage(0, 0, 0) {}

void PGraphicsOpenGLv33::strokeWeight(float weight) {}

void PGraphicsOpenGLv33::background(float a, float b, float c, float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PGraphicsOpenGLv33::background(float a) {
    background(a, a, a);
}

void    PGraphicsOpenGLv33::rect(float x, float y, float width, float height) const {}
void    PGraphicsOpenGLv33::ellipse(float x, float y, float width, float height) const {}
void    PGraphicsOpenGLv33::circle(float x, float y, float radius) const {}
void    PGraphicsOpenGLv33::ellipseDetail(int detail) {}
void    PGraphicsOpenGLv33::line(float x1, float y1, float x2, float y2) const {}
void    PGraphicsOpenGLv33::bezier(float x1, float y1,
                                   float x2, float y2,
                                   float x3, float y3,
                                   float x4, float y4) const {}
void    PGraphicsOpenGLv33::bezier(float x1, float y1, float z1,
                                   float x2, float y2, float z2,
                                   float x3, float y3, float z3,
                                   float x4, float y4, float z4) const {}
void    PGraphicsOpenGLv33::bezierDetail(int detail) {}
void    PGraphicsOpenGLv33::pointSize(float point_size) {}
void    PGraphicsOpenGLv33::point(float x, float y, float z) const {}
void    PGraphicsOpenGLv33::beginShape(int shape) {}
void    PGraphicsOpenGLv33::endShape() {}
void    PGraphicsOpenGLv33::vertex(float x, float y, float z) {}
void    PGraphicsOpenGLv33::vertex(float x, float y, float z, float u, float v) {}
PFont*  PGraphicsOpenGLv33::loadFont(const std::string& file, float size) { return nullptr; }
void    PGraphicsOpenGLv33::textFont(PFont* font) {}
void    PGraphicsOpenGLv33::textSize(float size) const {}
void    PGraphicsOpenGLv33::text(const char* value, float x, float y, float z) const {}
float   PGraphicsOpenGLv33::textWidth(const std::string& text) const { return 0.0f; }
PImage* PGraphicsOpenGLv33::loadImage(const std::string& filename) { return nullptr; }
void    PGraphicsOpenGLv33::image(const PImage* img, float x, float y, float w, float h) const {}
void    PGraphicsOpenGLv33::image(PImage* img, float x, float y) {}
void    PGraphicsOpenGLv33::texture(const PImage* img) {}
void    PGraphicsOpenGLv33::popMatrix() {}
void    PGraphicsOpenGLv33::pushMatrix() {}
void    PGraphicsOpenGLv33::translate(float x, float y, float z) {}
void    PGraphicsOpenGLv33::rotateX(float angle) {}
void    PGraphicsOpenGLv33::rotateY(float angle) {}
void    PGraphicsOpenGLv33::rotateZ(float angle) {}
void    PGraphicsOpenGLv33::rotate(float angle) {}
void    PGraphicsOpenGLv33::rotate(float angle, float x, float y, float z) {}
void    PGraphicsOpenGLv33::scale(float x) {}
void    PGraphicsOpenGLv33::scale(float x, float y) {}
void    PGraphicsOpenGLv33::scale(float x, float y, float z) {}
void    PGraphicsOpenGLv33::pixelDensity(int density) {}
void    PGraphicsOpenGLv33::hint(uint16_t property) {}
void    PGraphicsOpenGLv33::text_str(const std::string& text, float x, float y, float z) const {}
void    PGraphicsOpenGLv33::beginDraw() {}
void    PGraphicsOpenGLv33::endDraw() const {}
void    PGraphicsOpenGLv33::bind() const {}
void    PGraphicsOpenGLv33::init(uint32_t* pixels, int width, int height, int format) {}