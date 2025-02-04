#include "memory/tagged_heap_block.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        tagged_heap_block::tagged_heap_block(heap* heap, memory_size size)
            :m_tag(0)
            ,m_free_list_heap(heap, size)
        {

        }

        //-------------------------------------------------------------------------
        u8* tagged_heap_block::allocate(s32 tag, memory_size size)
        {
            // Set block tag
            m_tag = tag;

            return static_cast<u8*>(m_free_list_heap.allocate(size));
        }

        //-------------------------------------------------------------------------
        void tagged_heap_block::deallocate(void* ptr)
        {
            m_free_list_heap.deallocate(ptr);

            if (m_free_list_heap.current_size() == 0)
            {
                m_tag = 0;
            }
        }

        //-------------------------------------------------------------------------
        void tagged_heap_block::free()
        {
            m_tag = 0;

            m_free_list_heap.free();
        }
    }
}