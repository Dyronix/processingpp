#include "util/perlin_noise.h"

#include <noise/perlin_noise.h>

#include <algorithm>

namespace ppp
{
    namespace noise
    {
        namespace internal
        {
            siv::PerlinNoise _perlin;

            double _frequency = 0.1f;
            int _octaves = 1;
        }

        double perlin_noise(double x)
        {
            return internal::_perlin.octave1D_01(x * internal::_frequency, internal::_octaves);
        }

        double perlin_noise(double x, double y)
        {
            return internal::_perlin.octave2D_01(x * internal::_frequency, y * internal::_frequency, internal::_octaves);
        }

        double perlin_noise(double x, double y, double z)
        {
            return internal::_perlin.octave3D_01(x * internal::_frequency, y * internal::_frequency, z * internal::_frequency, internal::_octaves);
        }

        void perlin_noise_detail(double frequency, int octaves)
        {
            internal::_frequency = std::clamp(frequency, 0.1, 64.0);
            internal::_octaves = std::clamp(octaves, 1, 16);
        }

        void perlin_noise_seed(int seed)
        {
            internal::_perlin = siv::PerlinNoise(seed);
        }
    }
}