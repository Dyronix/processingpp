#include "memory/string_heap.h"
#include "memory/heap.h"

namespace ppp
{
    namespace memory
    {
        constexpr memory_size _block_size = 10_mb;

        static string_heap _string_heap(get_heap(), _block_size);

        //-------------------------------------------------------------------------
        string_heap* get_string_heap()
        {
            return &_string_heap;
        }
    }
}