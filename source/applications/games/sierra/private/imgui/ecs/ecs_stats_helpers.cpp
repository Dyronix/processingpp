#include "imgui/ecs/ecs_stats.h"

#include <stdio.h>

namespace ppp
{
    namespace imgui
    {
        int format_ticks_as_int(double val, char* buff, int size, void*)
        {
            return snprintf(buff, size, "%d", static_cast<int>(val));
        }
    }
}