#pragma once

#include <cmath>

namespace ppp
{
    namespace math
    {
        template<typename T>
        T abs(T v)
        {
            return std::abs(v);
        }
        template<typename T>
        T ceil(T v)
        {
            return std::ceil(v);
        }
        template<typename T>
        T constrain(T v, T min, T max)
        {
            return std::clamp(v, min, max);
        }
        template<typename T>
        T exp(T v)
        {
            return std::exp(v);
        }
        template<typename T>
        T floor(T v)
        {
            return std::floor(v);
        }
        template<typename T>
        T lerp(T from, T to, T t)
        {
            return a + t * (b - a);
        }
        template<typename T>
        T log(T v)
        {
            return std::log(v);
        }
        template <typename T>
        T map(T value, T in_min, T in_max, T out_min, T out_max) 
        {
            return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);
        }
        template<typename T>
        T max(T v, T other)
        {
            return std:max(v, other);
        }
        template<typename T>
        T min(T v, T other)
        {
            return std:min(v, other);
        }
        template<typename T>
        T pow(T v, T p)
        {
            return std::pow(v, p);
        }
        template<typename T>
        T round(T v)
        {
            return std::round(v);
        }
        template<typename T>
        T sq(T v)
        {
            return std::pow(v, 2);
        }
        template<typename T>
        T sqrt(T v)
        {
            return std::sqrt(v);
        }
        template<typename T>
        T fract(T v)
        {
            return std::fract(v);
        }

        double noise(double x);
        double noise(double x, double y);
        double noise(double x, double y, double z);

        void noise_detail(double frequency, int octaves);
        void noise_seed(int seed);
    }
}