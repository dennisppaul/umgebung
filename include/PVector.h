/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2023 Dennis P Paul.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cmath>
#include <random>

namespace umgebung {

    /*
      PVector operator+(const PVector &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    PVector operator-(const PVector &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    PVector operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    PVector operator/(float scalar) const {
        if (scalar != 0) {
            return {x / scalar, y / scalar, z / scalar};
        }
        return {x, y, z};
    }
  */

    //#include <cmath>
    //#include <random>

#ifndef M_PI
#define M_PI 3.141592653589
#endif

    class PVector {
    public:
        float x, y, z;

        // Constructors
        PVector() : x(0), y(0), z(0) {}

        PVector(float x, float y, float z = 0) : x(x), y(y), z(z) {}

        // Set the components of the vector
        void set(float _x, float _y, float _z = 0) {
            x = _x;
            y = _y;
            z = _z;
        }

        // Make a new 2D unit vector with a random direction
        static PVector random2D() {
            static std::default_random_engine            generator;
            static std::uniform_real_distribution<float> distribution(0.0, 1.0);
            float                                        angle = distribution(generator) * 2 * M_PI;
            return {(float) cos(angle), (float) sin(angle)};
        }

        // Make a new 3D unit vector with a random direction
        static PVector random3D() {
            static std::default_random_engine            generator;
            static std::uniform_real_distribution<float> distribution(0.0, 1.0);
            auto                                         angle1 = (float) (distribution(generator) * 2 * M_PI);
            auto                                         angle2 = (float) (distribution(generator) * 2 * M_PI);
            return {(float) cos(angle1) * (float) sin(angle2), (float) sin(angle1) * (float) sin(angle2),
                    (float) cos(angle2)};
        }

        // Make a new 2D unit vector from an angle
        static PVector fromAngle(float angle) {
            return {(float) cos(angle), (float) sin(angle)};
        }

        // Get a copy of the vector
        [[nodiscard]] PVector copy() const {
            return {x, y, z};
        }

        // Calculate the magnitude of the vector
        [[nodiscard]] float mag() const {
            return sqrt(x * x + y * y + z * z);
        }

        // Calculate the magnitude of the vector, squared
        [[nodiscard]] float magSq() const {
            return x * x + y * y + z * z;
        }

        // Adds x, y, and z components to a vector, one vector to another, or two independent vectors
        void add(float _x, float _y, float _z = 0) {
            x += _x;
            y += _y;
            z += _z;
        }

        void add(const PVector& v) {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        static PVector add(const PVector& v1, const PVector& v2) {
            return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
        }

        // Subtract x, y, and z components from a vector, one vector from another, or two independent vectors
        void sub(float _x, float _y, float _z = 0) {
            x -= _x;
            y -= _y;
            z -= _z;
        }

        void sub(const PVector& v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }

        static PVector sub(const PVector& v1, const PVector& v2) {
            return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
        }

        // Multiply a vector by a scalar
        void mult(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

        static PVector mult(const PVector& v, float scalar) {
            return {v.x * scalar, v.y * scalar, v.z * scalar};
        }

        // Divide a vector by a scalar
        void div(float scalar) {
            if (scalar != 0) {
                x /= scalar;
                y /= scalar;
                z /= scalar;
            }
        }

        static PVector div(const PVector& v, float scalar) {
            if (scalar != 0) {
                return {v.x / scalar, v.y / scalar, v.z / scalar};
            }
            return {0, 0, 0}; // Return a zero vector if division by zero
        }

        // Calculate the distance between two points
        static float dist(const PVector& v1, const PVector& v2) {
            float dx = v2.x - v1.x;
            float dy = v2.y - v1.y;
            float dz = v2.z - v1.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        // Calculate the dot product of two vectors
        static float dot(const PVector& v1, const PVector& v2) {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }

        // Calculate and return the cross product
        static PVector cross(const PVector& v1, const PVector& v2) {
            return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
        }

        // Normalize the vector to a length of 1
        void normalize() {
            float m = mag();
            if (m != 0) {
                div(m);
            }
        }

        // Limit the magnitude of the vector
        void limit(float max) {
            if (mag() > max) {
                normalize();
                mult(max);
            }
        }

        // Set the magnitude of the vector
        void setMag(float len) {
            normalize();
            mult(len);
        }

        // Calculate the angle of rotation for this vector
        [[nodiscard]] float heading() const {
            return atan2(y, x);
        }

        // Rotate the vector by an angle (2D only)
        void rotate(float angle) {
            float new_x = x * (float) cos(angle) - y * (float) sin(angle);
            float new_y = x * (float) sin(angle) + y * (float) cos(angle);
            x           = new_x;
            y           = new_y;
        }

        // Linear interpolate the vector to another vector
        void lerp(const PVector& target, float amount) {
            x = x + (target.x - x) * amount;
            y = y + (target.y - y) * amount;
            z = z + (target.z - z) * amount;
        }

        // Calculate and return the angle between two vectors
        static float angleBetween(const PVector& v1, const PVector& v2) {
            float dotProduct = dot(v1, v2);
            float magV1      = v1.mag();
            float magV2      = v2.mag();
            return acos(dotProduct / (magV1 * magV2));
        }

        // Return a representation of the vector as a float array
        [[nodiscard]] float* array() const {
            auto* arr = new float[3];
            arr[0]    = x;
            arr[1]    = y;
            arr[2]    = z;
            return arr;
        }
    };
} // namespace umgebung