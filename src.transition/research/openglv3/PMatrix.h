#ifndef PMATRIX_H
#define PMATRIX_H

#include <vector>

#include "PVector.h"

class PMatrix2D;
class PMatrix3D;

class PMatrix {
public:
    virtual ~PMatrix() {}

    virtual void               reset()                                         = 0;
    virtual void               get(PMatrix2D& copy) const                      = 0;
    virtual void               get(PMatrix3D& copy) const                      = 0;
    virtual void               get(std::vector<float>& target)                  = 0;
    virtual void               set(const PMatrix2D& src)                       = 0;
    virtual void               set(const PMatrix3D& src)                       = 0;
    virtual void               set(const std::vector<float>& source)           = 0;
    virtual void               set(float m00, float m01, float m02,
                                   float m10, float m11, float m12)            = 0;
    virtual void               set(float m00, float m01, float m02, float m03,
                                   float m10, float m11, float m12, float m13,
                                   float m20, float m21, float m22, float m23,
                                   float m30, float m31, float m32, float m33) = 0;

    virtual void translate(float tx, float ty)                     = 0;
    virtual void translate(float tx, float ty, float tz)           = 0;
    virtual void rotate(float angle)                               = 0;
    virtual void rotateX(float angle)                              = 0;
    virtual void rotateY(float angle)                              = 0;
    virtual void rotateZ(float angle)                              = 0;
    virtual void rotate(float angle, float v0, float v1, float v2) = 0;
    virtual void scale(float s)                                    = 0;
    virtual void scale(float sx, float sy)                         = 0;
    virtual void scale(float x, float y, float z)                  = 0;
    virtual void shearX(float angle)                               = 0;
    virtual void shearY(float angle)                               = 0;

    virtual void apply(const PMatrix2D&)                                                 = 0;
    virtual void apply(const PMatrix3D&)                                                 = 0;
    virtual void apply(float n00, float n01, float n02, float n10, float n11, float n12) = 0;
    virtual void apply(float n00, float n01, float n02, float n03,
                       float n10, float n11, float n12, float n13,
                       float n20, float n21, float n22, float n23,
                       float n30, float n31, float n32, float n33)                       = 0;

    virtual void preApply(const PMatrix2D& left)                                            = 0;
    virtual void preApply(const PMatrix3D& left)                                            = 0;
    virtual void preApply(float n00, float n01, float n02, float n10, float n11, float n12) = 0;
    virtual void preApply(float n00, float n01, float n02, float n03,
                          float n10, float n11, float n12, float n13,
                          float n20, float n21, float n22, float n23,
                          float n30, float n31, float n32, float n33)                       = 0;

    virtual void               mult(const PVector& source, PVector& target)                      = 0;
    virtual std::vector<float> mult(const std::vector<float>& source, std::vector<float> target) = 0;
    virtual void               transpose()                                                       = 0;
    virtual bool               invert()                                                          = 0;
    virtual float              determinant()                                                     = 0;
};

#endif // PMATRIX_H