#pragma once

#include "util/types.h"

namespace ppp
{
    namespace framebuffer_flags
    {
        constexpr s32 DEPTH = BIT(0);
        constexpr s32 SAMPLED_DEPTH = BIT(1);
        constexpr s32 COLOR = BIT(2);
    }
}