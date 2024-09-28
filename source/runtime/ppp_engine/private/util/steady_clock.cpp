#include "steady_clock.h"

namespace ppp
{
    namespace clock
    {
        time_point now() 
        {
            return std::chrono::steady_clock::now();
        }

        milliseconds duration(time_point start, time_point end) 
        {
            auto elapsed_duration = std::chrono::duration_cast<milliseconds>(end - start);
            return elapsed_duration;
        }

        void sleep_for(milliseconds duration) 
        {
            if (duration.count() > 0) 
            {
                std::this_thread::sleep_for(duration);
            }
        }
    }
}
