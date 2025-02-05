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

        public:
            memory_size current_size() const { return m_offset; }
            memory_size total_size() const { return m_total_size; }

        private:
            u8* m_base_memory;
            u64 m_offset;

            memory_size m_total_size;
        };

        heap* get_heap();
    }
}