#pragma once

namespace ppp
{
    // Returns the current day (1-31).
    int day();

    // Returns the current hour (0-23).
    int hour();

    // Returns the current minute (0-59).
    int minute();

    // Returns the current second (0-59).
    int second();

    // Returns the current month (1-12).
    int month();

    // Returns the current year (e.g., 2025).
    int year();

    // Returns the number of milliseconds since the program started.
    long long millis();
}