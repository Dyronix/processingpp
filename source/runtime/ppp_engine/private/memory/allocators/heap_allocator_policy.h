#pragma once

namespace ppp
{
    namespace memory
    {
        class iheap;
        class tagged_heap;
        
        //-----------------------------------------------------------------------------
        // Define memory policies for persistent memory.
        //-----------------------------------------------------------------------------

        // Global persistent policy.
        struct persistent_global_policy 
        {
            static iheap* get_heap();
        };

        // Frame persistent policy.
        struct persistent_frame_policy 
        {
            static iheap* get_heap();
        };

        // Scratch persistent policy.
        struct persistent_scratch_policy 
        {
            static iheap* get_heap();
        };

        // Tagged persistent policy.
        struct persistent_tagged_policy
        {
            static tagged_heap* get_heap();
        };

        //-----------------------------------------------------------------------------
        // Define memory policies for staging memory.
        //-----------------------------------------------------------------------------

        // Global staging policy.
        struct staging_global_policy 
        {
            static iheap* get_heap();
        };

        // Frame staging policy.
        struct staging_frame_policy 
        {
            static iheap* get_heap();
        };

        // Scratch staging policy.
        struct staging_scratch_policy
        {
            static iheap* get_heap();
        };

        // Tagged staging policy.
        struct staging_tagged_policy
        {
            static tagged_heap* get_heap();
        };

        //-----------------------------------------------------------------------------
        // Define memory policies for debug memory.
        //-----------------------------------------------------------------------------

        // Global debug policy.
        struct debug_global_policy
        {
            static iheap* get_heap();
        };

        // Frame debug policy.
        struct debug_frame_policy
        {
            static iheap* get_heap();
        };

        // Scratch debug policy.
        struct debug_scratch_policy
        {
            static iheap* get_heap();
        };

        // Tagged debug policy.
        struct debug_tagged_policy
        {
            static tagged_heap* get_heap();
        };
    }
}