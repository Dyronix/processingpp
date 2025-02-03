#include "memory/scratch_pool.h"
#include "memory/heap.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        scratch_pool* get_scratch_pool()
        {
            constexpr memory_size block_size = 2_mb;

            static scratch_pool s_scratch_pool(get_heap(), block_size);

            return &s_scratch_pool;
        }
    }
}