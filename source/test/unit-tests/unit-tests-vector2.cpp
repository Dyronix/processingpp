#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "vector.h"
#include "constants.h"
#include <cmath>
#include <sstream>

// Tests for base_vec2
TEST_CASE("base_vec2 operations", "[vec2]") 
{
    using ppp::base_vec2;
    base_vec2<double> v(3.0, 4.0);

    SECTION("to_string") 
    {
        std::string str = v.to_string();
        // Check that the string representation contains the component values.
        REQUIRE(str.find("3") != std::string::npos);
        REQUIRE(str.find("4") != std::string::npos);
    }

    SECTION("copy and set") 
    {
        auto v_copy = v.copy();
        REQUIRE(v_copy.x == 3.0);
        REQUIRE(v_copy.y == 4.0);
        v_copy.set(5.0, 6.0);
        REQUIRE(v_copy.x == 5.0);
        REQUIRE(v_copy.y == 6.0);
    }

    SECTION("addition and subtraction") 
    {
        base_vec2<double> v1(1.0, 2.0);
        base_vec2<double> v2(3.0, 4.0);
        auto v_add = v1;
        v_add.add(v2);
        REQUIRE(v_add.x == 4.0);
        REQUIRE(v_add.y == 6.0);

        auto v_sub = v2;
        v_sub.sub(v1);
        REQUIRE(v_sub.x == 2.0);
        REQUIRE(v_sub.y == 2.0);

        // Test operator overloads.
        auto v_plus = v1 + v2;
        REQUIRE(v_plus.x == 4.0);
        REQUIRE(v_plus.y == 6.0);
        auto v_minus = v2 - v1;
        REQUIRE(v_minus.x == 2.0);
        REQUIRE(v_minus.y == 2.0);
    }

    SECTION("multiplication and division by scalar") 
    {
        base_vec2<double> v1(2.0, 3.0);
        v1.mult(2.0);
        REQUIRE(v1.x == 4.0);
        REQUIRE(v1.y == 6.0);
        v1.div(2.0);
        REQUIRE(v1.x == 2.0);
        REQUIRE(v1.y == 3.0);
    }

    SECTION("remainder operations") 
    {
        base_vec2<double> v1(5.5, 7.2);
        v1.rem(3.0);
        REQUIRE(v1.x == Catch::Approx(std::fmod(5.5, 3.0)));
        REQUIRE(v1.y == Catch::Approx(std::fmod(7.2, 3.0)));

        base_vec2<double> v2(5.5, 7.2);
        base_vec2<double> mod(3.0, 3.0);
        v2.rem(mod);
        REQUIRE(v2.x == Catch::Approx(std::fmod(5.5, 3.0)));
        REQUIRE(v2.y == Catch::Approx(std::fmod(7.2, 3.0)));
    }

    SECTION("magnitude and normalization") 
    {
        base_vec2<double> v1(3.0, 4.0);
        double m = v1.mag();
        REQUIRE(m == Catch::Approx(5.0));
        v1.normalize();
        REQUIRE(v1.mag() == Catch::Approx(1.0));
    }

    SECTION("dot product") 
    {
        base_vec2<double> v1(1.0, 2.0);
        base_vec2<double> v2(3.0, 4.0);
        double d = v1.dot(v2);
        REQUIRE(d == Catch::Approx(11.0));
    }

    SECTION("limit and set magnitude") 
    {
        base_vec2<double> v1(3.0, 4.0); // magnitude 5.0
        v1.limit(3.0);
        REQUIRE(v1.mag() == Catch::Approx(3.0));

        base_vec2<double> v2(3.0, 4.0);
        v2.set_mag(10.0);
        REQUIRE(v2.mag() == Catch::Approx(10.0));
    }

    SECTION("heading and set_heading") 
    {
        base_vec2<double> v1(1.0, 0.0);
        double h = v1.heading();
        REQUIRE(h == Catch::Approx(0.0));
        v1.set_heading(ppp::pi() / 2);
        // After setting heading to 90 degrees, x should be nearly 0 and y equal to the original magnitude.
        REQUIRE(v1.x == Catch::Approx(0.0).margin(1e-6));
        REQUIRE(v1.y == Catch::Approx(1.0).epsilon(1e-6));
    }
}
