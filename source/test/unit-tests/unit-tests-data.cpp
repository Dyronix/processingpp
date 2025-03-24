#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN
#include "data.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

TEST_CASE("append function", "[data]") {
    std::vector<int> vec;
    REQUIRE(vec.empty());

    ppp::append(vec, 42);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0] == 42);

    ppp::append(vec, 100);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[1] == 100);
}

TEST_CASE("array_copy (5-parameter version)", "[data]") {
    std::vector<int> src = { 10, 20, 30, 40, 50 };
    std::vector<int> dst = { 1, 2, 3, 4, 5, 6 };

    // Copy from src starting at index 1, copy 3 elements, insert at index 2.
    // Expected: {1, 2, 20, 30, 40, 6}
    ppp::array_copy(src, 1, dst, 2, 3);
    std::vector<int> expected = { 1, 2, 20, 30, 40, 6 };
    REQUIRE(dst == expected);

    // When requested length exceeds available elements from src.
    dst = { 1, 2, 3, 4, 5, 6 };
    ppp::array_copy(src, 3, dst, 1, 10);
    // From src starting at index 3: {40,50}. Erase dst from index 1 for 2 elements.
    // Expected: {1, 40, 50, 4, 5, 6}
    expected = { 1, 40, 50, 4, 5, 6 };
    REQUIRE(dst == expected);

    // Test out-of-range conditions.
    REQUIRE_THROWS_AS(ppp::array_copy(src, -1, dst, 0, 2), std::out_of_range);
    REQUIRE_THROWS_AS(ppp::array_copy(src, 10, dst, 0, 2), std::out_of_range);
    REQUIRE_THROWS_AS(ppp::array_copy(src, 0, dst, -1, 2), std::out_of_range);
    REQUIRE_THROWS_AS(ppp::array_copy(src, 0, dst, dst.size() + 1, 2), std::out_of_range);
}

TEST_CASE("array_copy (2-parameter version)", "[data]") {
    std::vector<int> src = { 5, 6, 7, 8 };
    std::vector<int> dst = { 1, 2, 3 };
    ppp::array_copy(src, dst);
    REQUIRE(dst == src);
}

TEST_CASE("concat function", "[data]") {
    std::vector<int> list0 = { 1, 2, 3 };
    std::vector<int> list1 = { 4, 5 };
    std::vector<int> result = ppp::concat(list0, list1);
    std::vector<int> expected = { 1, 2, 3, 4, 5 };
    REQUIRE(result == expected);
}

TEST_CASE("reverse function", "[data]") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    ppp::reverse(vec);
    std::vector<int> expected = { 5, 4, 3, 2, 1 };
    REQUIRE(vec == expected);

    // Test reverse on an empty vector.
    std::vector<int> empty;
    ppp::reverse(empty);
    REQUIRE(empty.empty());
}

TEST_CASE("shorten function", "[data]") {
    std::vector<int> vec = { 10, 20, 30 };
    ppp::shorten(vec);
    std::vector<int> expected = { 10, 20 };
    REQUIRE(vec == expected);

    // Calling shorten on an empty vector should leave it empty.
    std::vector<int> empty;
    ppp::shorten(empty);
    REQUIRE(empty.empty());
}

TEST_CASE("shuffle function", "[data]") {
    // Test shuffle with modify == false.
    std::vector<int> original = { 1,2,3,4,5,6,7,8,9,10 };
    std::vector<int> vec = original;
    std::vector<int> shuffled = ppp::shuffle(vec, false);
    // Original remains unchanged.
    REQUIRE(vec == original);
    // Check that shuffled is a permutation of original.
    std::vector<int> sorted_shuffled = shuffled;
    std::sort(sorted_shuffled.begin(), sorted_shuffled.end());
    REQUIRE(sorted_shuffled == original);

    // Test shuffle with modify == true.
    vec = original;
    std::vector<int> shuffled_inplace = ppp::shuffle(vec, true);
    // The in-place shuffled vector should be a permutation of original.
    std::vector<int> sorted_inplace = shuffled_inplace;
    std::sort(sorted_inplace.begin(), sorted_inplace.end());
    REQUIRE(sorted_inplace == original);
}

TEST_CASE("sort_vector function", "[data]") {
    std::vector<int> unsorted = { 5, 2, 8, 3, 7, 1, 4 };

    // Sort entire vector.
    std::vector<int> sorted = ppp::sort_vector(unsorted);
    std::vector<int> expected_sorted = { 1, 2, 3, 4, 5, 7, 8 };
    REQUIRE(sorted == expected_sorted);

    // Sort only the first 4 elements.
    std::vector<int> partially_sorted = ppp::sort_vector(unsorted, 4);
    // The first 4 elements of unsorted {5,2,8,3} sorted become {2,3,5,8} and the rest remain unchanged.
    std::vector<int> expected_partial = { 2,3,5,8,7,1,4 };
    REQUIRE(partially_sorted == expected_partial);
}

TEST_CASE("splice function (vector overload)", "[data]") {
    std::vector<int> vec = { 1,2,3,4 };
    std::vector<int> values = { 8,9 };
    // Insert values at index 2: expected result {1,2,8,9,3,4}
    ppp::splice(vec, values, 2);
    std::vector<int> expected = { 1,2,8,9,3,4 };
    REQUIRE(vec == expected);

    // Test out-of-range index.
    REQUIRE_THROWS_AS(ppp::splice(vec, values, -1), std::out_of_range);
    REQUIRE_THROWS_AS(ppp::splice(vec, values, vec.size() + 1), std::out_of_range);
}

TEST_CASE("splice function (single value overload)", "[data]") {
    std::vector<int> vec = { 1,2,3,4 };
    // Insert single value 99 at index 1: expected result {1,99,2,3,4}
    ppp::splice(vec, 99, 1);
    std::vector<int> expected = { 1,99,2,3,4 };
    REQUIRE(vec == expected);

    // Test out-of-range index.
    REQUIRE_THROWS_AS(ppp::splice(vec, 100, -1), std::out_of_range);
    REQUIRE_THROWS_AS(ppp::splice(vec, 100, vec.size() + 1), std::out_of_range);
}

TEST_CASE("subset function", "[data]") {
    std::vector<int> vec = { 10,20,30,40,50 };

    // Subset starting at index 1 with count 3: expected {20,30,40}
    std::vector<int> sub = ppp::subset(vec, 1, 3);
    std::vector<int> expected = { 20,30,40 };
    REQUIRE(sub == expected);

    // Subset starting at index 2 to the end.
    sub = ppp::subset(vec, 2);
    expected = { 30,40,50 };
    REQUIRE(sub == expected);

    // Test out-of-range conditions.
    REQUIRE_THROWS_AS(ppp::subset(vec, 10), std::out_of_range);
    REQUIRE_THROWS_AS(ppp::subset(vec, 3, 5), std::out_of_range);
}
