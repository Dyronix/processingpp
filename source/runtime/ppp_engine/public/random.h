#pragma once

#include <vector>
#include <array>
#include <type_traits>
#include <assert.h>

namespace ppp
{
    namespace random
    {
        template<typename T>
        T random_value(T min, T max)
        {
            static_assert(std::is_integral_v<T>);

            return (rand() % (max - min)) + min;
        }

        template<typename T>
        const T& random(const std::vector<T>& vec)
        {
            auto max = vec.size();

            auto r = random_value(static_cast<decltype(max)>(0), max);
            
            return vec[r];
        }

        template<typename T, size_t size>
        const T& random(const std::array<T, size>& arr)
        {
            auto r = random_value(0u, size);

            return arr[r];
        }
    }
}