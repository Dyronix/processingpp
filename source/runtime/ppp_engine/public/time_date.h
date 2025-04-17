/**
 * @file time_date.h
 * @brief Utilities for retrieving current system date/time.
 */
#pragma once

namespace ppp
{
    /** @brief Get current day of month (1-31). */
    int day();
    /** @brief Get current hour of day (0-23). */
    int hour();
    /** @brief Get current minute of hour (0-59). */
    int minute();
    /** @brief Get current second of minute (0-59). */
    int second();
    /** @brief Get current month (1-12). */
    int month();
    /** @brief Get current year (e.g., 2025). */
    int year();
    /** @brief Get milliseconds elapsed since program start. */
    long long millis();
}
