#pragma once

#include <cmath>
#include <sstream>
#include <iostream>

namespace ppp
{
    // base_vec3
    template<typename T = double>
    class base_vec2
    {
    public:
        // Constructors with default values
        base_vec2(T x = 0, T y = 0)
            : x(x), y(y)
        {}

        // Returns a string representation
        std::string to_string() const
        {
            std::ostringstream oss;
            oss << "base_vec2 : [" << x << ", " << y << "]";
            return oss.str();
        }

        // Set the vector's components (overloaded for individual values or another base_vec2)
        base_vec2& set(T x, T y)
        {
            this->x = x;
            this->y = y;
            return *this;
        }
        base_vec2& set(const base_vec2& v)
        {
            this->x = v.x;
            this->y = v.y;
            return *this;
        }

        // Returns a copy of this vector
        base_vec2 copy() const
        {
            return base_vec2(x, y);
        }

        // Add components to the vector (overloaded for a scalar triplet or another vector)
        base_vec2& add(T x, T y)
        {
            this->x += x;
            this->y += y;
            return *this;
        }
        base_vec2& add(const base_vec2& v)
        {
            this->x += v.x;
            this->y += v.y;
            return *this;
        }

        // Subtract components from the vector
        base_vec2& sub(T x, T y)
        {
            this->x -= x;
            this->y -= y;
            return *this;
        }
        base_vec2& sub(const base_vec2& v)
        {
            this->x -= v.x;
            this->y -= v.y;
            return *this;
        }

        // Multiply the vector by a scalar or do element-wise multiplication with another vector
        base_vec2& mult(T scalar)
        {
            this->x *= scalar;
            this->y *= scalar;
            return *this;
        }
        base_vec2& mult(const base_vec2& v)
        {
            this->x *= v.x;
            this->y *= v.y;
            return *this;
        }

        // Divide the vector by a scalar or do element-wise division by another vector
        base_vec2& div(T scalar)
        {
            if (scalar == 0)
            {
                std::cerr << "Warning: Divide by zero in base_vec2::div(T)" << std::endl;
                return *this;
            }

            this->x /= scalar;
            this->y /= scalar;
            return *this;
        }
        base_vec2& div(const base_vec2& v)
        {
            if (v.x == 0 || v.y == 0)
            {
                std::cerr << "Warning: Divide by zero in base_vec2::div(base_vec2)" << std::endl;
                return *this;
            }

            this->x /= v.x;
            this->y /= v.y;
            return *this;
        }

        // Remainder (modulo) for each component
        base_vec2& rem(T val)
        {
            if (val != 0)
            {
                x = std::fmod(x, val);
                y = std::fmod(y, val);
            }

            return *this;
        }
        base_vec2& rem(const base_vec2& v)
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

        // Dot product (overloaded for a base_vec2 or individual components)
        T dot(const base_vec2& v) const
        {
            return x * v.x + y * v.y;
        }

        // Dot product (overloaded for a base_vec2 or individual components)
        T dot(T x, T y, T z = 0) const
        {
            return this->x * x + this->y * y;
        }

        // Distance between two vectors
        T dist(const base_vec2& v) const
        {
            return (*this - v).mag();
        }

        // Normalize the vector (set magnitude to 1)
        base_vec2& normalize()
        {
            T m = mag();
            if (m != 0)
            {
                mult(1.0 / m);
            }
            return *this;
        }

        // Limit the magnitude of the vector to a maximum value
        base_vec2& limit(T max)
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
        base_vec2& set_mag(T len)
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
        base_vec2& set_heading(T angle)
        {
            T m = mag();
            x = std::cos(angle) * m;
            y = std::sin(angle) * m;
            return *this;
        }

        // Operator overloads for convenience
        base_vec2 operator+(const base_vec2& v) const
        {
            return base_vec2(x + v.x, y + v.y);
        }
        base_vec2 operator-(const base_vec2& v) const
        {
            return base_vec2(x - v.x, y - v.y);
        }
        base_vec2& operator+=(const base_vec2& v)
        {
            return add(v);
        }
        base_vec2& operator-=(const base_vec2& v)
        {
            return sub(v);
        }
        base_vec2 operator*(T scalar) const
        {
            return base_vec2(x * scalar, y * scalar);
        }
        base_vec2 operator/(T scalar) const
        {
            return base_vec2(x / scalar, y / scalar);
        }

    public:
        T x, y;
    };

    // base_vec3
    template<typename T = double>
    class base_vec3
    {
    public:
        // Constructors with default values
        base_vec3(T x = 0, T y = 0, T z = 0)
            : x(x), y(y), z(z)
        {}

        // Returns a string representation
        std::string to_string() const
        {
            std::ostringstream oss;
            oss << "base_vec3 : [" << x << ", " << y << ", " << z << "]";
            return oss.str();
        }

        // Set the vector's components (overloaded for individual values or another base_vec3)
        base_vec3& set(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            return *this;
        }
        base_vec3& set(const base_vec3& v)
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            return *this;
        }

        // Returns a copy of this vector
        base_vec3 copy() const
        {
            return base_vec3(x, y, z);
        }

        // Add components to the vector (overloaded for a scalar triplet or another vector)
        base_vec3& add(T x, T y, T z)
        {
            this->x += x;
            this->y += y;
            this->z += z;
            return *this;
        }
        base_vec3& add(const base_vec3& v)
        {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }

        // Subtract components from the vector
        base_vec3& sub(T x, T y, T z)
        {
            this->x -= x;
            this->y -= y;
            this->z -= z;
            return *this;
        }
        base_vec3& sub(const base_vec3& v)
        {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            return *this;
        }

        // Multiply the vector by a scalar or do element-wise multiplication with another vector
        base_vec3& mult(T scalar)
        {
            this->x *= scalar;
            this->y *= scalar;
            this->z *= scalar;
            return *this;
        }
        base_vec3& mult(const base_vec3& v)
        {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            return *this;
        }

        // Divide the vector by a scalar or do element-wise division by another vector
        base_vec3& div(T scalar)
        {
            if (scalar == 0)
            {
                std::cerr << "Warning: Divide by zero in base_vec3::div(T)" << std::endl;
                return *this;
            }

            this->x /= scalar;
            this->y /= scalar;
            this->z /= scalar;
            return *this;
        }
        base_vec3& div(const base_vec3& v)
        {
            if (v.x == 0 || v.y == 0 || v.z == 0)
            {
                std::cerr << "Warning: Divide by zero in base_vec3::div(base_vec3)" << std::endl;
                return *this;
            }

            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            return *this;
        }

        // Remainder (modulo) for each component
        base_vec3& rem(T val)
        {
            if (val != 0)
            {
                x = std::fmod(x, val);
                y = std::fmod(y, val);
                z = std::fmod(z, val);
            }

            return *this;
        }
        base_vec3& rem(const base_vec3& v)
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

        // Dot product (overloaded for a base_vec3 or individual components)
        T dot(const base_vec3& v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }
        T dot(T x, T y, T z = 0) const
        {
            return this->x * x + this->y * y + this->z * z;
        }

        // Cross product (only meaningful in 3D)
        base_vec3 cross(const base_vec3& v) const
        {
            return base_vec3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }

        // Distance between two vectors
        T dist(const base_vec3& v) const
        {
            return (*this - v).mag();
        }

        // Normalize the vector (set magnitude to 1)
        base_vec3& normalize()
        {
            T m = mag();
            if (m != 0)
            {
                mult(1.0 / m);
            }
            return *this;
        }

        // Limit the magnitude of the vector to a maximum value
        base_vec3& limit(T max)
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
        base_vec3& set_mag(T len)
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
        base_vec3& set_heading(T angle)
        {
            T m = mag();
            x = std::cos(angle) * m;
            y = std::sin(angle) * m;
            return *this;
        }

        // Operator overloads for convenience
        base_vec3 operator+(const base_vec3& v) const
        {
            return base_vec3(x + v.x, y + v.y, z + v.z);
        }
        base_vec3 operator-(const base_vec3& v) const
        {
            return base_vec3(x - v.x, y - v.y, z - v.z);
        }
        base_vec3& operator+=(const base_vec3& v)
        {
            return add(v);
        }
        base_vec3& operator-=(const base_vec3& v)
        {
            return sub(v);
        }
        base_vec3 operator*(T scalar) const
        {
            return base_vec3(x * scalar, y * scalar, z * scalar);
        }
        base_vec3 operator/(T scalar) const
        {
            return base_vec3(x / scalar, y / scalar, z / scalar);
        }

    public:
        T x, y, z;
    };

    // base_vec4
    template<typename T = double>
    class base_vec4 {
    public:
        T x, y, z, w;

        // Constructor with default values (default: zero vector)
        base_vec4(T x = 0, T y = 0, T z = 0, T w = 0)
            : x(x), y(y), z(z), w(w)
        {}

        // Returns a string representation
        std::string to_string() const 
        {
            std::ostringstream oss;
            oss << "base_vec4 : [" << x << ", " << y << ", " << z << ", " << w << "]";
            return oss.str();
        }

        // Set the vector's components (individual values)
        base_vec4& set(T x, T y, T z, T w) 
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
            return *this;
        }
        // Set the vector's components from another base_vec4
        base_vec4& set(const base_vec4& v) 
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            this->w = v.w;
            return *this;
        }

        // Returns a copy of this vector
        base_vec4 copy() const 
        {
            return base_vec4(x, y, z, w);
        }

        // Add components to the vector (using individual values)
        base_vec4& add(T x, T y, T z, T w) 
        {
            this->x += x;
            this->y += y;
            this->z += z;
            this->w += w;
            return *this;
        }
        // Add another vector
        base_vec4& add(const base_vec4& v) 
        {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            this->w += v.w;
            return *this;
        }

        // Subtract components from the vector (using individual values)
        base_vec4& sub(T x, T y, T z, T w) 
        {
            this->x -= x;
            this->y -= y;
            this->z -= z;
            this->w -= w;
            return *this;
        }
        // Subtract another vector
        base_vec4& sub(const base_vec4& v) 
        {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            this->w -= v.w;
            return *this;
        }

        // Multiply the vector by a scalar
        base_vec4& mult(T scalar) 
        {
            this->x *= scalar;
            this->y *= scalar;
            this->z *= scalar;
            this->w *= scalar;
            return *this;
        }
        // Element-wise multiplication with another vector
        base_vec4& mult(const base_vec4& v) 
        {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            this->w *= v.w;
            return *this;
        }

        // Divide the vector by a scalar
        base_vec4& div(T scalar) 
        {
            if (scalar == 0) {
                std::cerr << "Warning: Divide by zero in base_vec4::div(T)" << std::endl;
                return *this;
            }
            this->x /= scalar;
            this->y /= scalar;
            this->z /= scalar;
            this->w /= scalar;
            return *this;
        }
        // Element-wise division by another vector
        base_vec4& div(const base_vec4& v) 
        {
            if (v.x == 0 || v.y == 0 || v.z == 0 || v.w == 0) {
                std::cerr << "Warning: Divide by zero in base_vec4::div(base_vec4)" << std::endl;
                return *this;
            }
            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            this->w /= v.w;
            return *this;
        }

        // Remainder (modulo) for each component with a scalar
        base_vec4& rem(T val) 
        {
            if (val != 0) {
                x = std::fmod(x, val);
                y = std::fmod(y, val);
                z = std::fmod(z, val);
                w = std::fmod(w, val);
            }
            return *this;
        }
        // Remainder (modulo) for each component with another vector
        base_vec4& rem(const base_vec4& v) 
        {
            if (v.x != 0) x = std::fmod(x, v.x);
            if (v.y != 0) y = std::fmod(y, v.y);
            if (v.z != 0) z = std::fmod(z, v.z);
            if (v.w != 0) w = std::fmod(w, v.w);
            return *this;
        }

        // Returns the squared magnitude of the vector
        T mag_sq() const 
        {
            return x * x + y * y + z * z + w * w;
        }

        // Returns the magnitude (length) of the vector
        T mag() const 
        {
            return std::sqrt(mag_sq());
        }

        // Dot product with another vector
        T dot(const base_vec4& v) const 
        {
            return x * v.x + y * v.y + z * v.z + w * v.w;
        }
        // Dot product using individual components
        T dot(T x, T y, T z, T w) const 
        {
            return this->x * x + this->y * y + this->z * z + this->w * w;
        }

        // Distance between two vectors
        T dist(const base_vec4& v) const 
        {
            return (*this - v).mag();
        }

        // Normalize the vector (set magnitude to 1)
        base_vec4& normalize() 
        {
            T m = mag();
            if (m != 0) {
                mult(1.0 / m);
            }
            return *this;
        }

        // Limit the magnitude of the vector to a maximum value
        base_vec4& limit(T max) 
        {
            T m_sq = mag_sq();
            if (m_sq > max * max) {
                normalize();
                mult(max);
            }
            return *this;
        }

        // Set the magnitude to a given value
        base_vec4& set_mag(T len) 
        {
            normalize();
            mult(len);
            return *this;
        }

        // Operator overloads for convenience
        base_vec4 operator+(const base_vec4& v) const 
        {
            return base_vec4(x + v.x, y + v.y, z + v.z, w + v.w);
        }
        base_vec4 operator-(const base_vec4& v) const 
        {
            return base_vec4(x - v.x, y - v.y, z - v.z, w - v.w);
        }
        base_vec4& operator+=(const base_vec4& v) 
        {
            return add(v);
        }
        base_vec4& operator-=(const base_vec4& v) 
        {
            return sub(v);
        }
        base_vec4 operator*(T scalar) const 
        {
            return base_vec4(x * scalar, y * scalar, z * scalar, w * scalar);
        }
        base_vec4 operator/(T scalar) const 
        {
            return base_vec4(x / scalar, y / scalar, z / scalar, w / scalar);
        }
    };

    using vec2f = base_vec2<float>;
    using vec2d = base_vec2<double>;
    using vec2 = vec2f;

    using vec3f = base_vec3<float>;
    using vec3d = base_vec3<double>;
    using vec3 = vec3f;

    using vec4f = base_vec4<float>;
    using vec4d = base_vec4<double>;
    using vec4 = vec4f;
}