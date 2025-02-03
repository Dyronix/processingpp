#include "memory/tagged_heap.h"

#include <assert.h>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        tagged_heap::tagged_heap(memory_size block_size, s32 block_count)
            :m_block_size(block_size)
            ,m_block_count(block_count)
        {
            m_base_memory = static_cast<u8*>(std::malloc(block_size * block_count));
            
            std::memset(m_base_memory, 0, block_size * block_count);

            assert(m_base_memory && "Memory allocation failed");

            m_blocks.reserve(block_count);

            for (s32 i = 0; i < block_count; ++i)
            {
                m_blocks.push_back(tagged_heap_block(block_size, m_base_memory + (i * block_size.size_in_bytes())));
            }
        }

        //-------------------------------------------------------------------------
        tagged_heap::~tagged_heap()
        {
            if (m_base_memory != nullptr)
            {
                std::free(m_base_memory);
            }
        }

        //-------------------------------------------------------------------------
        void* tagged_heap::allocate(u32 tag, memory_size size)
        {
            for (auto& block : m_blocks) 
            {
                if ((block.tag() == tag || block.tag() == 0) && (block.offset() + size <= m_block_size)) 
                {
                    return block.allocate(tag, size);
                }
            }

            assert("Out of memory!");
            return nullptr; // Out of memory
        }

        //-------------------------------------------------------------------------
        void tagged_heap::free_blocks(u32 tag)
        {
            for (auto& block : m_blocks)
            {
                if (block.tag() == tag)
                {
                    block.free();
                }
            }
        }

        //-------------------------------------------------------------------------
        void tagged_heap::free()
        {
            for (auto& block : m_blocks)
            {
                block.free();
            }

            std::free(m_base_memory);

            m_base_memory = nullptr;
        }

        //-------------------------------------------------------------------------
        tagged_heap* get_tagged_heap()
        {
            constexpr memory_size block_size = 500_kb;
            constexpr s32 block_count = 10;

            static tagged_heap s_tagged_heap(block_size, block_count);

            return &s_tagged_heap;
        }
    }
}