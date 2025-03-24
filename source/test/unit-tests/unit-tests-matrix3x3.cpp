#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "matrix.h"
#include "vector.h"  // Needed because matrix types use base_vec types

// Tests for base_mat3 (3x3 matrices)
TEST_CASE("base_mat3 identity matrix", "[matrix][mat3]") {
    using ppp::base_mat3;
    using ppp::base_vec3;
    base_mat3<float> m;
    // Default identity: col0 (1,0,0), col1 (0,1,0), col2 (0,0,1)
    REQUIRE(m.cols[0].x == Catch::Approx(1.0f));
    REQUIRE(m.cols[0].y == Catch::Approx(0.0f));
    REQUIRE(m.cols[0].z == Catch::Approx(0.0f));

    REQUIRE(m.cols[1].x == Catch::Approx(0.0f));
    REQUIRE(m.cols[1].y == Catch::Approx(1.0f));
    REQUIRE(m.cols[1].z == Catch::Approx(0.0f));

    REQUIRE(m.cols[2].x == Catch::Approx(0.0f));
    REQUIRE(m.cols[2].y == Catch::Approx(0.0f));
    REQUIRE(m.cols[2].z == Catch::Approx(1.0f));
}

TEST_CASE("base_mat3 custom construction", "[matrix][mat3]") {
    using ppp::base_mat3;
    using ppp::base_vec3;
    base_vec3<float> col0(1.0f, 2.0f, 3.0f);
    base_vec3<float> col1(4.0f, 5.0f, 6.0f);
    base_vec3<float> col2(7.0f, 8.0f, 9.0f);
    base_mat3<float> m(col0, col1, col2);
    REQUIRE(m.cols[0].x == Catch::Approx(1.0f));
    REQUIRE(m.cols[0].y == Catch::Approx(2.0f));
    REQUIRE(m.cols[0].z == Catch::Approx(3.0f));

    REQUIRE(m.cols[1].x == Catch::Approx(4.0f));
    REQUIRE(m.cols[1].y == Catch::Approx(5.0f));
    REQUIRE(m.cols[1].z == Catch::Approx(6.0f));

    REQUIRE(m.cols[2].x == Catch::Approx(7.0f));
    REQUIRE(m.cols[2].y == Catch::Approx(8.0f));
    REQUIRE(m.cols[2].z == Catch::Approx(9.0f));
}