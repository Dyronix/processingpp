#include "mathematics.h"
#include "util/perlin_noise.h"

namespace ppp
{
    namespace math
    {
        double noise(double x)
        {
            return noise::perlin_noise(x);
        }

        double noise(double x, double y)
        {
            return noise::perlin_noise(x, y);
        }

        double noise(double x, double y, double z)
        {
            return noise::perlin_noise(x, y, z);
        }

        void noise_detail(double frequency, int octaves)
        {
            noise::perlin_noise(frequency, octaves);
        }

        void noise_seed(int seed)
        {
            noise::perlin_noise(seed);
        }
    }
}