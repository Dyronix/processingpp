#pragma once

#include <cmath>

namespace ppp
{
    namespace trigonometry
    {
        template<typename T>
        T acos(T v)
        {
            return std::acos(v);
        }
        template<typename T>
        T asin(T v)
        {
            return std::asin(v);
        }
        template<typename T>
        T atan(T v)
        {
            return std::atan(v);
        }
        template<typename T>
        T atan2(T v)
        {
            return std::atan2(v);
        }
        template<typename T>
        T cos(T v)
        {
            return std::cos(v);
        }
        template<typename T>
        T sin(T v)
        {
            return std::sin(v);
        }
        template<typename T>
        T tan(T v)
        {
            return std::tan(v);
        }
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
    }
}