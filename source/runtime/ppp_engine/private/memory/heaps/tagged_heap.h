#pragma once

#include "util/types.h"

#include "memory/memory_size.h"
#include "memory/heaps/tagged_heap_block.h"

namespace ppp
{
    namespace memory
    {
        class iheap;

        using tagged_heap_blocks = std::vector<tagged_heap_block>;

        class tagged_heap
        {
        public:
            tagged_heap(iheap* heap, memory_size block_size, s32 block_count);
            ~tagged_heap();

            void* allocate(u32 tag, memory_size size) noexcept;
            void deallocate(u32 tag, void* ptr) noexcept;

            void free_blocks(u32 tag);
            void free();

        public:
            bool can_alloc(memory_size size) const;

            memory_size current_size(s32 block_index) const;
            memory_size total_size(s32 block_index) const;

        public:
            s32 block_count() const;
            u32 block_tag(s32 block_index) const;

        private:
            u8*                 m_base_memory;

            memory_size         m_block_size;
            s32                 m_block_count;
            tagged_heap_blocks  m_blocks;
        };
    }
}