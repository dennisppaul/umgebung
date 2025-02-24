#include <sstream>
#include <stdexcept>

#include "PVector.h"

PVector PVector::set(const float x, const float y, const float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
}

PVector PVector::set(const PVector& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

PVector PVector::set(const std::vector<float>& source) {
    if (source.size() >= 2) {
        x = source[0];
        y = source[1];
    }
    if (source.size() >= 3) {
        z = source[2];
    } else {
        z = 0;
    }
    return *this;
}

PVector PVector::random2D() {
    const float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
    return fromAngle(angle);
}

PVector PVector::random3D() {
    const float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
    const float vz    = static_cast<float>(rand()) / RAND_MAX * 2 - 1;
    const float vx    = sqrt(1 - vz * vz) * cos(angle);
    const float vy    = sqrt(1 - vz * vz) * sin(angle);
    return PVector(vx, vy, vz);
}

PVector PVector::fromAngle(const float angle) {
    return PVector(cos(angle), sin(angle), 0);
}

PVector PVector::copy() const { return *this; }

void PVector::get(std::vector<float>& target) const {
    if (target.size() < 2) {
        target.resize(2);
    }
    target[0] = x;
    target[1] = y;
    target[2] = z;
}

float PVector::mag() const {
    return sqrt(x * x + y * y + z * z);
}

float PVector::magSq() const {
    return x * x + y * y + z * z;
}

PVector& PVector::add(const PVector& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

PVector& PVector::add(const float x, const float y, const float z) {
    this->x += x;
    this->y += y;
    this->z += z;
    return *this;
}

void PVector::add(const PVector& v1, const PVector& v2, PVector& target) {
    target.set(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

PVector& PVector::sub(const PVector& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

PVector& PVector::sub(const float x, const float y, const float z) {
    this->x -= x;
    this->y -= y;
    this->z -= z;
    return *this;
}

void PVector::sub(const PVector& v1, const PVector& v2, PVector& target) {
    target.set(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

PVector& PVector::mult(const float n) {
    x *= n;
    y *= n;
    z *= n;
    return *this;
}

void PVector::mult(const PVector& v, const float n, PVector& target) {
    target.set(v.x * n, v.y * n, v.z * n);
}

PVector& PVector::div(const float n) {
    if (n == 0) {
        throw std::runtime_error("Division by zero");
    }
    x /= n;
    y /= n;
    z /= n;
    return *this;
}

void PVector::div(const PVector& v, const float n, PVector& target) {
    if (n == 0) {
        throw std::runtime_error("Division by zero");
    }
    target.set(v.x / n, v.y / n, v.z / n);
}

float PVector::dist(const PVector& v) const {
    const float dx = x - v.x;
    const float dy = y - v.y;
    const float dz = z - v.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

float PVector::dist(const PVector& v1, const PVector& v2) {
    return v1.dist(v2);
}

float PVector::dot(const PVector& v) const {
    return x * v.x + y * v.y + z * v.z;
}

float PVector::dot(const float x, const float y, const float z) const {
    return this->x * x + this->y * y + this->z * z;
}

float PVector::dot(const PVector& v1, const PVector& v2) {
    return v1.dot(v2);
}

void PVector::cross(const PVector& v, PVector& target) const {
    const float crossX = y * v.z - v.y * z;
    const float crossY = z * v.x - v.z * x;
    const float crossZ = x * v.y - v.x * y;

    target.set(crossX, crossY, crossZ);
}

void PVector::cross(const PVector& v1, const PVector& v2, PVector& target) {
    return v1.cross(v2, target);
}

PVector& PVector::normalize() {
    const float m = mag();
    if (m != 0 && m != 1) {
        div(m);
    }
    return *this;
}

void PVector::normalize(PVector& target) const {
    const float m = mag();
    target.set(x / m, y / m, z / m);
}

PVector& PVector::limit(const float max) {
    if (magSq() > max * max) {
        normalize();
        mult(max);
    }
    return *this;
}

PVector& PVector::setMag(const float len) {
    normalize();
    mult(len);
    return *this;
}

void PVector::setMag(PVector& target, const float len) const {
    normalize(target);
    target.mult(len);
}

float PVector::heading() const {
    return atan2(y, x);
}

PVector& PVector::setHeading(const float angle) {
    const float m = mag();
    x             = m * cos(angle);
    y             = m * sin(angle);
    return *this;
}

PVector& PVector::rotate(const float theta) {
    const float temp = x;
    x                = x * cos(theta) - y * sin(theta);
    y                = temp * sin(theta) + y * cos(theta);
    return *this;
}

PVector& PVector::lerp(const PVector& v, const float amt) {
    x = x + (v.x - x) * amt;
    y = y + (v.y - y) * amt;
    z = z + (v.z - z) * amt;
    return *this;
}

PVector& PVector::lerp(const PVector& v1, const PVector& v2, const float amt) {
    return v1.copy().lerp(v2, amt);
}

PVector& PVector::lerp(const float x, const float y, const float z, const float amt) {
    this->x = this->x + (x - this->x) * amt;
    this->y = this->y + (y - this->y) * amt;
    this->z = this->z + (z - this->z) * amt;
    return *this;
}

float PVector::angleBetween(const PVector& v1, const PVector& v2) {
    const float dot   = v1.dot(v2);
    const float v1mag = v1.mag();
    const float v2mag = v2.mag();
    return acos(dot / (v1mag * v2mag));
}

std::vector<float> PVector::array() const {
    return {x, y, z};
}

bool PVector::equals(const PVector& v) const {
    constexpr float epsilon = 1e-6;
    return fabs(x - v.x) < epsilon &&
           fabs(y - v.y) < epsilon &&
           fabs(z - v.z) < epsilon;
}

int PVector::hashCode() const {
    int result = 1;
    result     = 31 * result + *(int*) &x;
    result     = 31 * result + *(int*) &y;
    result     = 31 * result + *(int*) &z;
    return result;
}

std::string PVector::toString() const {
    std::ostringstream oss;
    oss << "[ " << x << ", " << y << ", " << z << " ]";
    return oss.str();
}
