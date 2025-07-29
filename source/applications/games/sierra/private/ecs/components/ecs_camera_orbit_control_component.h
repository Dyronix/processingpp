#pragma once

#include "util/types.h"

namespace ppp
{
    namespace ecs
    {
        struct orbit_control_component
        {
            f32 zoom_sensitivity = 200.0f;
            f32 panning_sensitivity = 0.5f;
            f32 rotation_sensitivity = 0.5f;
            f32 min_zoom = 1.0f;
            f32 max_zoom = 600.0f;
        };
    }
}
