#include "memory/string_pool.h"
#include "memory/heap.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        string_pool* get_string_pool()
        {
            constexpr memory_size block_size = 2_mb;

            static string_pool s_string_pool(get_heap(), block_size);

            return &s_string_pool;
        }
    }
}