#pragma once

namespace ppp
{
    namespace render
    {
        enum class topology_type
        {
            POINTS,
            LINES,
            TRIANGLES
        };

        enum class image_filter_type
        {
            NEAREST,
            LINEAR
        };

        enum class image_wrap_type
        {
            CLAMP_TO_EDGE,
            REPEAT,
        };

        enum class batch_buffer_policy
        {
            IMMEDIATE,
            STATIC
        };

        enum class batch_render_policy
        {
            BUILD_IN,
            CUSTOM
        };
    }
}