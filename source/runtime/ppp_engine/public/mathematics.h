/**
 * @file mathematics.h
 * @brief Common mathematical functions and noise utilities.
 */
#pragma once

#include "vector.h"

#include <algorithm>
#include <cmath>

namespace ppp
{
    namespace math
    {
        /**
         * @brief Calculates the absolute value of a number.
         * @tparam T Numeric type of the input value.
         * @param v Input value.
         * @return Absolute value of v.
         */
        template<typename T>
        T abs(T v)
        {
            return std::abs(v);
        }

        /**
         * @brief Calculates the smallest integer greater than or equal to a number.
         * @tparam T Numeric type of the input value.
         * @param v Input value.
         * @return Ceiling of v.
         */
        template<typename T>
        T ceil(T v)
        {
            return std::ceil(v);
        }

        /**
         * @brief Constrains a number between a minimum and maximum value.
         * @tparam T Numeric type of the input and bounds.
         * @param v Input value.
         * @param min Lower bound.
         * @param max Upper bound.
         * @return Clamped value between min and max.
         */
        template<typename T>
        T constrain(T v, T min, T max)
        {
            return std::clamp(v, min, max);
        }

        /**
         * @brief Calculates the distance between two 2D points specified by coordinates.
         * @tparam T Numeric type of the coordinates.
         * @param x1 X-coordinate of the first point.
         * @param y1 Y-coordinate of the first point.
         * @param x2 X-coordinate of the second point.
         * @param y2 Y-coordinate of the second point.
         * @return Euclidean distance between the two points.
         */
        template<typename T>
        T dist(T x1, T y1, T x2, T y2)
        {
            return std::hypot(x1 - x2, y1 - y2);
        }

        /**
         * @brief Calculates the distance between two 2D points.
         * @tparam T Numeric type of the vector components.
         * @param v1 First point as a glm::vec2.
         * @param v2 Second point as a glm::vec2.
         * @return Euclidean distance between v1 and v2.
         */
        template<typename T>
        T dist(const glm::vec<2, T, glm::defaultp>& v1, const glm::vec<2, T, glm::defaultp>& v2)
        {
            return std::hypot(v1.x - v2.x, v1.y - v2.y);
        }

        /**
         * @brief Calculates the distance between two 3D points specified by coordinates.
         * @tparam T Numeric type of the coordinates.
         * @param x1 X-coordinate of the first point.
         * @param y1 Y-coordinate of the first point.
         * @param z1 Z-coordinate of the first point.
         * @param x2 X-coordinate of the second point.
         * @param y2 Y-coordinate of the second point.
         * @param z2 Z-coordinate of the second point.
         * @return Euclidean distance between the two points.
         */
        template<typename T>
        T dist(T x1, T y1, T z1, T x2, T y2, T z2)
        {
            return std::hypot(x1 - x2, y1 - y2, z1 - z2);
        }

        /**
         * @brief Calculates the distance between two 3D points.
         * @tparam T Numeric type of the vector components.
         * @param v1 First point as a glm::vec3.
         * @param v2 Second point as a glm::vec3.
         * @return Euclidean distance between v1 and v2.
         */
        template<typename T>
        T dist(const glm::vec<3, T, glm::defaultp>& v1, const glm::vec<3, T, glm::defaultp>& v2)
        {
            return std::hypot(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
        }

        /**
         * @brief Calculates e raised to the power of a number.
         * @tparam T Numeric type of the input value.
         * @param v Exponent value.
         * @return e^v.
         */
        template<typename T>
        T exp(T v)
        {
            return std::exp(v);
        }

        /**
         * @brief Calculates the largest integer less than or equal to a number.
         * @tparam T Numeric type of the input value.
         * @param v Input value.
         * @return Floor of v.
         */
        template<typename T>
        T floor(T v)
        {
            return std::floor(v);
        }

        /**
         * @brief Decomposes a floating-point number into its integer and fractional parts.
         * @tparam T Numeric type of the input value.
         * @param v Input value.
         * @param iptr Pointer to store integer part.
         * @return Fractional part of v.
         */
        template<typename T>
        T fract(T v, T* iptr)
        {
            return std::modf(v, iptr);
        }

        /**
         * @brief Linearly interpolates between two values.
         * @tparam T Numeric type of the parameters.
         * @param from Starting value.
         * @param to Ending value.
         * @param t Interpolation factor in [0,1].
         * @return Interpolated value.
         */
        template<typename T>
        T lerp(T from, T to, T t)
        {
            return from + t * (to - from);
        }

        /**
         * @brief Calculates the natural logarithm (base e).
         * @tparam T Numeric type of the input value.
         * @param v Input value.
         * @return Natural logarithm of v.
         */
        template<typename T>
        T log(T v)
        {
            return std::log(v);
        }

        /**
         * @brief Calculates the magnitude of a 2D vector.
         * @tparam T Numeric type of the components.
         * @param x X-component.
         * @param y Y-component.
         * @return Length of the vector.
         */
        template<typename T>
        T mag(T x, T y)
        {
            return std::hypot(x, y);
        }

        /**
         * @brief Calculates the magnitude of a 3D vector.
         * @tparam T Numeric type of the components.
         * @param x X-component.
         * @param y Y-component.
         * @param z Z-component.
         * @return Length of the vector.
         */
        template<typename T>
        T mag(T x, T y, T z)
        {
            return std::hypot(x, y, z);
        }

        /**
         * @brief Re-maps a number from one range to another.
         * @tparam T Numeric type of the parameters.
         * @param value Input value.
         * @param in_min Input lower bound.
         * @param in_max Input upper bound.
         * @param out_min Output lower bound.
         * @param out_max Output upper bound.
         * @return Mapped value in [out_min,out_max].
         */
        template <typename T>
        T map(T value, T in_min, T in_max, T out_min, T out_max)
        {
            return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
        }

        /**
         * @brief Returns the larger of two values.
         * @tparam T Numeric type of the parameters.
         * @param v First value.
         * @param other Second value.
         * @return Maximum of v and other.
         */
        template<typename T>
        T max(T v, T other)
        {
            return (std::max)(v, other);
        }

        /**
         * @brief Returns the smaller of two values.
         * @tparam T Numeric type of the parameters.
         * @param v First value.
         * @param other Second value.
         * @return Minimum of v and other.
         */
        template<typename T>
        T min(T v, T other)
        {
            return (std::min)(v, other);
        }

        /**
         * @brief Normalizes a value within a range to [0,1].
         * @tparam T Numeric type of the parameters.
         * @param n Input value.
         * @param start Range start.
         * @param end Range end.
         * @return Normalized value.
         */
        template<typename T>
        T norm(T n, T start, T end)
        {
            return map(n, start, end, static_cast<T>(0), static_cast<T>(1));
        }

        /**
         * @brief Calculates a value raised to an exponent.
         * @tparam T Numeric type of the parameters.
         * @param v Base value.
         * @param p Exponent.
         * @return v^p.
         */
        template<typename T>
        T pow(T v, T p)
        {
            return std::pow(v, p);
        }

        /**
         * @brief Rounds a value to the nearest integer.
         * @tparam T Numeric type of the input.
         * @param v Input value.
         * @return Rounded integer.
         */
        template<typename T>
        T round(T v)
        {
            return std::round(v);
        }

        /**
         * @brief Calculates the square of a number.
         * @tparam T Numeric type of the input.
         * @param v Input value.
         * @return v squared.
         */
        template<typename T>
        T sq(T v)
        {
            return std::pow(v, 2);
        }

        /**
         * @brief Calculates the square root of a number.
         * @tparam T Numeric type of the input.
         * @param v Input value.
         * @return Square root of v.
         */
        template<typename T>
        T sqrt(T v)
        {
            return std::sqrt(v);
        }

        /**
         * @brief Generates Perlin noise at a given 1D coordinate.
         * @param x Input coordinate.
         * @return Noise value.
         */
        double noise(double x);

        /**
         * @brief Generates Perlin noise at a given 2D coordinate.
         * @param x X-coordinate.
         * @param y Y-coordinate.
         * @return Noise value.
         */
        double noise(double x, double y);

        /**
         * @brief Generates Perlin noise at a given 3D coordinate.
         * @param x X-coordinate.
         * @param y Y-coordinate.
         * @param z Z-coordinate.
         * @return Noise value.
         */
        double noise(double x, double y, double z);

        /**
         * @brief Sets the frequency and octaves for Perlin noise generation.
         * @param frequency Base frequency.
         * @param octaves Number of octaves.
         */
        void noise_detail(double frequency, int octaves);

        /**
         * @brief Seeds the Perlin noise generator.
         * @param seed Seed value.
         */
        void noise_seed(int seed);
    }
}