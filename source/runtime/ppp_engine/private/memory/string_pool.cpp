#include "memory/string_pool.h"
#include "memory/heap.h"

namespace ppp
{
    namespace memory
    {
        constexpr memory_size _block_size = 10_mb;

        static string_pool _string_pool(get_heap(), _block_size);

        //-------------------------------------------------------------------------
        string_pool* get_string_pool()
        {
            return &_string_pool;
        }
    }
}