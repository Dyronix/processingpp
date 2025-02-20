#include "memory/allocators/heap_allocator_policy.h"

#include "memory/memory_manager.h"

namespace ppp
{
    namespace memory
    {
        // Global persistent policy.
        iheap* persistent_global_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_heap();
        }

        // Frame persistent policy.
        iheap* persistent_frame_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_frame_heap();
        }

        // Scratch persistent policy.
        iheap* persistent_scratch_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_scratch_heap();
        }

        // Tagged persistent policy.
        tagged_heap* persistent_tagged_policy::get_heap()
        {
            return get_memory_manager().get_persistent_region().get_tagged_heap();
        }

        //-----------------------------------------------------------------------------
        // Define memory policies for staging memory.
        //-----------------------------------------------------------------------------

        // Global staging policy.
        iheap* staging_global_policy::get_heap()
        {
            return get_memory_manager().get_staging_region().get_heap();
        }

        // Frame staging policy.
        iheap* staging_frame_policy::get_heap()
        {
            return get_memory_manager().get_staging_region().get_frame_heap();
        }

        // Scratch staging policy.
        iheap* staging_scratch_policy::get_heap()
        {
            return get_memory_manager().get_staging_region().get_scratch_heap();
        }

        // Tagged staging policy.
        tagged_heap* staging_tagged_policy::get_heap()
        {
            return get_memory_manager().get_staging_region().get_tagged_heap();
        }

        //-----------------------------------------------------------------------------
        // Define memory policies for debug memory.
        //-----------------------------------------------------------------------------

        // Global debug policy.
        iheap* debug_global_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_heap();
        }

        // Frame debug policy.
        iheap* debug_frame_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_frame_heap();
        }

        // Scratch debug policy.
        iheap* debug_scratch_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_scratch_heap();
        }

        // Tagged debug policy.
        tagged_heap* debug_tagged_policy::get_heap()
        {
            return get_memory_manager().get_debug_region().get_tagged_heap();
        }
    }
}