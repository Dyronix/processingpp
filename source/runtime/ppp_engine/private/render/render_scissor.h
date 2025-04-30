#pragma once

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        struct render_scissor
        {
            s32     x = -1;
            s32     y = -1;
            s32     width = -1;
            s32     height = -1;

            bool    enable = false;
        };

        void push_scissor(s32 x, s32 y, s32 width, s32 height);
        void push_scissor_enable(bool enable);

        bool scissor_rect_enabled();

        const render_scissor* scissor_rect();
    }
}