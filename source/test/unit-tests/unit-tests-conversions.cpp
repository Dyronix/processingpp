#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#define CATCH_CONFIG_MAIN
#include "conversions.h"
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

// ------------------------------------------------------------------------
// to_float tests
// ------------------------------------------------------------------------
TEST_CASE("to_float from string", "[conversions][to_float]")
{
    REQUIRE(ppp::to_float("3.14") == Catch::Approx(3.14f));
    REQUIRE(ppp::to_float("0") == Catch::Approx(0.0f));

    float nan_val = ppp::to_float("invalid");
    REQUIRE(std::isnan(nan_val));
}

TEST_CASE("to_float from vector of strings", "[conversions][to_float]")
{
    std::vector<std::string> vec = { "1.0", "2.5", "bad" };
    std::vector<float> result = ppp::to_float(vec);
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == Catch::Approx(1.0f));
    REQUIRE(result[1] == Catch::Approx(2.5f));
    REQUIRE(std::isnan(result[2]));
}

TEST_CASE("to_float from float", "[conversions][to_float]")
{
    float value = 5.0f;
    REQUIRE(ppp::to_float(value) == Catch::Approx(5.0f));
}

// ------------------------------------------------------------------------
// to_int tests
// ------------------------------------------------------------------------
TEST_CASE("to_int from string", "[conversions][to_int]")
{
    REQUIRE(ppp::to_int("123") == 123);
    REQUIRE(ppp::to_int("0") == 0);
    REQUIRE(ppp::to_int("Infinity") == std::numeric_limits<int>::max());
    REQUIRE(ppp::to_int("-Infinity") == std::numeric_limits<int>::min());
    // Invalid conversion should throw
    REQUIRE_THROWS_AS(ppp::to_int("abc"), std::invalid_argument);
}

TEST_CASE("to_int from string with radix", "[conversions][to_int]")
{
    // "FF" in hexadecimal should convert to 255.
    REQUIRE(ppp::to_int("FF", 16) == 255);
}

TEST_CASE("to_int from float", "[conversions][to_int]")
{
    // Decimal part is truncated.
    REQUIRE(ppp::to_int(3.99f) == 3);
    // Test infinity conversion.
    REQUIRE(ppp::to_int(std::numeric_limits<float>::infinity()) == std::numeric_limits<int>::max());
    REQUIRE(ppp::to_int(-std::numeric_limits<float>::infinity()) == std::numeric_limits<int>::min());
}

TEST_CASE("to_int from bool", "[conversions][to_int]")
{
    REQUIRE(ppp::to_int(true) == 1);
    REQUIRE(ppp::to_int(false) == 0);
}

TEST_CASE("to_int from vector", "[conversions][to_int]")
{
    std::vector<std::string> vec = { "10", "20", "30" };
    std::vector<int> result = ppp::to_int(vec);
    std::vector<int> expected = { 10, 20, 30 };
    REQUIRE(result == expected);
}

// ------------------------------------------------------------------------
// to_str tests
// ------------------------------------------------------------------------
TEST_CASE("to_str conversion", "[conversions][to_str]")
{
    // Identity for strings.
    REQUIRE(ppp::to_str(std::string("hello")) == "hello");
    // int to string.
    REQUIRE(ppp::to_str(123) == "123");
    // float to string (non-empty result).
    std::string float_str = ppp::to_str(3.14f);
    REQUIRE(!float_str.empty());
    // bool to string.
    REQUIRE(ppp::to_str(true) == "true");
    REQUIRE(ppp::to_str(false) == "false");
}

TEST_CASE("to_str vector conversion", "[conversions][to_str]")
{
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<std::string> result = ppp::to_str(vec);
    std::vector<std::string> expected = { "1", "2", "3" };
    REQUIRE(result == expected);
}

// ------------------------------------------------------------------------
// to_boolean tests
// ------------------------------------------------------------------------
TEST_CASE("to_boolean conversion", "[conversions][to_boolean]")
{
    // String conversion.
    REQUIRE(ppp::to_boolean("true") == true);
    REQUIRE(ppp::to_boolean("false") == false);
    REQUIRE(ppp::to_boolean("TrUe") == true);
    REQUIRE(ppp::to_boolean("FaLsE") == false);
    // int conversion.
    REQUIRE(ppp::to_boolean(0) == false);
    REQUIRE(ppp::to_boolean(10) == true);
    // float conversion.
    REQUIRE(ppp::to_boolean(0.0f) == false);
    REQUIRE(ppp::to_boolean(0.1f) == true);
    // bool conversion.
    REQUIRE(ppp::to_boolean(true) == true);
    REQUIRE(ppp::to_boolean(false) == false);
}

TEST_CASE("to_boolean vector conversion", "[conversions][to_boolean]")
{
    std::vector<int> vec = { 0, 1, 2, 0 };
    std::vector<bool> result = ppp::to_boolean(vec);
    std::vector<bool> expected = { false, true, true, false };
    REQUIRE(result == expected);
}

// ------------------------------------------------------------------------
// to_byte tests
// ------------------------------------------------------------------------
TEST_CASE("to_byte conversion", "[conversions][to_byte]")
{
    // from string
    REQUIRE(ppp::to_byte("0") == 0);
    REQUIRE(ppp::to_byte("128") == -128);
    REQUIRE(ppp::to_byte("129") == -127);
    // from int
    REQUIRE(ppp::to_byte(0) == 0);
    REQUIRE(ppp::to_byte(128) == -128);
    REQUIRE(ppp::to_byte(129) == -127);
    // from float (uses conversion via int)
    REQUIRE(ppp::to_byte(128.5f) == -128);
    // from bool: true becomes 1.
    REQUIRE(ppp::to_byte(true) == ((1 + 128) % 256 - 128));
}

TEST_CASE("to_byte vector conversion", "[conversions][to_byte]")
{
    std::vector<int> vec = { 0, 128, 129 };
    std::vector<int> result = ppp::to_byte(vec);
    std::vector<int> expected = { 0, -128, -127 };
    REQUIRE(result == expected);
}

// ------------------------------------------------------------------------
// to_char tests
// ------------------------------------------------------------------------
TEST_CASE("to_char conversion", "[conversions][to_char]")
{
    // int to char: 65 -> "A"
    REQUIRE(ppp::to_char(65) == "A");
    // float to char: 66.7 -> int 66 -> "B"
    REQUIRE(ppp::to_char(66.7f) == "B");
    // string to char: "67" -> int 67 -> "C"
    REQUIRE(ppp::to_char(std::string("67")) == "C");
}

TEST_CASE("to_char vector conversion", "[conversions][to_char]")
{
    std::vector<int> vec = { 65, 66, 67 };
    std::vector<std::string> result = ppp::to_char(vec);
    std::vector<std::string> expected = { "A", "B", "C" };
    REQUIRE(result == expected);
}

// ------------------------------------------------------------------------
// unchar tests
// ------------------------------------------------------------------------
TEST_CASE("unchar conversion", "[conversions][unchar]")
{
    // Single character "A" should return its ASCII code 65.
    REQUIRE(ppp::unchar("A") == 65);
    // Invalid input: string with more than one character.
    REQUIRE_THROWS_AS(ppp::unchar("AB"), std::invalid_argument);
}

TEST_CASE("unchar vector conversion", "[conversions][unchar]")
{
    std::vector<std::string> vec = { "A", "B", "C" };
    std::vector<int> result = ppp::unchar(vec);
    std::vector<int> expected = { 65, 66, 67 };
    REQUIRE(result == expected);
}

// ------------------------------------------------------------------------
// hex_val tests
// ------------------------------------------------------------------------

// Helper lambda to convert string to lowercase.
auto to_lower = [](const std::string& s)
    {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
    };

TEST_CASE("hex_val conversion", "[conversions][hex_val]")
{
    // For a positive integer.
    std::string hex1 = ppp::hex_val(255, 4);
    // Expected: "00FF" (case-insensitive)
    REQUIRE(to_lower(hex1) == "00ff");

    // For a negative integer: -1 becomes 0xffffffff (8 digits).
    std::string hex2 = ppp::hex_val(-1, 8);
    REQUIRE(to_lower(hex2) == "ffffffff");

    // For float: 255.0f should match int conversion.
    std::string hex3 = ppp::hex_val(255.0f, 4);
    REQUIRE(hex3 == hex1);

    // For double.
    std::string hex4 = ppp::hex_val(255.0, 4);
    REQUIRE(hex4 == hex1);
}

TEST_CASE("hex_val vector conversion", "[conversions][hex_val]")
{
    std::vector<int> vec = { 255, -1 };
    std::vector<std::string> result = ppp::hex_val(vec, 8);
    REQUIRE(result.size() == 2);
    REQUIRE(to_lower(result[0]) == "000000ff");
    REQUIRE(to_lower(result[1]) == "ffffffff");
}

// ------------------------------------------------------------------------
// unhex tests
// ------------------------------------------------------------------------
TEST_CASE("unhex conversion", "[conversions][unhex]")
{
    // "00ff" should convert to 255.
    REQUIRE(ppp::unhex("00ff") == 255);
    // Invalid hex string should throw.
    REQUIRE_THROWS_AS(ppp::unhex("GHIJK"), std::invalid_argument);
}

TEST_CASE("unhex vector conversion", "[conversions][unhex]")
{
    std::vector<std::string> vec = { "00ff", "ffffffff" };
    std::vector<int> result = ppp::unhex(vec);
    // Expect 255 and, for "ffffffff", likely -1 (depending on implementation).
    std::vector<int> expected = { 255, -1 };
    REQUIRE(result == expected);
}
