#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "vector.h"
#include <cmath>
#include <sstream>

// Tests for base_vec4
TEST_CASE("base_vec4 operations", "[vec4]") 
{
    using ppp::base_vec4;
    base_vec4<double> v(1.0, 2.0, 3.0, 4.0);

    SECTION("to_string") 
    {
        std::string str = v.to_string();
        REQUIRE(str.find("1") != std::string::npos);
        REQUIRE(str.find("2") != std::string::npos);
        REQUIRE(str.find("3") != std::string::npos);
        REQUIRE(str.find("4") != std::string::npos);
    }

    SECTION("copy and set") 
    {
        auto v_copy = v.copy();
        REQUIRE(v_copy.x == 1.0);
        REQUIRE(v_copy.y == 2.0);
        REQUIRE(v_copy.z == 3.0);
        REQUIRE(v_copy.w == 4.0);
        v_copy.set(5.0, 6.0, 7.0, 8.0);
        REQUIRE(v_copy.x == 5.0);
        REQUIRE(v_copy.y == 6.0);
        REQUIRE(v_copy.z == 7.0);
        REQUIRE(v_copy.w == 8.0);
    }

    SECTION("addition and subtraction") 
    {
        base_vec4<double> v1(1.0, 2.0, 3.0, 4.0);
        base_vec4<double> v2(4.0, 3.0, 2.0, 1.0);
        auto v_add = v1;
        v_add.add(v2);
        REQUIRE(v_add.x == 5.0);
        REQUIRE(v_add.y == 5.0);
        REQUIRE(v_add.z == 5.0);
        REQUIRE(v_add.w == 5.0);

        auto v_sub = v1;
        v_sub.sub(v2);
        REQUIRE(v_sub.x == -3.0);
        REQUIRE(v_sub.y == -1.0);
        REQUIRE(v_sub.z == 1.0);
        REQUIRE(v_sub.w == 3.0);
    }

    SECTION("multiplication and division by scalar") 
    {
        base_vec4<double> v1(2.0, 3.0, 4.0, 5.0);
        v1.mult(2.0);
        REQUIRE(v1.x == 4.0);
        REQUIRE(v1.y == 6.0);
        REQUIRE(v1.z == 8.0);
        REQUIRE(v1.w == 10.0);
        v1.div(2.0);
        REQUIRE(v1.x == 2.0);
        REQUIRE(v1.y == 3.0);
        REQUIRE(v1.z == 4.0);
        REQUIRE(v1.w == 5.0);
    }

    SECTION("remainder operations") 
    {
        base_vec4<double> v1(5.5, 7.2, 9.3, 11.4);
        v1.rem(3.0);
        REQUIRE(v1.x == Catch::Approx(std::fmod(5.5, 3.0)));
        REQUIRE(v1.y == Catch::Approx(std::fmod(7.2, 3.0)));
        REQUIRE(v1.z == Catch::Approx(std::fmod(9.3, 3.0)));
        REQUIRE(v1.w == Catch::Approx(std::fmod(11.4, 3.0)));

        base_vec4<double> v2(5.5, 7.2, 9.3, 11.4);
        base_vec4<double> mod(3.0, 3.0, 3.0, 3.0);
        v2.rem(mod);
        REQUIRE(v2.x == Catch::Approx(std::fmod(5.5, 3.0)));
        REQUIRE(v2.y == Catch::Approx(std::fmod(7.2, 3.0)));
        REQUIRE(v2.z == Catch::Approx(std::fmod(9.3, 3.0)));
        REQUIRE(v2.w == Catch::Approx(std::fmod(11.4, 3.0)));
    }

    SECTION("magnitude and normalization") 
    {
        base_vec4<double> v1(2.0, 3.0, 6.0, 2.0);
        double expected = std::sqrt(4 + 9 + 36 + 4);
        REQUIRE(v1.mag() == Catch::Approx(expected));
        v1.normalize();
        REQUIRE(v1.mag() == Catch::Approx(1.0));
    }

    SECTION("dot product") 
    {
        base_vec4<double> v1(1.0, 2.0, 3.0, 4.0);
        base_vec4<double> v2(4.0, 3.0, 2.0, 1.0);
        double d = v1.dot(v2);
        REQUIRE(d == Catch::Approx(1 * 4 + 2 * 3 + 3 * 2 + 4 * 1));
    }

    SECTION("limit and set magnitude") 
    {
        base_vec4<double> v1(1.0, 2.0, 2.0, 1.0); // magnitude = sqrt(1+4+4+1) = sqrt(10)
        v1.limit(2.0);
        REQUIRE(v1.mag() <= 2.0);

        base_vec4<double> v2(1.0, 2.0, 2.0, 1.0);
        v2.set_mag(5.0);
        REQUIRE(v2.mag() == Catch::Approx(5.0));
    }
}