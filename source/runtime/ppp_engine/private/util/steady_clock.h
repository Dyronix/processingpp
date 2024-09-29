#pragma once

#include "util/types.h"

#include <chrono>

namespace ppp
{
    namespace clock
    {
        using milliseconds = std::chrono::milliseconds;
        using time_point = std::chrono::steady_clock::time_point;

        time_point now();

        milliseconds duration(time_point start, time_point end);

        void precise_sleep_for(milliseconds duration);
        void accurate_sleep_for(milliseconds duration);
    }
}
