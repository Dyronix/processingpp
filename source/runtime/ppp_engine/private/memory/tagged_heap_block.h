#pragma once

#include "util/types.h"

#include "memory/memory_size.h"
#include "memory/free_list_heap.h"

namespace ppp
{
    namespace memory
    {
        class tagged_heap_block
        {
        public:
            tagged_heap_block(heap* heap, memory_size size);

            u8*             allocate(s32 tag, memory_size size);
            void            deallocate(void* ptr);

            void            free();

        public:
            u32             tag() const { return m_tag; }

            memory_size     current_size() const { return m_free_list_heap.current_size(); }
            memory_size     total_size() const { return m_free_list_heap.total_size(); }

            bool            can_alloc(memory_size size) const { return m_free_list_heap.can_alloc(size); }

        private:
            u32             m_tag;
            free_list_heap  m_free_list_heap;
        };
    }
}