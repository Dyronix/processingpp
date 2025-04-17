#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "trigonometry.h"
#include <cmath>

// Test the angle_mode setter and getter.
TEST_CASE("Angle mode getter and setter", "[trigonometry]")
{
    using ppp::angle_mode;
    using ppp::angle_mode_type;

    // Default should be RADIANS.
    REQUIRE(angle_mode() == angle_mode_type::RADIANS);

    // Change to DEGREES.
    angle_mode(angle_mode_type::DEGREES);
    REQUIRE(angle_mode() == angle_mode_type::DEGREES);

    // Reset back to RADIANS.
    angle_mode(angle_mode_type::RADIANS);
    REQUIRE(angle_mode() == angle_mode_type::RADIANS);
}

// Test the conversion functions for degrees and radians.
TEST_CASE("Degrees and Radians conversion", "[trigonometry]")
{
    using ppp::degrees;
    using ppp::radians;
    const double pi = 3.141592653589793;

    // Check that pi radians converts to approximately 180 degrees.
    REQUIRE(degrees(pi) == Catch::Approx(180.0).epsilon(1e-6));

    // Check that 180 degrees converts to approximately pi radians.
    REQUIRE(radians(180.0) == Catch::Approx(pi).epsilon(1e-6));
}

// Test the heading function for base_vec2.
TEST_CASE("Heading of a vector", "[trigonometry]")
{
    using ppp::heading;
    
    // For a vector pointing along the positive x-axis.
    ppp::vec2d v1(1.0, 0.0);
    REQUIRE(heading(v1) == Catch::Approx(0.0).epsilon(1e-6));

    // For a vector pointing along the positive y-axis.
    ppp::vec2d v2(0.0, 1.0);
    const double half_pi = 3.141592653589793 / 2;
    REQUIRE(heading(v2) == Catch::Approx(half_pi).epsilon(1e-6));
}

// Test the from_angle function.
TEST_CASE("Vector from angle", "[trigonometry]")
{
    using ppp::from_angle;
    
    // For an angle of 0 radians, should return a vector (1, 0).
    auto v0 = from_angle<double>(0.0);
    REQUIRE(v0.x == Catch::Approx(std::cos(0.0)).epsilon(1e-6));
    REQUIRE(v0.y == Catch::Approx(std::sin(0.0)).epsilon(1e-6));

    // For an angle of pi/4 radians, verify components.
    double angle = 3.141592653589793 / 4;
    auto v1 = from_angle<double>(angle);
    REQUIRE(v1.x == Catch::Approx(std::cos(angle)).epsilon(1e-6));
    REQUIRE(v1.y == Catch::Approx(std::sin(angle)).epsilon(1e-6));
    // The vector should be normalized.
    double mag = std::sqrt(v1.x * v1.x + v1.y * v1.y);
    REQUIRE(mag == Catch::Approx(1.0).epsilon(1e-6));
}

// Test the trigonometric wrapper functions.
TEST_CASE("Trigonometric wrapper functions", "[trigonometry]")
{
    using ppp::acos;
    using ppp::asin;
    using ppp::atan;
    using ppp::atan2;
    using ppp::cos;
    using ppp::sin;
    using ppp::tan;

    double val = 0.5;

    // Compare against the standard library functions.
    REQUIRE(acos(val) == Catch::Approx(std::acos(val)).epsilon(1e-6));
    REQUIRE(asin(val) == Catch::Approx(std::asin(val)).epsilon(1e-6));
    REQUIRE(atan(val) == Catch::Approx(std::atan(val)).epsilon(1e-6));

    // Test atan2 for a pair of values.
    REQUIRE(atan2(1.0, 1.0) == Catch::Approx(std::atan2(1.0, 1.0)).epsilon(1e-6));

    // Test cos, sin, and tan.
    double angle = 1.0; // in radians
    REQUIRE(cos(angle) == Catch::Approx(std::cos(angle)).epsilon(1e-6));
    REQUIRE(sin(angle) == Catch::Approx(std::sin(angle)).epsilon(1e-6));
    REQUIRE(tan(angle) == Catch::Approx(std::tan(angle)).epsilon(1e-6));
}
