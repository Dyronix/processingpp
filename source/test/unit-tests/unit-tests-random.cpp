#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "random.h"
#include <vector>
#include <array>
#include <cmath>
#include <limits>

// Helper function to check if a double is in [min, max)
static void require_in_range(double value, double min, double max)
{
    REQUIRE(value >= min);
    REQUIRE(value < max);
}

TEST_CASE("random(double min, double max) produces values in range and is reproducible", "[random]")
{
    // Set a known seed.
    ppp::random_seed(42);
    double val1 = ppp::random(5.0, 15.0);
    require_in_range(val1, 5.0, 15.0);

    // Reset the seed and get the value again.
    ppp::random_seed(42);
    double val2 = ppp::random(5.0, 15.0);
    REQUIRE(val1 == Catch::Approx(val2));
}

TEST_CASE("random() returns a value in [0,1) and is reproducible", "[random]")
{
    ppp::random_seed(1234);
    double r1 = ppp::random();
    require_in_range(r1, 0.0, 1.0);

    ppp::random_seed(1234);
    double r2 = ppp::random();
    REQUIRE(r1 == Catch::Approx(r2));
}

TEST_CASE("random(double max) returns a value in [0,max) and is reproducible", "[random]")
{
    ppp::random_seed(9876);
    double r1 = ppp::random(20.0);
    require_in_range(r1, 0.0, 20.0);

    ppp::random_seed(9876);
    double r2 = ppp::random(20.0);
    REQUIRE(r1 == Catch::Approx(r2));
}

TEST_CASE("random element from vector", "[random]")
{
    std::vector<int> vec = { 10, 20, 30, 40, 50 };
    ppp::random_seed(555);
    const int& elem1 = ppp::random(vec);
    // Check that the element is one of the expected values.
    REQUIRE(std::find(vec.begin(), vec.end(), elem1) != vec.end());

    // Reproducibility: resetting the seed returns the same element.
    ppp::random_seed(555);
    const int& elem2 = ppp::random(vec);
    REQUIRE(elem1 == elem2);
}

TEST_CASE("random element from array", "[random]")
{
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    ppp::random_seed(777);
    const int& elem1 = ppp::random(arr);
    // Check that the element is one of the expected values.
    REQUIRE(std::find(arr.begin(), arr.end(), elem1) != arr.end());

    ppp::random_seed(777);
    const int& elem2 = ppp::random(arr);
    REQUIRE(elem1 == elem2);
}

TEST_CASE("random_gaussian produces reproducible results", "[random]")
{
    // Using default mean = 0 and sd = 1.
    ppp::random_seed(42);
    double g1 = ppp::random_gaussian();
    REQUIRE(std::isfinite(g1));

    ppp::random_seed(42);
    double g2 = ppp::random_gaussian();
    REQUIRE(g1 == Catch::Approx(g2));

    // Check that altering mean and standard deviation shifts the output.
    ppp::random_seed(100);
    double g3 = ppp::random_gaussian(10.0, 2.0);
    REQUIRE(std::isfinite(g3));
    // Since we have fixed seed, g3 is reproducible.
    ppp::random_seed(100);
    double g4 = ppp::random_gaussian(10.0, 2.0);
    REQUIRE(g3 == Catch::Approx(g4));
}
