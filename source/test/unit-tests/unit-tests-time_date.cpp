#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN
#include "time_date.h"
#include <chrono>
#include <thread>

TEST_CASE("Time and date functions return values in valid ranges", "[time_date]")
{
    int d = ppp::day();
    int h = ppp::hour();
    int m = ppp::minute();
    int s = ppp::second();
    int mon = ppp::month();
    int y = ppp::year();

    // Day should be between 1 and 31.
    REQUIRE(d >= 1);
    REQUIRE(d <= 31);

    // Hour should be between 0 and 23.
    REQUIRE(h >= 0);
    REQUIRE(h <= 23);

    // Minute and second should be between 0 and 59.
    REQUIRE(m >= 0);
    REQUIRE(m <= 59);
    REQUIRE(s >= 0);
    REQUIRE(s <= 59);

    // Month should be between 1 and 12.
    REQUIRE(mon >= 1);
    REQUIRE(mon <= 12);

    // Year should be a reasonable value (e.g., >= 1970).
    REQUIRE(y >= 1970);
}

TEST_CASE("millis function returns elapsed time", "[time_date]")
{
    long long start = ppp::millis();
    // Wait a short duration.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    long long end = ppp::millis();

    // At least 50 milliseconds should have elapsed (allow some margin for scheduling delays).
    REQUIRE(end - start >= 50);
}
