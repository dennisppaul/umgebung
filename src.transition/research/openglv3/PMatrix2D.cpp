#include <stdexcept>

#include "PMatrix2D.h"
#include "PMatrix3D.h"

PMatrix2D::PMatrix2D() {
    PMatrix2D::reset();
}

PMatrix2D::PMatrix2D(const float m00, const float m01, const float m02,
                     const float m10, const float m11, const float m12) {
    PMatrix2D::set(m00, m01, m02, m10, m11, m12);
}

PMatrix2D::PMatrix2D(const PMatrix2D& other) {
    PMatrix2D::set(other);
}

PMatrix2D::PMatrix2D(const PMatrix3D& other) {
    PMatrix2D::set(other);
}

void PMatrix2D::reset() {
    set(1, 0, 0, 0, 1, 0);
}

void PMatrix2D::get(PMatrix2D& copy) const {
    copy.set(*this);
}

void PMatrix2D::get(PMatrix3D& copy) const {
    throw std::runtime_error("PMatrix2D does not support 3D matrix operations");
}

void PMatrix2D::get(std::vector<float>& target) {
    if (target.size() < 6) {
        target.resize(6);
    }
    target[0] = m00;
    target[1] = m01;
    target[2] = m02;
    target[3] = m10;
    target[4] = m11;
    target[5] = m12;
}

void PMatrix2D::set(const PMatrix2D& src) {
    set(src.m00, src.m01, src.m02, src.m10, src.m11, src.m12);
}

void PMatrix2D::set(const PMatrix3D& src) {
    set(src.m00, src.m01, src.m02,
        src.m10, src.m11, src.m12);
}
void PMatrix2D::set(const std::vector<float>& source) {
    if (source.size() != 6) {
        throw std::runtime_error("Source array must have exactly 6 elements");
    }
    m00 = source[0];
    m01 = source[1];
    m02 = source[2];
    m10 = source[3];
    m11 = source[4];
    m12 = source[5];
}

void PMatrix2D::set(const float m00, const float m01, const float m02,
                    const float m10, const float m11, const float m12) {
    this->m00 = m00;
    this->m01 = m01;
    this->m02 = m02;
    this->m10 = m10;
    this->m11 = m11;
    this->m12 = m12;
}

void PMatrix2D::set(float m00, float m01, float m02, float m03,
                    float m10, float m11, float m12, float m13,
                    float m20, float m21, float m22, float m23,
                    float m30, float m31, float m32, float m33) {
    throw std::runtime_error("PMatrix2D does not support 4x4 matrix operations");
}

void PMatrix2D::translate(const float tx, const float ty) {
    m02 = tx * m00 + ty * m01 + m02;
    m12 = tx * m10 + ty * m11 + m12;
}

void PMatrix2D::translate(float tx, float ty, float tz) {
    throw std::runtime_error("PMatrix2D does not support 3D translation");
}

void PMatrix2D::rotate(const float angle) {
    const float s = sin(angle);
    const float c = cos(angle);

    float temp1 = m00;
    float temp2 = m01;
    m00         = c * temp1 + s * temp2;
    m01         = -s * temp1 + c * temp2;

    temp1 = m10;
    temp2 = m11;
    m10   = c * temp1 + s * temp2;
    m11   = -s * temp1 + c * temp2;
}

void PMatrix2D::rotateX(float angle) {
    throw std::runtime_error("PMatrix2D does not support 3D rotation");
}

void PMatrix2D::rotateY(float angle) {
    throw std::runtime_error("PMatrix2D does not support 3D rotation");
}

void PMatrix2D::rotateZ(const float angle) {
    rotate(angle);
}

void PMatrix2D::rotate(float angle, float v0, float v1, float v2) {
    throw std::runtime_error("PMatrix2D does not support 3D rotation");
}

void PMatrix2D::scale(const float s) {
    scale(s, s);
}

void PMatrix2D::scale(const float sx, const float sy) {
    m00 *= sx;
    m01 *= sy;
    m10 *= sx;
    m11 *= sy;
}

void PMatrix2D::scale(float x, float y, float z) {
    throw std::runtime_error("PMatrix2D does not support 3D scaling");
}

void PMatrix2D::shearX(const float angle) {
    const float t = tan(angle);
    apply(1, t, 0, 0, 1, 0);
}

void PMatrix2D::shearY(const float angle) {
    const float t = tan(angle);
    apply(1, 0, 0, t, 1, 0);
}

void PMatrix2D::apply(const PMatrix2D& source) {
    apply(source.m00, source.m01, source.m02, source.m10, source.m11, source.m12);
}

void PMatrix2D::apply(const PMatrix3D& source) {
    throw std::runtime_error("PMatrix2D does not support 3D matrix operations");
}

void PMatrix2D::apply(const float n00, const float n01, const float n02, const float n10, const float n11, const float n12) {
    float t0 = m00;
    float t1 = m01;
    m00      = n00 * t0 + n10 * t1;
    m01      = n01 * t0 + n11 * t1;
    m02 += n02 * t0 + n12 * t1;

    t0  = m10;
    t1  = m11;
    m10 = n00 * t0 + n10 * t1;
    m11 = n01 * t0 + n11 * t1;
    m12 += n02 * t0 + n12 * t1;
}

void PMatrix2D::apply(float n00, float n01, float n02, float n03,
                      float n10, float n11, float n12, float n13,
                      float n20, float n21, float n22, float n23,
                      float n30, float n31, float n32, float n33) {
    throw std::runtime_error("PMatrix2D does not support 4x4 matrix operations");
}

void PMatrix2D::preApply(const PMatrix2D& left) {
    preApply(left.m00, left.m01, left.m02,
             left.m10, left.m11, left.m12);
}

void PMatrix2D::preApply(const PMatrix3D& left) {
    throw std::runtime_error("PMatrix2D does not support 3D matrix operations");
}

void PMatrix2D::preApply(const float n00, const float n01, float n02, const float n10, const float n11, float n12) {
    float t0 = m02;
    float t1 = m12;
    n02 += t0 * n00 + t1 * n01;
    n12 += t0 * n10 + t1 * n11;

    m02 = n02;
    m12 = n12;

    t0  = m00;
    t1  = m10;
    m00 = t0 * n00 + t1 * n01;
    m10 = t0 * n10 + t1 * n11;

    t0  = m01;
    t1  = m11;
    m01 = t0 * n00 + t1 * n01;
    m11 = t0 * n10 + t1 * n11;
}

void PMatrix2D::preApply(float n00, float n01, float n02, float n03,
                         float n10, float n11, float n12, float n13,
                         float n20, float n21, float n22, float n23,
                         float n30, float n31, float n32, float n33) {
    throw std::runtime_error("PMatrix2D does not support 4x4 matrix operations");
}

void PMatrix2D::mult(const PVector& source, PVector& target) {
    target.x = m00 * source.x + m01 * source.y + m02;
    target.y = m10 * source.x + m11 * source.y + m12;
}

std::vector<float> PMatrix2D::mult(const std::vector<float>& source, std::vector<float> target) {
    if (target.empty() || target.size() < 2) {
        target = std::vector<float>(2);
    }
    target[0] = m00 * source[0] + m01 * source[1] + m02;
    target[1] = m10 * source[0] + m11 * source[1] + m12;
    return target;
}

void PMatrix2D::transpose() {
    // Transposing a 2D affine matrix is not meaningful in most cases.
}

bool PMatrix2D::invert() {
    const float _determinant = determinant();
    if (fabs(_determinant) <= 1e-6) { // Check for near-zero determinant
        return false;
    }

    const float t00 = m11 / _determinant;
    const float t10 = -m10 / _determinant;
    const float t01 = -m01 / _determinant;
    const float t11 = m00 / _determinant;
    const float t02 = (m01 * m12 - m11 * m02) / _determinant;
    const float t12 = (m10 * m02 - m00 * m12) / _determinant;

    m00 = t00;
    m10 = t10;
    m01 = t01;
    m11 = t11;
    m02 = t02;
    m12 = t12;

    return true;
}

float PMatrix2D::determinant() {
    return m00 * m11 - m01 * m10;
}

void PMatrix2D::print() const {
    printf("[%.2f, %.2f, %.2f]\n", m00, m01, m02);
    printf("[%.2f, %.2f, %.2f]\n", m10, m11, m12);
}

float PMatrix2D::max(const float a, const float b) {
    return (a > b) ? a : b;
}

float PMatrix2D::abs(const float a) {
    return (a < 0) ? -a : a;
}

float PMatrix2D::sin(const float angle) {
    return std::sin(angle);
}

float PMatrix2D::cos(const float angle) {
    return std::cos(angle);
}

float PMatrix2D::tan(const float angle) {
    return std::tan(angle);
}
