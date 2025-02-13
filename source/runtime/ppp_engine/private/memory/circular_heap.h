#pragma once

#include "memory/memory_size.h"

namespace ppp
{
    namespace memory
    {
        class heap;

        class circular_heap
        {
        public:
            circular_heap(heap* heap, memory_size size);

        public:
            void*           allocate(memory_size size) noexcept;
            void            deallocate(void* ptr) noexcept;

            void            free();

        public:
            bool            can_alloc(memory_size size) const;

            memory_size     total_memory() const;
            memory_size     current_memory() const;

        private:
            bool            has_space(u64 allocation_start, u64 size) const;

        private:
            memory_size     m_total_size;

            void*           m_base_memory;

            // m_head:      the next allocation offset.
            // m_tail:      the oldest still–active allocation (for a more sophisticated free–list you would update tail when freeing oldest blocks).
            u64             m_tail;
            u64             m_head;
        };
    }
}