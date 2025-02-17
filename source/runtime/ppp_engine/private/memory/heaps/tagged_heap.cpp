#include "memory/heaps/tagged_heap.h"

#include "util/log.h"

#include <assert.h>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        tagged_heap::tagged_heap(iheap* heap, memory_size block_size, s32 block_count)
            :m_block_size(block_size)
            ,m_block_count(block_count)
        {
            if (block_count > 0 && block_size.size_in_bytes() > 0)
            {
                m_blocks.reserve(block_count);

                for (s32 i = 0; i < block_count; ++i)
                {
                    m_blocks.push_back(tagged_heap_block(heap, block_size));
                }
            }
        }

        //-------------------------------------------------------------------------
        tagged_heap::~tagged_heap()
        {
            free();
        }

        //-------------------------------------------------------------------------
        void* tagged_heap::allocate(u32 tag, memory_size size) noexcept
        {
            assert(!m_blocks.empty());

            for (auto& block : m_blocks) 
            {
                if ((block.tag() == tag || block.tag() == 0) && (block.can_alloc(size))) 
                {
                    return block.allocate(tag, size);
                }
            }

            assert("Out of memory!");
            return nullptr; // Out of memory
        }

        //-------------------------------------------------------------------------
        void tagged_heap::deallocate(u32 tag, void* ptr) noexcept
        {
            assert(!m_blocks.empty());

            for (auto& block : m_blocks)
            {
                if (block.tag() == tag)
                {
                    block.deallocate(ptr);
                }
            }
        }

        //-------------------------------------------------------------------------
        bool tagged_heap::can_alloc(memory_size size) const
        {
            for (auto& block : m_blocks)
            {
                if (block.can_alloc(size))
                {
                    return true;
                }
            }

            return false;
        }

        //-------------------------------------------------------------------------
        void tagged_heap::free_blocks(u32 tag)
        {
            if (!m_blocks.empty())
            {
                for (auto& block : m_blocks)
                {
                    if (block.tag() == tag)
                    {
                        block.free();
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        void tagged_heap::free()
        {
            if (!m_blocks.empty())
            {
                for (auto& block : m_blocks)
                {
                    block.free();
                }

                m_blocks.clear();
                m_block_count = 0;
            }
        }

        //-------------------------------------------------------------------------
        s32 tagged_heap::block_count() const
        {
            return m_block_count;
        }

        //-------------------------------------------------------------------------
        u32 tagged_heap::block_tag(s32 block_index) const
        {
            if (!m_blocks.empty())
            {
                assert(block_index < m_block_count);
                return m_blocks[block_index].tag();
            }

            return -1;
        }

        //-------------------------------------------------------------------------
        memory_size tagged_heap::current_size(s32 block_index) const
        {
            if (!m_blocks.empty())
            {
                assert(block_index < m_block_count);
                return m_blocks[block_index].current_size();
            }
            
            return 0;
        }

        //-------------------------------------------------------------------------
        memory_size tagged_heap::total_size(s32 block_index) const
        {
            if (!m_blocks.empty())
            {
                assert(block_index < m_block_count);
                return m_blocks[block_index].total_size();
            }

            return 0;
        }
    }
}