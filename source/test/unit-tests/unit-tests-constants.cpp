#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "constants.h"
#include <cmath>

TEST_CASE("Constants functions return correct values", "[constants]") {
    // Expected values (approximated to eight decimal places)
    const float expected_pi = 3.14159265f;
    const float expected_half_pi = 1.57079633f;
    const float expected_quarter_pi = 0.78539816f;
    const float expected_two_pi = 6.28318531f;
    const float expected_tau = 6.28318531f; // Typically, tau is defined as 2*pi

    SECTION("pi() returns pi") {
        REQUIRE(ppp::pi() == Catch::Approx(expected_pi).epsilon(0.0001));
    }
    SECTION("half_pi() returns half of pi") {
        REQUIRE(ppp::half_pi() == Catch::Approx(expected_half_pi).epsilon(0.0001));
    }
    SECTION("quarter_pi() returns a quarter of pi") {
        REQUIRE(ppp::quarter_pi() == Catch::Approx(expected_quarter_pi).epsilon(0.0001));
    }
    SECTION("two_pi() returns two times pi") {
        REQUIRE(ppp::two_pi() == Catch::Approx(expected_two_pi).epsilon(0.0001));
    }
    SECTION("tau() returns tau (2*pi)") {
        REQUIRE(ppp::tau() == Catch::Approx(expected_tau).epsilon(0.0001));
    }
    SECTION("Mathematical relationships hold") {
        REQUIRE(ppp::half_pi() * 2 == Catch::Approx(ppp::pi()).epsilon(0.0001));
        REQUIRE(ppp::quarter_pi() * 4 == Catch::Approx(ppp::pi()).epsilon(0.0001));
        REQUIRE(ppp::two_pi() == Catch::Approx(ppp::pi() * 2).epsilon(0.0001));
        REQUIRE(ppp::tau() == Catch::Approx(ppp::two_pi()).epsilon(0.0001));
    }
}
