/**
 * @file trigonometry.h
 * @brief Trigonometric functions and angle utilities.
 */
#pragma once

#include "vector.h"

#include <glm/geometric.hpp>

#include <cmath>

namespace ppp
{
    /**
     * @brief Units used for angular measurements.
     */
    enum class angle_mode_type
    {
        DEGREES, /**< Degrees mode */
        RADIANS  /**< Radians mode */
    };

    /**
     * @brief Sets the unit system for angle measurements.
     * @param mode Desired angle mode (DEGREES or RADIANS).
     */
    void angle_mode(angle_mode_type mode);

    /**
     * @brief Retrieves the current angle measurement mode.
     * @return Current angle mode.
     */
    angle_mode_type angle_mode();

    /**
     * @brief Converts radians to degrees.
     * @tparam T Numeric type of the angle.
     * @param rad Angle in radians.
     * @return Equivalent angle in degrees.
     */
    template<typename T>
    T degrees(T rad)
    {
        return rad * static_cast<T>(57.295779513082320876798154814105f);
    }

    /**
     * @brief Converts degrees to radians.
     * @tparam T Numeric type of the angle.
     * @param deg Angle in degrees.
     * @return Equivalent angle in radians.
     */
    template<typename T>
    T radians(T deg)
    {
        return deg * static_cast<T>(0.01745329251994329576923690768489f);
    }

    /**
     * @brief Computes the heading (angle) of a 2D vector.
     * @tparam T Numeric type of the vector components.
     * @param v Input 2D vector.
     * @return Angle in radians (or degrees, per current mode).
     */
    template<typename T>
    T heading(const glm::vec<2, T, glm::defaultp>& v)
    {
        return std::atan2(v.y, v.x);
    }

    /**
     * @brief Constructs a unit 2D vector from an angle.
     * @tparam T Numeric type of the angle.
     * @param angle Input angle.
     * @return Unit vector pointing at the given angle.
     */
    template<typename T>
    glm::vec<2, T, glm::defaultp> from_angle(T angle)
    {
        T v_x = std::cos(angle);
        T v_y = std::sin(angle);
        auto v = glm::vec<2, T, glm::defaultp>{ v_x, v_y };
        return glm::normalize(v);
    }

    /**
     * @brief Calculates the arc cosine of a number.
     * @param x Input value.
     * @return acos(x).
     */
    inline double acos(double x)
    {
        return std::acos(x);
    }

    /**
     * @brief Calculates the arc sine of a number.
     * @param x Input value.
     * @return asin(x).
     */
    inline double asin(double x)
    {
        return std::asin(x);
    }

    /**
     * @brief Calculates the arc tangent of a number.
     * @param x Input value.
     * @return atan(x).
     */
    inline double atan(double x)
    {
        return std::atan(x);
    }

    /**
     * @brief Calculates the angle from origin to (x,y) relative to the positive x-axis.
     * @param y Y-coordinate.
     * @param x X-coordinate.
     * @return atan2(y, x).
     */
    inline double atan2(double y, double x)
    {
        return std::atan2(y, x);
    }

    /**
     * @brief Calculates the cosine of an angle.
     * @param x Angle value.
     * @return cos(x).
     */
    inline double cos(double x)
    {
        return std::cos(x);
    }

    /**
     * @brief Calculates the sine of an angle.
     * @param x Angle value.
     * @return sin(x).
     */
    inline double sin(double x)
    {
        return std::sin(x);
    }

    /**
     * @brief Calculates the tangent of an angle.
     * @param x Angle value.
     * @return tan(x).
     */
    inline double tan(double x)
    {
        return std::tan(x);
    }
}
