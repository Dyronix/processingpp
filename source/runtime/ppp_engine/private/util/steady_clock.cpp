#include "steady_clock.h"

#include <thread>

namespace ppp
{
    namespace clock
    {
        namespace internal
        {
            void sleep_for(milliseconds duration)
            {
                if (duration.count() > 0)
                {
                    std::this_thread::sleep_for(duration);
                }
            }
        }

        time_point now() 
        {
            using namespace std::chrono;

            return steady_clock::now();
        }

        milliseconds duration(time_point start, time_point end) 
        {
            using namespace std::chrono;

            auto elapsed_duration = duration_cast<milliseconds>(end - start);
            return elapsed_duration;
        }

        void precise_sleep_for(milliseconds duration)
        {
            using namespace std::chrono;

            auto start_time = steady_clock::now();

            // Spin wait until the target time has passed
            while (steady_clock::now() - start_time < duration) {}
        }

        void accurate_sleep_for(milliseconds duration)
        {
            using namespace std::chrono;

            // If the duration is greater than 10 milliseconds, sleep for most of it
            // This threshold is used because sleep durations shorter than 10 milliseconds 
            // may not be accurately represented by std::this_thread::sleep_for(), 
            // leading to potential delays that can accumulate over multiple frames.
            // For very short durations, busy-waiting ensures that we achieve higher precision,
            // allowing the application to remain responsive and meet tight timing constraints.

            constexpr auto min_sleep_duration = 15ms; // 15 milliseconds
            if (duration > min_sleep_duration)
            {
                milliseconds sleep_duration(duration.count() - 1);
                internal::sleep_for(sleep_duration);

                duration -= sleep_duration;
            }

            // Busy-wait for the remainder of the duration
            precise_sleep_for(duration);
        }
    }
}
