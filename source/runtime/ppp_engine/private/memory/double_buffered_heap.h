#pragma once

#include "memory/memory_size.h"
#include "memory/linear_heap.h"

namespace ppp
{
    namespace memory
    {
        class heap;

        class double_buffered_heap
        {
        public:
            double_buffered_heap(heap* heap, memory_size size);

        public:
            void*           allocate(memory_size size) noexcept;
            void            deallocate(void* ptr) noexcept;

            void            free();

        public:
            memory_size     total_memory() const;
            memory_size     current_memory() const;

        private:
            void            present();

        private:
            linear_heap*    m_active_linear_heap;

            linear_heap     m_buffer_0;
            linear_heap     m_buffer_1;
        };
    }
}