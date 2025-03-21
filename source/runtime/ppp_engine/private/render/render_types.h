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

        enum class vertex_type
        {
            POSITION,
            POSITION_NORMAL,
            POSITION_NORMAL_COLOR,
            POSITION_TEXCOORD,
            POSITION_TEXCOORD_NORMAL,
            POSITION_COLOR,
            POSITION_TEXCOORD_COLOR,
            POSITION_TEXCOORD_NORMAL_COLOR
        };

        enum class shading_model_type
        {
            LIT,
            UNLIT
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

        enum class render_buffer_policy
        {
            IMMEDIATE,
            RETAINED
        };

        enum class render_draw_policy
        {
            BUILD_IN,
            CUSTOM
        };

        enum class render_draw_mode
        {
            INSTANCED,
            BATCHED
        };

        enum class render_rendering_mode
        {
            FORWARD,
            DEFERRED
        };
    }
}