#include "memory/memory_manager.h"

namespace ppp
{
    namespace memory
    {
        constexpr memory_size   _global_memory_size = 10_mb;

        constexpr memory_size   _frame_heap_size = 1_mb;

        constexpr memory_size   _tagged_heap_block_size = 500_kb;
        constexpr s32           _tagged_heap_block_count = 15;

        constexpr memory_size   _scratch_heap_size = 1_mb;
        //-------------------------------------------------------------------------
        // Static function that returns the singleton instance
        memory_manager& memory_manager::instance()
        {
            static memory_manager s_instance(_global_memory_size);
            return s_instance;
        }

        //-------------------------------------------------------------------------
        memory_manager::memory_manager(memory_size size)
            : m_heap(size)
            , m_frame_heap(&m_heap, _frame_heap_size)
            , m_tagged_heap(&m_heap, _tagged_heap_block_size, _tagged_heap_block_count)
            , m_scratch_heap(&m_heap, _scratch_heap_size)
        {}

        //-------------------------------------------------------------------------
        memory_manager::~memory_manager()
        {
            m_frame_heap.free();
            m_tagged_heap.free();
            m_scratch_heap.free();

            m_heap.free();
        }

        //-------------------------------------------------------------------------
        heap* memory_manager::get_global_heap()
        {
            return &m_heap;
        }

        //-------------------------------------------------------------------------
        linear_heap* memory_manager::get_frame_heap()
        {
            return &m_frame_heap;
        }
        
        //-------------------------------------------------------------------------
        tagged_heap* memory_manager::get_tagged_heap()
        {
            return &m_tagged_heap;
        }
        
        //-------------------------------------------------------------------------
        circular_heap* memory_manager::get_scratch_heap()
        {
            return &m_scratch_heap;
        }
    }
}