#pragma once

#include "memory/memory_size.h"

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        class heap
        {
        public:
            heap(memory_size size);
            ~heap();

            void* allocate(memory_size size);

            void free();

        private:
            u8* m_base_memory;
            u64 m_offset;
        };

        heap* get_heap();
    }
}