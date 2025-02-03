#pragma once

#include "util/types.h"

#include "memory/memory_size.h"

namespace ppp
{
    namespace memory
    {
        class tagged_heap_block
        {
        public:
            tagged_heap_block(memory_size size, u8* memory);

            u8*             allocate(s32 tag, memory_size size);

            void            free();

        public:
            memory_size     size() const { return m_size; }
            u32             tag() const { return m_tag; }
            u64             offset() const { return m_offset; }
            u8*             memory() const { return m_memory; }

        private:
            memory_size     m_size;
            u32             m_tag;
            u64             m_offset;
            u8*             m_memory;
        };
    }
}