#pragma once

#include "constants.h"

#include <glm/glm.hpp>

#include <cmath>

namespace ppp
{
    namespace trigonometry
    {
        enum class angle_mode_type
        {
            DEGREES,
            RADIANS
        };

        void angle_mode(angle_mode_type mode);

        angle_mode_type angle_mode();

        template<typename T>
        T degrees(T rad)
        {
            return rad * static_cast<T>(57.295779513082320876798154814105f);
        }
        template<typename T>
        T radians(T deg)
        {
            return deg * static_cast<T>(0.01745329251994329576923690768489f);
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