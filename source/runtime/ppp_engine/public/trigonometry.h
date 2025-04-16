#pragma once

#include "constants.h"

#include "vector.h"

#include <cmath>

namespace ppp
{
    // The unit system we use to measure angles
    enum class angle_mode_type
    {
        DEGREES,
        RADIANS
    };

    // Changes the unit system used to measure angles.
    void angle_mode(angle_mode_type mode);

    // Retrieve the unit system used to measure angles.
    angle_mode_type angle_mode();

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

    template<typename T>
    T heading(const base_vec2<T>& v)
    {
        return std::atan2(v.y, v.x);
    }

    template<typename T>
    base_vec2<T> from_angle(T angle)
    {
        T v_x = std::cos(angle);
        T v_y = std::sin(angle);

        auto v = base_vec2<T>{ v_x, v_y };

        v.normalize();

        return v;
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
}