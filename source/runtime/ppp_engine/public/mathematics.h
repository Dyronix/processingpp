#pragma once

#include "vector.h"

#include <algorithm>
#include <cmath>

namespace ppp
{
    namespace math
    {
        // Calculation
        // Calculates the absolute value of a number.
        template<typename T>
        T abs(T v)
        {
            return std::abs(v);
        }
        // Calculates the closest integer value that is greater than or equal to a number.
        template<typename T>
        T ceil(T v)
        {
            return std::ceil(v);
        }
        // Constrains a number between a minimum and maximum value.
        template<typename T>
        T constrain(T v, T min, T max)
        {
            return std::clamp(v, min, max);
        }
        // Calculates the distance between two 2D points.
        template<typename T>
        T dist(T x1, T y1, T x2, T y2)
        {
            return std::hypot(x1 - x2, y1 - y2);
        }
        // Calculates the distance between two 2D points.
        template<typename T>
        T dist(const base_vec2<T>& v1, const base_vec2<T>& v2)
        {
            return std::hypot(v1.x - v2.x, v1.y - v2.y);
        }
        // Calculates the distance between two 3D points.
        template<typename T>
        T dist(T x1, T y1, T z1, T x2, T y2, T z2)
        {
            return std::hypot(x1 - x2, y1 - y2, z1 - z2);
        }
        // Calculates the distance between two 3D points.
        template<typename T>
        T dist(const base_vec3<T>& v1, const base_vec3<T>& v2)
        {
            return std::hypot(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
        }
        // Calculates the value of Euler's number e (2.71828...) raised to the power of a number.
        template<typename T>
        T exp(T v)
        {
            return std::exp(v);
        }
        // Calculates the closest integer value that is less than or equal to the value of a number.
        template<typename T>
        T floor(T v)
        {
            return std::floor(v);
        }
        // Calculates the fractional part of a number.
        template<typename T>
        T fract(T v, T* iptr)
        {
            return std::modf(v, iptr);
        }
        // Calculates a number between two numbers at a specific increment.
        template<typename T>
        T lerp(T from, T to, T t)
        {
            return from + t * (to - from);
        }
        // Calculates the natural logarithm (the base-e logarithm) of a number.
        template<typename T>
        T log(T v)
        {
            return std::log(v);
        }
        // Calculates the magnitude, or length, of a vector.
        template<typename T>
        T mag(T x, T y)
        {
            return std::hypot(x, y);
        }
        // Calculates the magnitude, or length, of a vector.
        template<typename T>
        T mag(T x, T y, T z)
        {
            return std::hypot(x, y, z);
        }
        // Re-maps a number from one range to another.
        template <typename T>
        T map(T value, T in_min, T in_max, T out_min, T out_max)
        {
            return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
        }
        // Returns the largest value in a sequence of numbers.
        template<typename T>
        T max(T v, T other)
        {
            return (std::max)(v, other);
        }
        // Returns the smallest value in a sequence of numbers.
        template<typename T>
        T min(T v, T other)
        {
            return (std::min)(v, other);
        }
        // Maps a number from one range to a value between 0 and 1.
        template<typename T>
        T norm(T n, T start, T end)
        {
            return map(n, start, end, static_cast<T>(0), static_cast<T>(1));
        }
        // Calculates exponential expressions such as 2^3.
        template<typename T>
        T pow(T v, T p)
        {
            return std::pow(v, p);
        }
        // Calculates the integer closest to a number.
        template<typename T>
        T round(T v)
        {
            return std::round(v);
        }
        // Calculates the square of a number.
        template<typename T>
        T sq(T v)
        {
            return std::pow(v, 2);
        }
        // Calculates the square root of a number.
        template<typename T>
        T sqrt(T v)
        {
            return std::sqrt(v);
        }

        // Noise
        // Calculates the absolute value of a number.
        double noise(double x);
        double noise(double x, double y);
        double noise(double x, double y, double z);
        // Calculates the absolute value of a number.
        void noise_detail(double frequency, int octaves);
        void noise_seed(int seed);
    }
}