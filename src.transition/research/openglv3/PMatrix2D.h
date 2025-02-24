#ifndef PMATRIX2D_H
#define PMATRIX2D_H

#include <vector>

#include "PMatrix.h"
#include "PVector.h"

class PMatrix2D final : public PMatrix {
public:
    float m00, m01, m02;
    float m10, m11, m12;

    PMatrix2D();
    PMatrix2D(float m00, float m01, float m02, float m10, float m11, float m12);
    explicit PMatrix2D(const PMatrix2D& other);
    explicit PMatrix2D(const PMatrix3D& other);

    void reset() override;
    void get(PMatrix2D& copy) const override;
    void get(PMatrix3D& copy) const override;
    void get(std::vector<float>& target) override;
    void set(const PMatrix2D& src) override;
    void set(const PMatrix3D& src) override;
    void set(const std::vector<float>& source) override;
    void set(float m00, float m01, float m02,
             float m10, float m11, float m12) override;
    void set(float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33) override;

    void translate(float tx, float ty) override;
    void translate(float tx, float ty, float tz) override;

    void rotate(float angle) override;
    void rotateX(float angle) override;
    void rotateY(float angle) override;
    void rotateZ(float angle) override;
    void rotate(float angle, float v0, float v1, float v2) override;

    void scale(float s) override;
    void scale(float sx, float sy) override;
    void scale(float x, float y, float z) override;

    void shearX(float angle) override;
    void shearY(float angle) override;

    void apply(const PMatrix2D& source) override;
    void apply(const PMatrix3D& source) override;
    void apply(float n00, float n01, float n02, float n10, float n11, float n12) override;
    void apply(float n00, float n01, float n02, float n03,
               float n10, float n11, float n12, float n13,
               float n20, float n21, float n22, float n23,
               float n30, float n31, float n32, float n33) override;

    void preApply(const PMatrix2D& left) override;
    void preApply(const PMatrix3D& left) override;
    void preApply(float n00, float n01, float n02, float n10, float n11, float n12) override;
    void preApply(float n00, float n01, float n02, float n03,
                  float n10, float n11, float n12, float n13,
                  float n20, float n21, float n22, float n23,
                  float n30, float n31, float n32, float n33) override;

    void               mult(const PVector& source, PVector& target) override;
    std::vector<float> mult(const std::vector<float>& source, std::vector<float> target) override;

    void  transpose() override;
    bool  invert() override;
    float determinant() override;

    void print() const;

private:
    static float max(float a, float b);
    static float abs(float a);
    static float sin(float angle);
    static float cos(float angle);
    static float tan(float angle);
};

#endif // PMATRIX2D_H