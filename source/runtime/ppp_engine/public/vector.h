#pragma once

#include <cmath>
#include <sstream>
#include <iostream>

namespace ppp 
{
    template<typename T = double>
    class vec2
    {
    public:
        // Constructors with default values
        vec2(T x = 0, T y = 0)
            : x(x), y(y)
        {}

        // Returns a string representation
        std::string to_string() const 
        {
            std::ostringstream oss;
            oss << "vec2 : [" << x << ", " << y << "]";
            return oss.str();
        }

        // Set the vector's components (overloaded for individual values or another vec2)
        vec2& set(T x, T y) 
        {
            this->x = x;
            this->y = y;
            return *this;
        }
        vec2& set(const vec2& v) 
        {
            this->x = v.x;
            this->y = v.y;
            return *this;
        }

        // Returns a copy of this vector
        vec2 copy() const 
        {
            return vec2(x, y);
        }

        // Add components to the vector (overloaded for a scalar triplet or another vector)
        vec2& add(T x, T y) 
        {
            this->x += x;
            this->y += y;
            return *this;
        }
        vec2& add(const vec2& v) 
        {
            this->x += v.x;
            this->y += v.y;
            return *this;
        }

        // Subtract components from the vector
        vec2& sub(T x, T y) 
        {
            this->x -= x;
            this->y -= y;
            return *this;
        }
        vec2& sub(const vec2& v) 
        {
            this->x -= v.x;
            this->y -= v.y;
            return *this;
        }

        // Multiply the vector by a scalar or do element-wise multiplication with another vector
        vec2& mult(T scalar) 
        {
            this->x *= scalar;
            this->y *= scalar;
            return *this;
        }
        vec2& mult(const vec2& v) 
        {
            this->x *= v.x;
            this->y *= v.y;
            return *this;
        }

        // Divide the vector by a scalar or do element-wise division by another vector
        vec2& div(T scalar) 
        {
            if (scalar == 0) 
            {
                std::cerr << "Warning: Divide by zero in vec2::div(T)" << std::endl;
                return *this;
            }

            this->x /= scalar;
            this->y /= scalar;
            return *this;
        }
        vec2& div(const vec2& v) 
        {
            if (v.x == 0 || v.y == 0 || v.z == 0) 
            {
                std::cerr << "Warning: Divide by zero in vec2::div(vec2)" << std::endl;
                return *this;
            }

            this->x /= v.x;
            this->y /= v.y;
            return *this;
        }

        // Remainder (modulo) for each component
        vec2& rem(T val) 
        {
            if (val != 0) 
            {
                x = std::fmod(x, val);
                y = std::fmod(y, val);
            }

            return *this;
        }
        vec2& rem(const vec2& v) 
        {
            if (v.x != 0) x = std::fmod(x, v.x);
            if (v.y != 0) y = std::fmod(y, v.y);

            return *this;
        }

        // Returns the magnitude (length) of the vector
        T mag() const 
        {
            return std::sqrt(mag_sq());
        }

        // Returns the squared magnitude
        T mag_sq() const 
        {
            return x * x + y * y;
        }

        // Dot product (overloaded for a vec2 or individual components)
        T dot(const vec2& v) const 
        {
            return x * v.x + y * v.y;
        }
        T dot(T x, T y, T z = 0) const 
        {
            return this->x * x + this->y * y;
        }

        // Distance between two vectors
        T dist(const vec2& v) const 
        {
            return (*this - v).mag();
        }

        // Normalize the vector (set magnitude to 1)
        vec2& normalize() 
        {
            T m = mag();
            if (m != 0) 
            {
                mult(1.0 / m);
            }
            return *this;
        }

        // Limit the magnitude of the vector to a maximum value
        vec2& limit(T max) 
        {
            T m_sq = mag_sq();
            if (m_sq > max * max)
            {
                normalize();
                mult(max);
            }
            return *this;
        }

        // Set the magnitude to a given value
        vec2& set_mag(T len) 
        {
            normalize();
            mult(len);
            return *this;
        }

        // Heading (angle in radians) of a 2D vector (ignores z)
        T heading() const 
        {
            return std::atan2(y, x);
        }

        // Set the heading (angle in radians) while preserving magnitude (2D)
        vec2& set_heading(T angle) 
        {
            T m = mag();
            x = std::cos(angle) * m;
            y = std::sin(angle) * m;
            return *this;
        }

        // Operator overloads for convenience
        vec2 operator+(const vec2& v) const 
        {
            return vec2(x + v.x, y + v.y);
        }
        vec2 operator-(const vec2& v) const 
        {
            return vec2(x - v.x, y - v.y);
        }
        vec2& operator+=(const vec2& v) 
        {
            return add(v);
        }
        vec2& operator-=(const vec2& v) 
        {
            return sub(v);
        }
        vec2 operator*(T scalar) const 
        {
            return vec2(x * scalar, y * scalar);
        }
        vec2 operator/(T scalar) const 
        {
            return vec2(x / scalar, y / scalar);
        }

    public:
        T x, y;
    };

    // vec3
    template<typename T = double>
    class vec3
    {
    public:
        // Constructors with default values
        vec3(T x = 0, T y = 0, T z = 0)
            : x(x), y(y), z(z) 
        {}

        // Returns a string representation
        std::string to_string() const 
        {
            std::ostringstream oss;
            oss << "vec3 : [" << x << ", " << y << ", " << z << "]";
            return oss.str();
        }

        // Set the vector's components (overloaded for individual values or another vec3)
        vec3& set(T x, T y, T z) 
        {
            this->x = x;
            this->y = y;
            this->z = z;
            return *this;
        }
        vec3& set(const vec3& v) 
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            return *this;
        }

        // Returns a copy of this vector
        vec3 copy() const 
        {
            return vec3(x, y, z);
        }

        // Add components to the vector (overloaded for a scalar triplet or another vector)
        vec3& add(T x, T y, T z) 
        {
            this->x += x;
            this->y += y;
            this->z += z;
            return *this;
        }
        vec3& add(const vec3& v) 
        {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }

        // Subtract components from the vector
        vec3& sub(T x, T y, T z) 
        {
            this->x -= x;
            this->y -= y;
            this->z -= z;
            return *this;
        }
        vec3& sub(const vec3& v) 
        {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            return *this;
        }

        // Multiply the vector by a scalar or do element-wise multiplication with another vector
        vec3& mult(T scalar) 
        {
            this->x *= scalar;
            this->y *= scalar;
            this->z *= scalar;
            return *this;
        }
        vec3& mult(const vec3& v) 
        {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            return *this;
        }

        // Divide the vector by a scalar or do element-wise division by another vector
        vec3& div(T scalar) 
        {
            if (scalar == 0) 
            {
                std::cerr << "Warning: Divide by zero in vec3::div(T)" << std::endl;
                return *this;
            }

            this->x /= scalar;
            this->y /= scalar;
            this->z /= scalar;
            return *this;
        }
        vec3& div(const vec3& v) 
        {
            if (v.x == 0 || v.y == 0 || v.z == 0) 
            {
                std::cerr << "Warning: Divide by zero in vec3::div(vec3)" << std::endl;
                return *this;
            }

            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            return *this;
        }

        // Remainder (modulo) for each component
        vec3& rem(T val) 
        {
            if (val != 0) 
            {
                x = std::fmod(x, val);
                y = std::fmod(y, val);
                z = std::fmod(z, val);
            }

            return *this;
        }
        vec3& rem(const vec3& v) 
        {
            if (v.x != 0) x = std::fmod(x, v.x);
            if (v.y != 0) y = std::fmod(y, v.y);
            if (v.z != 0) z = std::fmod(z, v.z);

            return *this;
        }

        // Returns the magnitude (length) of the vector
        T mag() const 
        {
            return std::sqrt(mag_sq());
        }

        // Returns the squared magnitude
        T mag_sq() const 
        {
            return x * x + y * y + z * z;
        }

        // Dot product (overloaded for a vec3 or individual components)
        T dot(const vec3& v) const 
        {
            return x * v.x + y * v.y + z * v.z;
        }
        T dot(T x, T y, T z = 0) const 
        {
            return this->x * x + this->y * y + this->z * z;
        }

        // Cross product (only meaningful in 3D)
        vec3 cross(const vec3& v) const 
        {
            return vec3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }

        // Distance between two vectors
        T dist(const vec3& v) const 
        {
            return (*this - v).mag();
        }

        // Normalize the vector (set magnitude to 1)
        vec3& normalize() 
        {
            T m = mag();
            if (m != 0) 
            {
                mult(1.0 / m);
            }
            return *this;
        }

        // Limit the magnitude of the vector to a maximum value
        vec3& limit(T max) 
        {
            T m_sq = mag_sq();
            if (m_sq > max * max)
            {
                normalize();
                mult(max);
            }
            return *this;
        }

        // Set the magnitude to a given value
        vec3& set_mag(T len) 
        {
            normalize();
            mult(len);
            return *this;
        }

        // Heading (angle in radians) of a 2D vector (ignores z)
        T heading() const 
        {
            return std::atan2(y, x);
        }

        // Set the heading (angle in radians) while preserving magnitude (2D)
        vec3& set_heading(T angle) 
        {
            T m = mag();
            x = std::cos(angle) * m;
            y = std::sin(angle) * m;
            return *this;
        }

        // Operator overloads for convenience
        vec3 operator+(const vec3& v) const 
        {
            return vec3(x + v.x, y + v.y, z + v.z);
        }
        vec3 operator-(const vec3& v) const 
        {
            return vec3(x - v.x, y - v.y, z - v.z);
        }
        vec3& operator+=(const vec3& v) 
        {
            return add(v);
        }
        vec3& operator-=(const vec3& v) 
        {
            return sub(v);
        }
        vec3 operator*(T scalar) const 
        {
            return vec3(x * scalar, y * scalar, z * scalar);
        }
        vec3 operator/(T scalar) const 
        {
            return vec3(x / scalar, y / scalar, z / scalar);
        }

    public:
        T x, y, z;
    };
}