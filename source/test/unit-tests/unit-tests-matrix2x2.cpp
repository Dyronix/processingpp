#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "matrix.h"
#include "vector.h"  // Needed because matrix types use base_vec types

// Tests for base_mat2 (2x2 matrices)
TEST_CASE("base_mat2 identity matrix", "[matrix][mat2]") {
    using ppp::base_mat2;
    using ppp::base_vec2;
    base_mat2<float> m;
    // Default identity: first column (1,0), second column (0,1)
    REQUIRE(m.cols[0].x == Catch::Approx(1.0f));
    REQUIRE(m.cols[0].y == Catch::Approx(0.0f));
    REQUIRE(m.cols[1].x == Catch::Approx(0.0f));
    REQUIRE(m.cols[1].y == Catch::Approx(1.0f));
}

TEST_CASE("base_mat2 custom construction", "[matrix][mat2]") {
    using ppp::base_mat2;
    using ppp::base_vec2;
    base_vec2<float> col0(2.0f, 3.0f);
    base_vec2<float> col1(4.0f, 5.0f);
    base_mat2<float> m(col0, col1);
    REQUIRE(m.cols[0].x == Catch::Approx(2.0f));
    REQUIRE(m.cols[0].y == Catch::Approx(3.0f));
    REQUIRE(m.cols[1].x == Catch::Approx(4.0f));
    REQUIRE(m.cols[1].y == Catch::Approx(5.0f));
}