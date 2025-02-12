#include "memory/linear_heap.h"
#include "memory/heap.h"

#include "util/types.h"
#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        linear_heap::linear_heap(heap* heap, memory_size size)
            :m_total_memory(size)
            ,m_base_memory(nullptr)
            ,m_offset(0)
        {
            m_base_memory = heap->allocate(size);
        }

        //-------------------------------------------------------------------------
        void* linear_heap::allocate(memory_size size) noexcept
        {
            constexpr u64 alignment = alignof(std::max_align_t);

            uptr raw_address = reinterpret_cast<uptr>(m_base_memory) + m_offset;
            uptr aligned_address = align_up(raw_address, alignment);

            // Calculate the new offset after alignment
            m_offset = aligned_address - reinterpret_cast<uptr>(m_base_memory) + size.size_in_bytes();

            // Return the aligned pointer
            return reinterpret_cast<void*>(aligned_address);
        }

        //-------------------------------------------------------------------------
        void linear_heap::deallocate(void* ptr) noexcept
        {
            assert(false && "Linear allocators should free all their allocated memory all at once");
        }

        //-------------------------------------------------------------------------
        void linear_heap::free()
        {
            m_offset = 0;
        }
    }
}