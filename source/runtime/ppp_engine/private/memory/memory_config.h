#pragma once

#include "memory/memory_size.h"

namespace ppp
{
    namespace memory
    {
        memory_size development_persistant_frame_heap_size();
        memory_size development_persistant_tagged_heap_block_size();
        s32         development_persistant_tagged_heap_block_count();
        memory_size development_persistant_scratch_heap_size();

        memory_size development_staging_frame_heap_size();
        memory_size development_staging_tagged_heap_block_size();
        s32         development_staging_tagged_heap_block_count();
        memory_size development_staging_scratch_heap_size();

        memory_size development_debug_frame_heap_size();
        memory_size development_debug_tagged_heap_block_size();
        s32         development_debug_tagged_heap_block_count();
        memory_size development_debug_scratch_heap_size();

        memory_size shipping_persistant_frame_heap_size();
        memory_size shipping_persistant_tagged_heap_block_size();
        s32         shipping_persistant_tagged_heap_block_count();
        memory_size shipping_persistant_scratch_heap_size();

        memory_size shipping_staging_frame_heap_size();
        memory_size shipping_staging_tagged_heap_block_size();
        s32         shipping_staging_tagged_heap_block_count();
        memory_size shipping_staging_scratch_heap_size();

        memory_size shipping_debug_frame_heap_size();
        memory_size shipping_debug_tagged_heap_block_size();
        s32         shipping_debug_tagged_heap_block_count();
        memory_size shipping_debug_scratch_heap_size();
    }
}