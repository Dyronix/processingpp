#pragma once

#include "constants.h"

#include <glm/glm.hpp>

#include <cmath>

namespace ppp
{
    namespace trigonometry
    {
        // The unit system we use to measure angles
        enum class angle_mode_type
        {
            DEGREES,
            RADIANS
        };

        // Changes the unit system used to measure angles.
        void angle_mode(angle_mode_type mode);

        // Converts an angle measured in radians to its value in degrees.
        template<typename T>
        inline T degrees(T rad)
        {
            return rad * static_cast<T>(57.295779513082320876798154814105f);
        }

        // Converts an angle measured in degrees to its value in radians.
        template<typename T>
        inline T radians(T deg)
        {
            return deg * static_cast<T>(0.01745329251994329576923690768489f);
        }

        // Calculates the arc cosine of a number.
        inline double acos(double x)
        {
            return std::acos(x);
        }
        // Calculates the arc sine of a number.
        inline double asin(double x)
        {
            return std::asin(x);
        }
        // Calculates the arc tangent of a number.
        inline double  atan(double x)
        {
            return std::atan(x);
        }
        // Calculates the angle formed by a point, the origin, and the positive x-axis.
        inline double  atan2(double y, double x)
        {
            return std::atan2(y, x);
        }
        // Calculates the cosine of an angle.
        inline double  cos(double x)
        {
            return std::cos(x);
        }
        // Calculates the sine of an angle.
        inline double  sin(double x)
        {
            return std::sin(x);
        }
        // Calculates the tangent of an angle.
        inline double  tan(double x)
        {
            return std::tan(x);
        }

        template<typename T>
        T heading(const glm::vec<2, T, glm::defaultp>& v)
        {
            return std::atan2(v.y, v.x);
        }

        template<typename T>
        glm::vec<2, T, glm::defaultp> from_angle(T angle)
        {
            T v_x = std::cos(angle);
            T v_y = std::sin(angle);

            return glm::normalize(glm::vec<2, T, glm::defaultp> { v_x, v_y });
        }

        template<typename T>
        glm::vec<2, T, glm::defaultp> random_2d()
        {
            float angle = ((rand() % 10) / 10.0f) * constants::two_pi();
            float length = 1.0f;

            T v_x = length * std::cos(angle);
            T v_y = length * std::sin(angle);

            return glm::vec<2, T, glm::defaultp> { v_x, v_y };
        }

        template<typename T>
        glm::vec<2, T, glm::defaultp> set_magnitude(const glm::vec<2, T, glm::defaultp>& v, T mag)
        {
            return glm::normalize(v) * mag;
        }

        template<typename T>
        T magnitude_squared(const glm::vec<2, T, glm::defaultp>& v)
        {
            const T x = v.x;
            const T y = v.y;
            return x * x + y * y;
        }

        template<typename T>
        glm::vec<2, T, glm::defaultp> limit(const glm::vec<2, T, glm::defaultp>& v, T max)
        {
            glm::vec<2, T, glm::defaultp> limit_v = v;

            const T mag_squared = magnitude_squared<T>(limit_v);
            if (mag_squared > max * max)
            {
                limit_v /= std::sqrt(mag_squared);
                limit_v *= max;
            }

            return limit_v;
        }
    }
}