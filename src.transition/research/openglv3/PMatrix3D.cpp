#include <stdexcept>

#include "PMatrix2D.h"
#include "PMatrix3D.h"

PMatrix3D::PMatrix3D() {
    PMatrix3D::reset();
}

PMatrix3D::PMatrix3D(const float m00, const float m01, const float m02,
                     const float m10, const float m11, const float m12) {
    PMatrix3D::set(m00, m01, m02, 0,
                   m10, m11, m12, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);
}
PMatrix3D::PMatrix3D(const float m00, const float m01, const float m02, const float m03,
                     const float m10, const float m11, const float m12, const float m13,
                     const float m20, const float m21, const float m22, const float m23,
                     const float m30, const float m31, const float m32, const float m33) {
    PMatrix3D::set(m00, m01, m02, m03,
                   m10, m11, m12, m13,
                   m20, m21, m22, m23,
                   m30, m31, m32, m33);
}

PMatrix3D::PMatrix3D(const PMatrix3D& other) {
    PMatrix3D::set(other);
}

PMatrix3D::PMatrix3D(const PMatrix2D& other) {
    PMatrix3D::set(other);
}

void PMatrix3D::reset() {
    set(1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

void PMatrix3D::get(PMatrix2D& copy) const {
    copy.set(m00, m01, m02,
             m10, m11, m12);
}

void PMatrix3D::get(PMatrix3D& copy) const {
    copy.set(m00, m01, m02, m03,
             m10, m11, m12, m13,
             m20, m21, m22, m23,
             m30, m31, m32, m33);
}

void PMatrix3D::get(std::vector<float>& target) {
    if (target.size() < 16) {
        target.resize(16);
    }
    target[0]  = m00;
    target[1]  = m01;
    target[2]  = m02;
    target[3]  = m03;
    target[4]  = m10;
    target[5]  = m11;
    target[6]  = m12;
    target[7]  = m13;
    target[8]  = m20;
    target[9]  = m21;
    target[10] = m22;
    target[11] = m23;
    target[12] = m30;
    target[13] = m31;
    target[14] = m32;
    target[15] = m33;
}

void PMatrix3D::set(const PMatrix2D& src) {
    set(src.m00, src.m01, src.m02, 0,
        src.m10, src.m11, src.m12, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

void PMatrix3D::set(const PMatrix3D& src) {
    set(src.m00, src.m01, src.m02, src.m03,
        src.m10, src.m11, src.m12, src.m13,
        src.m20, src.m21, src.m22, src.m23,
        src.m30, src.m31, src.m32, src.m33);
}

void PMatrix3D::set(const std::vector<float>& source) {
    if (source.size() != 16) {
        throw std::runtime_error("Source array must have exactly 16 elements");
    }
    m00 = source[0];
    m01 = source[1];
    m02 = source[2];
    m03 = source[3];
    m10 = source[4];
    m11 = source[5];
    m12 = source[6];
    m13 = source[7];
    m20 = source[8];
    m21 = source[9];
    m22 = source[10];
    m23 = source[11];
    m30 = source[12];
    m31 = source[13];
    m32 = source[14];
    m33 = source[15];
}

void PMatrix3D::set(const float m00, const float m01, const float m02,
                    const float m10, const float m11, const float m12) {
    set(m00, m01, 0, m02,
        m10, m11, 0, m12,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

void PMatrix3D::set(const float m00, const float m01, const float m02, const float m03,
                    const float m10, const float m11, const float m12, const float m13,
                    const float m20, const float m21, const float m22, const float m23,
                    const float m30, const float m31, const float m32, const float m33) {
    this->m00 = m00;
    this->m01 = m01;
    this->m02 = m02;
    this->m03 = m03;
    this->m10 = m10;
    this->m11 = m11;
    this->m12 = m12;
    this->m13 = m13;
    this->m20 = m20;
    this->m21 = m21;
    this->m22 = m22;
    this->m23 = m23;
    this->m30 = m30;
    this->m31 = m31;
    this->m32 = m32;
    this->m33 = m33;
}

void PMatrix3D::translate(const float tx, const float ty) {
    translate(tx, ty, 0);
}

void PMatrix3D::translate(const float tx, const float ty, const float tz) {
    m03 += tx * m00 + ty * m01 + tz * m02;
    m13 += tx * m10 + ty * m11 + tz * m12;
    m23 += tx * m20 + ty * m21 + tz * m22;
    m33 += tx * m30 + ty * m31 + tz * m32;
}

void PMatrix3D::rotate(const float angle) {
    rotateZ(angle);
}

void PMatrix3D::rotateX(const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    apply(1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1);
}

void PMatrix3D::rotateY(const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    apply(c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1);
}

void PMatrix3D::rotateZ(const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    apply(c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void PMatrix3D::rotate(const float angle, float v0, float v1, float v2) {
    const float norm2 = v0 * v0 + v1 * v1 + v2 * v2;
    if (norm2 < 1e-6) { // Check for near-zero vector
        return;
    }

    if (fabs(norm2 - 1) > 1e-6) { // Normalize the vector
        const float norm = sqrt(norm2);
        v0 /= norm;
        v1 /= norm;
        v2 /= norm;
    }

    const float c = cos(angle);
    const float s = sin(angle);
    const float t = 1.0f - c;

    apply((t * v0 * v0) + c, (t * v0 * v1) - (s * v2), (t * v0 * v2) + (s * v1), 0,
          (t * v0 * v1) + (s * v2), (t * v1 * v1) + c, (t * v1 * v2) - (s * v0), 0,
          (t * v0 * v2) - (s * v1), (t * v1 * v2) + (s * v0), (t * v2 * v2) + c, 0,
          0, 0, 0, 1);
}

void PMatrix3D::scale(const float s) {
    scale(s, s, s);
}

void PMatrix3D::scale(const float sx, const float sy) {
    scale(sx, sy, 1);
}

void PMatrix3D::scale(const float x, const float y, const float z) {
    m00 *= x;
    m01 *= y;
    m02 *= z;
    m10 *= x;
    m11 *= y;
    m12 *= z;
    m20 *= x;
    m21 *= y;
    m22 *= z;
    m30 *= x;
    m31 *= y;
    m32 *= z;
}

void PMatrix3D::shearX(const float angle) {
    const float t = tan(angle);
    apply(1, t, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void PMatrix3D::shearY(const float angle) {
    const float t = tan(angle);
    apply(1, 0, 0, 0, t, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void PMatrix3D::apply(const PMatrix2D& source) {
    apply(source.m00, source.m01, 0, source.m02,
          source.m10, source.m11, 0, source.m12,
          0, 0, 1, 0,
          0, 0, 0, 1);
}

void PMatrix3D::apply(const PMatrix3D& source) {
    apply(source.m00, source.m01, source.m02, source.m03,
          source.m10, source.m11, source.m12, source.m13,
          source.m20, source.m21, source.m22, source.m23,
          source.m30, source.m31, source.m32, source.m33);
}

void PMatrix3D::apply(const float n00, const float n01, const float n02, const float n03,
                      const float n10, const float n11, const float n12, const float n13,
                      const float n20, const float n21, const float n22, const float n23,
                      const float n30, const float n31, const float n32, const float n33) {
    const float r00 = m00 * n00 + m01 * n10 + m02 * n20 + m03 * n30;
    const float r01 = m00 * n01 + m01 * n11 + m02 * n21 + m03 * n31;
    const float r02 = m00 * n02 + m01 * n12 + m02 * n22 + m03 * n32;
    const float r03 = m00 * n03 + m01 * n13 + m02 * n23 + m03 * n33;

    const float r10 = m10 * n00 + m11 * n10 + m12 * n20 + m13 * n30;
    const float r11 = m10 * n01 + m11 * n11 + m12 * n21 + m13 * n31;
    const float r12 = m10 * n02 + m11 * n12 + m12 * n22 + m13 * n32;
    const float r13 = m10 * n03 + m11 * n13 + m12 * n23 + m13 * n33;

    const float r20 = m20 * n00 + m21 * n10 + m22 * n20 + m23 * n30;
    const float r21 = m20 * n01 + m21 * n11 + m22 * n21 + m23 * n31;
    const float r22 = m20 * n02 + m21 * n12 + m22 * n22 + m23 * n32;
    const float r23 = m20 * n03 + m21 * n13 + m22 * n23 + m23 * n33;

    const float r30 = m30 * n00 + m31 * n10 + m32 * n20 + m33 * n30;
    const float r31 = m30 * n01 + m31 * n11 + m32 * n21 + m33 * n31;
    const float r32 = m30 * n02 + m31 * n12 + m32 * n22 + m33 * n32;
    const float r33 = m30 * n03 + m31 * n13 + m32 * n23 + m33 * n33;

    m00 = r00;
    m01 = r01;
    m02 = r02;
    m03 = r03;
    m10 = r10;
    m11 = r11;
    m12 = r12;
    m13 = r13;
    m20 = r20;
    m21 = r21;
    m22 = r22;
    m23 = r23;
    m30 = r30;
    m31 = r31;
    m32 = r32;
    m33 = r33;
}

void PMatrix3D::preApply(const PMatrix2D& source) {
    throw std::runtime_error("PMatrix3D does not support 2D matrix operations");
}

void PMatrix3D::preApply(const PMatrix3D& left) {
    preApply(left.m00, left.m01, left.m02, left.m03,
             left.m10, left.m11, left.m12, left.m13,
             left.m20, left.m21, left.m22, left.m23,
             left.m30, left.m31, left.m32, left.m33);
}

float PMatrix3D::determinant() {
    float f =
        m00 * ((m11 * m22 * m33 + m12 * m23 * m31 + m13 * m21 * m32) - m13 * m22 * m31 - m11 * m23 * m32 - m12 * m21 * m33);
    f -= m01 * ((m10 * m22 * m33 + m12 * m23 * m30 + m13 * m20 * m32) - m13 * m22 * m30 - m10 * m23 * m32 - m12 * m20 * m33);
    f += m02 * ((m10 * m21 * m33 + m11 * m23 * m30 + m13 * m20 * m31) - m13 * m21 * m30 - m10 * m23 * m31 - m11 * m20 * m33);
    f -= m03 * ((m10 * m21 * m32 + m11 * m22 * m30 + m12 * m20 * m31) - m12 * m21 * m30 - m10 * m22 * m31 - m11 * m20 * m32);
    return f;
}

void PMatrix3D::preApply(const float n00, const float n01, const float n02, const float n03,
                         const float n10, const float n11, const float n12, const float n13,
                         const float n20, const float n21, const float n22, const float n23,
                         const float n30, const float n31, const float n32, const float n33) {
    const float r00 = n00 * m00 + n01 * m10 + n02 * m20 + n03 * m30;
    const float r01 = n00 * m01 + n01 * m11 + n02 * m21 + n03 * m31;
    const float r02 = n00 * m02 + n01 * m12 + n02 * m22 + n03 * m32;
    const float r03 = n00 * m03 + n01 * m13 + n02 * m23 + n03 * m33;

    const float r10 = n10 * m00 + n11 * m10 + n12 * m20 + n13 * m30;
    const float r11 = n10 * m01 + n11 * m11 + n12 * m21 + n13 * m31;
    const float r12 = n10 * m02 + n11 * m12 + n12 * m22 + n13 * m32;
    const float r13 = n10 * m03 + n11 * m13 + n12 * m23 + n13 * m33;

    const float r20 = n20 * m00 + n21 * m10 + n22 * m20 + n23 * m30;
    const float r21 = n20 * m01 + n21 * m11 + n22 * m21 + n23 * m31;
    const float r22 = n20 * m02 + n21 * m12 + n22 * m22 + n23 * m32;
    const float r23 = n20 * m03 + n21 * m13 + n22 * m23 + n23 * m33;

    const float r30 = n30 * m00 + n31 * m10 + n32 * m20 + n33 * m30;
    const float r31 = n30 * m01 + n31 * m11 + n32 * m21 + n33 * m31;
    const float r32 = n30 * m02 + n31 * m12 + n32 * m22 + n33 * m32;
    const float r33 = n30 * m03 + n31 * m13 + n32 * m23 + n33 * m33;

    m00 = r00;
    m01 = r01;
    m02 = r02;
    m03 = r03;
    m10 = r10;
    m11 = r11;
    m12 = r12;
    m13 = r13;
    m20 = r20;
    m21 = r21;
    m22 = r22;
    m23 = r23;
    m30 = r30;
    m31 = r31;
    m32 = r32;
    m33 = r33;
}

void PMatrix3D::mult(const PVector& source, PVector& target) {
    target.x = m00 * source.x + m01 * source.y + m02 * source.z + m03;
    target.y = m10 * source.x + m11 * source.y + m12 * source.z + m13;
    target.z = m20 * source.x + m21 * source.y + m22 * source.z + m23;
}

std::vector<float> PMatrix3D::mult(const std::vector<float>& source, std::vector<float> target) {
    if (target.empty() || target.size() < 3) {
        target = std::vector<float>(3);
    }
    target[0] = m00 * source[0] + m01 * source[1] + m02 * source[2] + m03;
    target[1] = m10 * source[0] + m11 * source[1] + m12 * source[2] + m13;
    target[2] = m20 * source[0] + m21 * source[1] + m22 * source[2] + m23;
    return target;
}

void PMatrix3D::transpose() {
    std::swap(m01, m10);
    std::swap(m02, m20);
    std::swap(m03, m30);
    std::swap(m12, m21);
    std::swap(m13, m31);
    std::swap(m23, m32);
}

/**
 * Calculate the determinant of a 3x3 matrix.
 * @return result
 */
float PMatrix3D::determinant3x3(const float t00, const float t01, const float t02,
                                const float t10, const float t11, const float t12,
                                const float t20, const float t21, const float t22) {
    return (t00 * (t11 * t22 - t12 * t21) +
            t01 * (t12 * t20 - t10 * t22) +
            t02 * (t10 * t21 - t11 * t20));
}

bool PMatrix3D::invert() {
    const float _determinant = determinant();
    if (fabs(_determinant) <= 1e-6) { // Check for near-zero determinant
        return false;
    }

    const float t00 = determinant3x3(m11, m12, m13, m21, m22, m23, m31, m32, m33);
    const float t01 = -determinant3x3(m10, m12, m13, m20, m22, m23, m30, m32, m33);
    const float t02 = determinant3x3(m10, m11, m13, m20, m21, m23, m30, m31, m33);
    const float t03 = -determinant3x3(m10, m11, m12, m20, m21, m22, m30, m31, m32);

    const float t10 = -determinant3x3(m01, m02, m03, m21, m22, m23, m31, m32, m33);
    const float t11 = determinant3x3(m00, m02, m03, m20, m22, m23, m30, m32, m33);
    const float t12 = -determinant3x3(m00, m01, m03, m20, m21, m23, m30, m31, m33);
    const float t13 = determinant3x3(m00, m01, m02, m20, m21, m22, m30, m31, m32);

    const float t20 = determinant3x3(m01, m02, m03, m11, m12, m13, m31, m32, m33);
    const float t21 = -determinant3x3(m00, m02, m03, m10, m12, m13, m30, m32, m33);
    const float t22 = determinant3x3(m00, m01, m03, m10, m11, m13, m30, m31, m33);
    const float t23 = -determinant3x3(m00, m01, m02, m10, m11, m12, m30, m31, m32);

    const float t30 = -determinant3x3(m01, m02, m03, m11, m12, m13, m21, m22, m23);
    const float t31 = determinant3x3(m00, m02, m03, m10, m12, m13, m20, m22, m23);
    const float t32 = -determinant3x3(m00, m01, m03, m10, m11, m13, m20, m21, m23);
    const float t33 = determinant3x3(m00, m01, m02, m10, m11, m12, m20, m21, m22);

    // Transpose and divide by the determinant
    m00 = t00 / _determinant;
    m01 = t10 / _determinant;
    m02 = t20 / _determinant;
    m03 = t30 / _determinant;
    m10 = t01 / _determinant;
    m11 = t11 / _determinant;
    m12 = t21 / _determinant;
    m13 = t31 / _determinant;
    m20 = t02 / _determinant;
    m21 = t12 / _determinant;
    m22 = t22 / _determinant;
    m23 = t32 / _determinant;
    m30 = t03 / _determinant;
    m31 = t13 / _determinant;
    m32 = t23 / _determinant;
    m33 = t33 / _determinant;

    return true;
}

void PMatrix3D::print() const {
    printf("[%.2f, %.2f, %.2f, %.2f]\n", m00, m01, m02, m03);
    printf("[%.2f, %.2f, %.2f, %.2f]\n", m10, m11, m12, m13);
    printf("[%.2f, %.2f, %.2f, %.2f]\n", m20, m21, m22, m23);
    printf("[%.2f, %.2f, %.2f, %.2f]\n", m30, m31, m32, m33);
}

float PMatrix3D::max(const float a, const float b) {
    return (a > b) ? a : b;
}

float PMatrix3D::abs(const float a) {
    return (a < 0) ? -a : a;
}

float PMatrix3D::sin(const float angle) {
    return std::sin(angle);
}

float PMatrix3D::cos(const float angle) {
    return std::cos(angle);
}

float PMatrix3D::tan(const float angle) {
    return std::tan(angle);
}

void PMatrix3D::apply(const float n00, const float n01, const float n02, const float n10, const float n11, const float n12) {
    apply(n00, n01, 0, n02,
          n10, n11, 0, n12,
          0, 0, 1, 0,
          0, 0, 0, 1);
}

void PMatrix3D::preApply(const float n00, const float n01, const float n02, const float n10, const float n11, const float n12) {
    preApply(n00, n01, 0, n02,
             n10, n11, 0, n12,
             0, 0, 1, 0,
             0, 0, 0, 1);
}