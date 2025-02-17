#pragma once

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        namespace tags
        {
            static const u32 graphics = BIT(0);
            static const u32 fileio = BIT(1);
            static const u32 global = BIT(2);
        }
    }
}