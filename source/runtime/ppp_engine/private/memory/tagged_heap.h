#pragma once

#include "util/types.h"

#include "memory/memory_size.h"
#include "memory/tagged_heap_block.h"

namespace ppp
{
    namespace memory
    {
        class heap;

        using tagged_heap_blocks = std::vector<tagged_heap_block>;
        class tagged_heap
        {
        public:
            tagged_heap(heap* heap, memory_size block_size, s32 block_count);
            ~tagged_heap();

            void* allocate(u32 tag, memory_size size);
            void deallocate(u32 tag, void* ptr);

            void free_blocks(u32 tag);
            void free();

        private:
            u8*                 m_base_memory;

            memory_size         m_block_size;
            s32                 m_block_count;
            tagged_heap_blocks  m_blocks;
        };

        tagged_heap* get_tagged_heap();
    }
}