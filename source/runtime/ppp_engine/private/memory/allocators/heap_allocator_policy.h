#pragma once

#include "memory/memory_manager.h"

#include <type_traits>

namespace ppp
{
    namespace memory
    {
        //-----------------------------------------------------------------------------
        // Define memory policies for persistent memory.
        //-----------------------------------------------------------------------------

        // Global persistent policy.
        struct persistent_global_policy 
        {
            static iheap* get_heap() 
            {
                return get_memory_manager().get_persistent_region().get_heap();
            }
        };

        // Frame persistent policy.
        struct persistent_frame_policy 
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_persistent_region().get_frame_heap();
            }
        };

        // Scratch persistent policy.
        struct persistent_scratch_policy 
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_persistent_region().get_scratch_heap();
            }
        };

        // Tagged persistent policy.
        struct persistent_tagged_policy
        {
            static tagged_heap* get_heap()
            {
                return get_memory_manager().get_persistent_region().get_tagged_heap();
            }
        };

        //-----------------------------------------------------------------------------
        // Define memory policies for staging memory.
        //-----------------------------------------------------------------------------

        // Global staging policy.
        struct staging_global_policy 
        {
            static iheap* get_heap() 
            {
                return get_memory_manager().get_staging_region().get_heap();
            }
        };

        // Frame staging policy.
        struct staging_frame_policy 
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_staging_region().get_frame_heap();
            }
        };

        // Scratch staging policy.
        struct staging_scratch_policy
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_staging_region().get_scratch_heap();
            }
        };

        // Tagged staging policy.
        struct staging_tagged_policy
        {
            static tagged_heap* get_heap()
            {
                return get_memory_manager().get_staging_region().get_tagged_heap();
            }
        };

        //-----------------------------------------------------------------------------
        // Define memory policies for debug memory.
        //-----------------------------------------------------------------------------

        // Global debug policy.
        struct debug_global_policy
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_debug_region().get_heap();
            }
        };

        // Frame debug policy.
        struct debug_frame_policy
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_debug_region().get_frame_heap();
            }
        };

        // Scratch debug policy.
        struct debug_scratch_policy
        {
            static iheap* get_heap()
            {
                return get_memory_manager().get_debug_region().get_scratch_heap();
            }
        };

        // Tagged debug policy.
        struct debug_tagged_policy
        {
            static tagged_heap* get_heap()
            {
                return get_memory_manager().get_debug_region().get_tagged_heap();
            }
        };

        template<typename T, typename = void>
        struct has_memory_policy : std::false_type {};

        template<typename T>
        struct has_memory_policy<T, std::void_t<typename T::memory_policy>> : std::true_type {};

        template<typename T>
        static constexpr bool has_policy_v = has_memory_policy<T>::value;

        template<typename T, typename = void>
        struct has_allocator : std::false_type {};

        template<typename T>
        struct has_allocator<T, std::void_t<typename T::allocator_type>> : std::true_type {};

        template<typename T>
        static constexpr bool has_allocator_v = has_allocator<T>::value;

        template<typename, typename TDefaultPolicy, typename = void>
        struct default_memory_policy
        {
            using memory_policy = TDefaultPolicy;
        };

        template<typename T, typename TDefaultPolicy>
        struct default_memory_policy<T, TDefaultPolicy, std::enable_if_t<has_policy_v<T>>>
        {
            using memory_policy = typename T::memory_policy;
        };

        template<typename T, typename TDefaultPolicy>
        struct default_memory_policy<T, TDefaultPolicy, std::enable_if_t<!has_policy_v<T>&& has_allocator_v<T>>>
        {
            using memory_policy = typename default_memory_policy<typename T::allocator_type, TDefaultPolicy>::memory_policy;
        };

        template<typename T>
        using default_global_memory_policy_t = typename default_memory_policy<T, persistent_global_policy>::memory_policy;

        template<typename T>
        using default_tagged_memory_policy_t = typename default_memory_policy<T, persistent_tagged_policy>::memory_policy;
    }
}