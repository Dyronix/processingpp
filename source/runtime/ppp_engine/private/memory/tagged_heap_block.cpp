#include "memory/tagged_heap_block.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        tagged_heap_block::tagged_heap_block(memory_size size, u8* memory)
            :m_size(size)
            ,m_tag(0)
            ,m_offset(0)
            ,m_memory(memory)
        {

        }

        //-------------------------------------------------------------------------
        u8* tagged_heap_block::allocate(s32 tag, memory_size size)
        {
            // Set block tag
            m_tag = tag;

            // Allocate memory
            u8* ptr = m_memory + m_offset;

            m_offset += size.size_in_bytes();
            
            return ptr;
        }

        //-------------------------------------------------------------------------
        void tagged_heap_block::free()
        {
            m_tag = 0;
            m_offset = 0;
        }
    }
}