#pragma once

#include <chrono>
#include <thread>

namespace ppp
{
    namespace clock
    {
        using milliseconds = std::chrono::milliseconds;
        using time_point = std::chrono::steady_clock::time_point;

        time_point now();

        milliseconds duration(time_point start, time_point end);

        void sleep_for(milliseconds duration);
    }
}
