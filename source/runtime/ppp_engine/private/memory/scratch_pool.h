#pragma once

#include "memory/free_list_heap.h"

namespace ppp
{
    namespace memory
    {
        using scratch_pool = free_list_heap;

        scratch_pool* get_scratch_pool();
    }
}