#pragma once

#include "util/types.h"

namespace ppp
{
    namespace ecs
    {
        struct free_control_component
        {
            f32 look_sensitivity = 0.1f;
            f32 movement_speed = 100.0f;
            f32 boost_multiplier = 4.0f;
        };
    }
}
