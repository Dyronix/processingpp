#pragma once

#include "memory/free_list_heap.h"

namespace ppp
{
    namespace memory
    {
        using string_heap = free_list_heap;

        string_heap* get_string_heap();
    }
}