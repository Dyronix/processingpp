#include "memory/heap.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        heap::heap(memory_size size)
            :m_base_memory(nullptr)
            ,m_offset(0)
        {
            m_base_memory = static_cast<u8*>(std::malloc(size.size_in_bytes()));

            std::memset(m_base_memory, 0, size.size_in_bytes());
        }

        //-------------------------------------------------------------------------
        heap::~heap()
        {
            if (m_base_memory != nullptr)
            {
                std::free(m_base_memory);
            }
        }

        //-------------------------------------------------------------------------
        void* heap::allocate(memory_size size)
        {
            void* memory = m_base_memory + m_offset;

            m_offset += size.size_in_bytes();

            return memory;
        }

        //-------------------------------------------------------------------------
        void heap::free()
        {
            std::free(m_base_memory);

            m_base_memory = nullptr;
            m_offset = 0;
        }

        //-------------------------------------------------------------------------
        heap* get_heap()
        {
            constexpr memory_size block_size = 10_mb;

            static heap s_heap(block_size);

            return &s_heap;
        }
    }
}