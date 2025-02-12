#include "memory/scratch_pool.h"
#include "memory/heap.h"

namespace ppp
{
    namespace memory
    {
        constexpr memory_size _block_size = 10_mb;

        static scratch_pool _scratch_pool(get_heap(), _block_size);

        //-------------------------------------------------------------------------
        scratch_pool* get_scratch_pool()
        {
            return &_scratch_pool;
        }
    }
}