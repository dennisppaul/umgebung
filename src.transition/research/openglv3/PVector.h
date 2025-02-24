#ifndef PVECTOR_H
#define PVECTOR_H

#include <vector>

class PVector {
public:
    float x, y, z;

    PVector() : x(0), y(0), z(0) {}
    PVector(const float x, const float y, const float z = 0) : x(x), y(y), z(z) {}

    PVector set(float x, float y, float z = 0);
    PVector set(const PVector& v);
    PVector set(const std::vector<float>& source);

    static PVector random2D();
    static PVector random3D();
    static PVector fromAngle(float angle);

    PVector copy() const;
    void    get(std::vector<float>& target) const;

    float mag() const;
    float magSq() const;

    PVector&    add(const PVector& v);
    PVector&    add(float x, float y, float z = 0);
    static void add(const PVector& v1, const PVector& v2, PVector& target);

    PVector&    sub(const PVector& v);
    PVector&    sub(float x, float y, float z = 0);
    static void sub(const PVector& v1, const PVector& v2, PVector& target);

    PVector&    mult(float n);
    static void mult(const PVector& v, float n, PVector& target);

    PVector&    div(float n);
    static void div(const PVector& v, float n, PVector& target);

    float        dist(const PVector& v) const;
    static float dist(const PVector& v1, const PVector& v2);

    float        dot(const PVector& v) const;
    float        dot(float x, float y, float z) const;
    static float dot(const PVector& v1, const PVector& v2);

    void        cross(const PVector& v, PVector& target) const;
    static void cross(const PVector& v1, const PVector& v2, PVector& target);

    PVector& normalize();
    void     normalize(PVector& target) const;

    PVector& limit(float max);
    PVector& setMag(float len);
    void     setMag(PVector& target, float len) const;

    float    heading() const;
    PVector& setHeading(float angle);
    PVector& rotate(float theta);

    PVector&        lerp(const PVector& v, float amt);
    static PVector& lerp(const PVector& v1, const PVector& v2, float amt);
    PVector&        lerp(float x, float y, float z, float amt);

    static float angleBetween(const PVector& v1, const PVector& v2);

    std::vector<float> array() const;

    bool equals(const PVector& v) const;
    int  hashCode() const;

    std::string toString() const;

private:
    std::vector<float> array_; // Transient array for temporary use
};

#endif // PVECTOR_H
