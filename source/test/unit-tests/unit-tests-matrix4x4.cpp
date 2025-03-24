#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "matrix.h"
#include "vector.h"

// Tests for base_mat4 (4x4 matrices)
TEST_CASE("base_mat4 identity matrix", "[matrix][mat4]") {
    using ppp::base_mat4;
    using ppp::base_vec4;
    base_mat4<float> m;
    // Default identity: 
    // col0: (1, 0, 0, 0)
    // col1: (0, 1, 0, 0)
    // col2: (0, 0, 1, 0)
    // col3: (0, 0, 0, 1)
    REQUIRE(m.cols[0].x == Catch::Approx(1.0f));
    REQUIRE(m.cols[0].y == Catch::Approx(0.0f));
    REQUIRE(m.cols[0].z == Catch::Approx(0.0f));
    REQUIRE(m.cols[0].w == Catch::Approx(0.0f));

    REQUIRE(m.cols[1].x == Catch::Approx(0.0f));
    REQUIRE(m.cols[1].y == Catch::Approx(1.0f));
    REQUIRE(m.cols[1].z == Catch::Approx(0.0f));
    REQUIRE(m.cols[1].w == Catch::Approx(0.0f));

    REQUIRE(m.cols[2].x == Catch::Approx(0.0f));
    REQUIRE(m.cols[2].y == Catch::Approx(0.0f));
    REQUIRE(m.cols[2].z == Catch::Approx(1.0f));
    REQUIRE(m.cols[2].w == Catch::Approx(0.0f));

    REQUIRE(m.cols[3].x == Catch::Approx(0.0f));
    REQUIRE(m.cols[3].y == Catch::Approx(0.0f));
    REQUIRE(m.cols[3].z == Catch::Approx(0.0f));
    REQUIRE(m.cols[3].w == Catch::Approx(1.0f));
}

TEST_CASE("base_mat4 custom construction", "[matrix][mat4]") {
    using ppp::base_mat4;
    using ppp::base_vec4;
    base_vec4<float> col0(1.0f, 2.0f, 3.0f, 4.0f);
    base_vec4<float> col1(5.0f, 6.0f, 7.0f, 8.0f);
    base_vec4<float> col2(9.0f, 10.0f, 11.0f, 12.0f);
    base_vec4<float> col3(13.0f, 14.0f, 15.0f, 16.0f);
    base_mat4<float> m(col0, col1, col2, col3);
    REQUIRE(m.cols[0].x == Catch::Approx(1.0f));
    REQUIRE(m.cols[0].y == Catch::Approx(2.0f));
    REQUIRE(m.cols[0].z == Catch::Approx(3.0f));
    REQUIRE(m.cols[0].w == Catch::Approx(4.0f));

    REQUIRE(m.cols[1].x == Catch::Approx(5.0f));
    REQUIRE(m.cols[1].y == Catch::Approx(6.0f));
    REQUIRE(m.cols[1].z == Catch::Approx(7.0f));
    REQUIRE(m.cols[1].w == Catch::Approx(8.0f));

    REQUIRE(m.cols[2].x == Catch::Approx(9.0f));
    REQUIRE(m.cols[2].y == Catch::Approx(10.0f));
    REQUIRE(m.cols[2].z == Catch::Approx(11.0f));
    REQUIRE(m.cols[2].w == Catch::Approx(12.0f));

    REQUIRE(m.cols[3].x == Catch::Approx(13.0f));
    REQUIRE(m.cols[3].y == Catch::Approx(14.0f));
    REQUIRE(m.cols[3].z == Catch::Approx(15.0f));
    REQUIRE(m.cols[3].w == Catch::Approx(16.0f));
}
