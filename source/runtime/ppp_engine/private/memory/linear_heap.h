#pragma once

#include "memory/memory_size.h"

namespace ppp
{
    namespace memory
    {
        class heap;

        class linear_heap
        {
        public:
            linear_heap(heap* heap, memory_size size);

        public:
            void*           allocate(memory_size size) noexcept;
            void            deallocate(void* ptr) noexcept;

            void            free();

        public:
            bool            can_alloc(memory_size size) const;

            memory_size     total_memory() const { return m_total_memory; }
            memory_size     current_memory() const { return m_offset; }

        private:
            memory_size     m_total_memory;
            void*           m_base_memory;
            u64             m_offset;
        };
    }
}