#pragma once

#include <vector>
#include <array>
#include <type_traits>
#include <cmath>
#include <assert.h>

namespace ppp
{
    namespace random
    {
        // Sets the seed value for the random() and randomGaussian() functions
        void random_seed(unsigned int s)
        {
            std::srand(s);
        }

        // Returns a random double in [min, max]
        double random(double min, double max)
        {
            return (static_cast<double>(std::rand()) / (RAND_MAX + 1.0)) * (max - min) + min;
        }

        // Returns a random double in the interval [0, 1]
        double random()
        {
            return random(0.0, 1.0);
        }

        // Returns a random double in the interval [0, max]
        double random(double max)
        {
            return random(0.0, max);
        }

        // Returns a random double in the interval [min, max]
        double random(double min, double max)
        {
            if (min > max)
            {
                std::swap(min, max);
            }

            return random(min, max);
        }

        // Returns a random element from an array.
        template<typename T>
        const T& random(const std::vector<T>& vec)
        {
            auto max = vec.size();

            auto r = random(static_cast<decltype(max)>(0), max);

            return vec[(int)r];
        }

        // Returns a random element from an array.
        template<typename T, size_t size>
        const T& random(const std::array<T, size>& arr)
        {
            auto r = random(0u, size);

            return arr[(int)r];
        }

        // Returns a random number from a Gaussian (normal) distribution using the Box–Muller transform.
        double random_gaussian(double mean = 0.0, double sd = 1.0) 
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
}