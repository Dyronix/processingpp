#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "mathematics.h"
#include "vector.h"   // Required because some functions use base_vec2 and base_vec3
#include <cmath>
#include <limits>

TEST_CASE("math::abs", "[math]")
{
    using ppp::math::abs;
    REQUIRE(abs(-5) == 5);
    REQUIRE(abs(5) == 5);
    REQUIRE(abs(-3.14) == Catch::Approx(3.14));
}

TEST_CASE("math::ceil", "[math]")
{
    using ppp::math::ceil;
    REQUIRE(ceil(3.14) == 4);
    REQUIRE(ceil(-3.14) == -3);
}

TEST_CASE("math::constrain", "[math]")
{
    using ppp::math::constrain;
    // within range
    REQUIRE(constrain(5, 1, 10) == 5);
    // below minimum
    REQUIRE(constrain(0, 1, 10) == 1);
    // above maximum
    REQUIRE(constrain(15, 1, 10) == 10);
}

TEST_CASE("math::dist for 2D points", "[math]")
{
    using ppp::math::dist;
    // Using individual coordinates: distance from (0,0) to (3,4) should be 5.
    REQUIRE(dist(0.0, 0.0, 3.0, 4.0) == Catch::Approx(5.0));

    // Using base_vec2 overload.
    ppp::base_vec2<double> v1(0.0, 0.0);
    ppp::base_vec2<double> v2(3.0, 4.0);
    REQUIRE(dist(v1, v2) == Catch::Approx(5.0));
}

TEST_CASE("math::dist for 3D points", "[math]")
{
    using ppp::math::dist;
    // Using individual coordinates: from (0,0,0) to (1,2,2) distance should be 3.
    REQUIRE(dist(0.0, 0.0, 0.0, 1.0, 2.0, 2.0) == Catch::Approx(3.0));

    // Using base_vec3 overload.
    ppp::base_vec3<double> v1(0.0, 0.0, 0.0);
    ppp::base_vec3<double> v2(1.0, 2.0, 2.0);
    REQUIRE(dist(v1, v2) == Catch::Approx(3.0));
}

TEST_CASE("math::exp", "[math]")
{
    using ppp::math::exp;
    REQUIRE(exp(1.0) == Catch::Approx(std::exp(1.0)));
}

TEST_CASE("math::floor", "[math]")
{
    using ppp::math::floor;
    REQUIRE(floor(3.7) == 3);
    REQUIRE(floor(-3.7) == -4);
}

TEST_CASE("fract returns correct fractional and integer parts for positive numbers", "[fract]")
{
    using ppp::math::fract;

    float value = 3.14f;
    float intPart = 0.0f;
    float fracPart = fract(value, &intPart);

    // For positive numbers, modf truncates toward zero.
    // 3.14 = 3.0 + 0.14 (approximately)
    REQUIRE(intPart == Catch::Approx(3.0f));
    REQUIRE(fracPart == Catch::Approx(0.14f).epsilon(0.01f));
}

TEST_CASE("fract returns correct fractional and integer parts for negative numbers", "[fract]")
{
    using ppp::math::fract;

    double value = -2.71828;
    double intPart = 0.0;
    double fracPart = fract(value, &intPart);

    // For negative values, modf truncates toward zero.
    // -2.71828 = (-2.0) + (-0.71828)
    REQUIRE(intPart == Catch::Approx(-2.0));
    REQUIRE(fracPart == Catch::Approx(-0.71828).epsilon(0.0001));
}

TEST_CASE("fract returns zero fractional part for integer input", "[fract]")
{
    using ppp::math::fract;

    float value = 5.0f;
    float intPart = 0.0f;
    float fracPart = fract(value, &intPart);

    // For an integer input, the fractional part should be zero.
    REQUIRE(intPart == Catch::Approx(5.0f));
    REQUIRE(fracPart == Catch::Approx(0.0f));
}

TEST_CASE("math::lerp", "[math]")
{
    using ppp::math::lerp;
    // Interpolating between 0 and 10 with t = 0.5 should yield 5.
    REQUIRE(lerp(0.0, 10.0, 0.5) == Catch::Approx(5.0));
}

TEST_CASE("math::log", "[math]")
{
    using ppp::math::log;
    // log(exp(1)) should be approximately 1.
    REQUIRE(log(std::exp(1.0)) == Catch::Approx(1.0));
}

TEST_CASE("math::mag", "[math]")
{
    using ppp::math::mag;
    // For 2D: mag(3,4) should be 5.
    REQUIRE(mag(3.0, 4.0) == Catch::Approx(5.0));
    // For 3D: mag(1,2,2) should be 3.
    REQUIRE(mag(1.0, 2.0, 2.0) == Catch::Approx(3.0));
}

TEST_CASE("math::map", "[math]")
{
    using ppp::math::map;
    // Mapping 5 from [0,10] to [0,100] should yield 50.
    REQUIRE(map(5.0, 0.0, 10.0, 0.0, 100.0) == Catch::Approx(50.0));
}

TEST_CASE("math::max and math::min", "[math]")
{
    using ppp::math::max;
    using ppp::math::min;
    REQUIRE(max(3, 5) == 5);
    REQUIRE(max(10, -2) == 10);
    REQUIRE(min(3, 5) == 3);
    REQUIRE(min(-10, -2) == -10);
}

TEST_CASE("math::norm", "[math]")
{
    using ppp::math::norm;
    // Normalizing 5 in range [0,10] should yield 0.5.
    REQUIRE(norm(5.0, 0.0, 10.0) == Catch::Approx(0.5));
}

TEST_CASE("math::pow", "[math]")
{
    using ppp::math::pow;
    REQUIRE(pow(2.0, 3.0) == Catch::Approx(8.0));
}

TEST_CASE("math::round", "[math]")
{
    using ppp::math::round;
    REQUIRE(round(3.6) == 4);
    REQUIRE(round(3.4) == 3);
}

TEST_CASE("math::sq", "[math]")
{
    using ppp::math::sq;
    REQUIRE(sq(3.0) == Catch::Approx(9.0));
}

TEST_CASE("math::sqrt", "[math]")
{
    using ppp::math::sqrt;
    REQUIRE(sqrt(9.0) == Catch::Approx(3.0));
}

TEST_CASE("Noise functions", "[math][noise]")
{
    using ppp::math::noise;
    using ppp::math::noise_detail;
    using ppp::math::noise_seed;

    // Call noise functions and verify that they return finite values.
    double n1 = noise(0.5);
    double n2 = noise(0.5, 0.5);
    double n3 = noise(0.5, 0.5, 0.5);
    REQUIRE(std::isfinite(n1));
    REQUIRE(std::isfinite(n2));
    REQUIRE(std::isfinite(n3));

    // Call noise_detail and noise_seed to ensure they execute without error.
    noise_detail(0.5, 3);
    noise_seed(42);

    // Check determinism (if the noise implementation is deterministic).
    double n4 = noise(0.5);
    noise_seed(42);
    double n5 = noise(0.5);
    REQUIRE(n4 == Catch::Approx(n5));
}
