#pragma once

namespace ppp
{
    namespace noise
    {
        double perlin_noise(double x);
        double perlin_noise(double x, double y);
        double perlin_noise(double x, double y, double z);

        void perlin_noise_detail(double frequency, int octaves);
        void perlin_noise_seed(int seed);
    }
}