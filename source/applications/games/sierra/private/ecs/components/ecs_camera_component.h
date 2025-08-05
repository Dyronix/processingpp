#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace ecs
    {
        enum class projection_type
        {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        struct camera_component
        {
            std::string_view tag;

            projection_type type = projection_type::PERSPECTIVE;

            f32 near_clip = 0.1f;
            f32 far_clip = 2000.0f;

            // perspective
            f32 fovy = 55.0f;
            f32 aspect_ratio = 16.0f / 9.0f;

            // ortho
            f32 left;
            f32 right;
            f32 bottom;
            f32 top;
        };
    }
}
