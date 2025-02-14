#pragma once

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        namespace tags
        {
            static const u32 geometry = BIT(0);
            static const u32 graphics = BIT(1);
            static const u32 fileio = BIT(2);
            static const u32 resources = BIT(3);
            static const u32 global = BIT(4);
            static const u32 initialization = BIT(5);
        }
    }
}