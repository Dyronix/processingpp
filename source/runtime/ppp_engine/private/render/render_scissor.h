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
    }
}