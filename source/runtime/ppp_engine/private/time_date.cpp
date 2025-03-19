#include "time_date.h"

#include <chrono>
#include <ctime>

namespace ppp
{
    // Record the time when the program starts (for millis()).
    static const auto _millis_start = std::chrono::steady_clock::now();

    int day() 
    {
        std::time_t t = std::time(nullptr);
        std::tm* local_time = std::localtime(&t);
        return local_time->tm_mday;
    }

    int hour() 
    {
        std::time_t t = std::time(nullptr);
        std::tm* local_time = std::localtime(&t);
        return local_time->tm_hour;
    }

    int minute() 
    {
        std::time_t t = std::time(nullptr);
        std::tm* local_time = std::localtime(&t);
        return local_time->tm_min;
    }

    int second() 
    {
        std::time_t t = std::time(nullptr);
        std::tm* local_time = std::localtime(&t);
        return local_time->tm_sec;
    }

    int month() 
    {
        std::time_t t = std::time(nullptr);
        std::tm* local_time = std::localtime(&t);
        return local_time->tm_mon + 1;  // tm_mon is 0-indexed (0 = January)
    }

    int year() 
    {
        std::time_t t = std::time(nullptr);
        std::tm* local_time = std::localtime(&t);
        return local_time->tm_year + 1900;
    }

    long long millis() 
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _millis_start).count();
        return elapsed;
    }
}