#pragma once

#include "memory/heaps/heap.h"

#include "memory/heaps/linear_heap.h"
#include "memory/heaps/circular_heap.h"
#include "memory/heaps/tagged_heap.h"

namespace ppp
{
    namespace memory
    {
        class memory_manager
        {
        public:
            // Access the singleton instance
            static memory_manager&  instance();

            // Getters for each sub-heap
            heap*                   get_global_heap();
            
            linear_heap*            get_frame_heap();
            tagged_heap*            get_tagged_heap();
            circular_heap*          get_scratch_heap();


        private:
            // Private constructor/destructor to enforce singleton
            memory_manager(memory_size size);
            ~memory_manager();

            // Non-copyable
            memory_manager(const memory_manager&) = delete;
            memory_manager& operator=(const memory_manager&) = delete;

        private:
            // The "base" heap that owns the entire block of memory
            heap             m_heap;

            // Sub-allocators
            linear_heap      m_frame_heap;
            tagged_heap      m_tagged_heap;
            circular_heap    m_scratch_heap;
        };
    }
}