/**
 * @file random.h
 * @brief Random number generation utilities.
 */
#pragma once

#include <vector>
#include <array>
#include <type_traits>
#include <cmath>
#include <assert.h>

namespace ppp
{
    /**
     * @brief Sets the seed for random number generation.
     * @param s Seed value.
     */
    inline void random_seed(unsigned int s)
    {
        std::srand(s);
    }

    /**
     * @brief Returns a random double in the range [min, max].
     * @param min Lower bound.
     * @param max Upper bound.
     * @return Random double in [min, max].
     */
    inline double random(double min, double max)
    {
        return (static_cast<double>(std::rand()) / (RAND_MAX + 1.0)) * (max - min) + min;
    }

    /**
     * @brief Returns a random double in the range [0, 1].
     * @return Random double in [0, 1].
     */
    inline double random()
    {
        return random(0.0, 1.0);
    }

    /**
     * @brief Returns a random double in the range [0, max].
     * @param max Upper bound.
     * @return Random double in [0, max].
     */
    inline double random(double max)
    {
        return random(0.0, max);
    }

    /**
     * @brief Returns a random element from a std::vector.
     * @tparam T Element type.
     * @param vec Input vector.
     * @return Random element from vec.
     */
    template<typename T>
    const T& random(const std::vector<T>& vec)
    {
        auto max = vec.size();
        auto r = random(static_cast<decltype(max)>(0), max);
        return vec[(int)r];
    }

    /**
     * @brief Returns a random element from a std::array.
     * @tparam T Element type.
     * @tparam size Array size.
     * @param arr Input array.
     * @return Random element from arr.
     */
    template<typename T, size_t size>
    const T& random(const std::array<T, size>& arr)
    {
        auto r = random(0u, size);
        return arr[(int)r];
    }

    /**
     * @brief Generates a random number from a Gaussian distribution.
     * @param mean Distribution mean (default 0.0).
     * @param sd Distribution standard deviation (default 1.0).
     * @return Random Gaussian value.
     */
    inline double random_gaussian(double mean = 0.0, double sd = 1.0)
    {
        double u, v, s;
        do
        {
            u = random(-1.0, 1.0);
            v = random(-1.0, 1.0);
            s = u * u + v * v;
        } while (s >= 1.0 || s == 0.0);

        double multiplier = std::sqrt(-2.0 * std::log(s) / s);
        return u * multiplier * sd + mean;
    }
}