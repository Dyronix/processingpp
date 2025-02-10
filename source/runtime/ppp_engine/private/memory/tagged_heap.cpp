#include "memory/tagged_heap.h"
#include "memory/heap.h"

#include <assert.h>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        tagged_heap::tagged_heap(heap* heap, memory_size block_size, s32 block_count)
            :m_block_size(block_size)
            ,m_block_count(block_count)
        {
            m_blocks.reserve(block_count);

            for (s32 i = 0; i < block_count; ++i)
            {
                m_blocks.push_back(tagged_heap_block(heap, block_size));
            }
        }

        //-------------------------------------------------------------------------
        tagged_heap::~tagged_heap()
        {
            // No need to free m_base_memory here because its lifetime is managed by the heap.
        }

        //-------------------------------------------------------------------------
        void* tagged_heap::allocate(u32 tag, memory_size size) noexcept
        {
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
            for (auto& block : m_blocks)
            {
                if (block.tag() == tag)
                {
                    block.deallocate(ptr);
                }
            }
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

            m_base_memory = nullptr;
        }

        //-------------------------------------------------------------------------
        s32 tagged_heap::block_count() const
        {
            return m_block_count;
        }

        //-------------------------------------------------------------------------
        u32 tagged_heap::block_tag(s32 block_index) const
        {
            assert(block_index < m_block_count);

            return m_blocks[block_index].tag();
        }

        //-------------------------------------------------------------------------
        memory_size tagged_heap::current_size(s32 block_index) const
        {
            assert(block_index < m_block_count);

            return m_blocks[block_index].current_size();
        }

        //-------------------------------------------------------------------------
        memory_size tagged_heap::total_size(s32 block_index) const
        {
            assert(block_index < m_block_count);

            return m_blocks[block_index].total_size();
        }

        constexpr memory_size _block_size = 500_kb;
        constexpr s32 _block_count = 15;

        tagged_heap _tagged_heap(get_heap(), _block_size, _block_count);

        //-------------------------------------------------------------------------
        tagged_heap* get_tagged_heap()
        {
            return &_tagged_heap;
        }
    }
}