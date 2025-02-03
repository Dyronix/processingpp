#pragma once

#include "memory/free_list_heap.h"

namespace ppp
{
    namespace memory
    {
        using string_pool = free_list_heap;

        string_pool* get_string_pool();
    }
}