#pragma once

#include "util/types.h"

#include "memory/allocators/heap_allocator_deleter.h"

#include <memory>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        template <typename TTaggedHeapContainer,
            typename TMemoryPolicy = default_tagged_memory_policy_t<TTaggedHeapContainer>,
            typename u32 tag = tags::default_memory_tag_v<TTaggedHeapContainer>,
            typename... Args>
        std::unique_ptr<TTaggedHeapContainer, allocator_deleter<TTaggedHeapContainer, typename TTaggedHeapContainer::allocator_type>> tagged_placement_new(Args&&... args)
        {
            auto heap = TMemoryPolicy::get_heap();

            void* p = heap->allocate(tag, sizeof(TTaggedHeapContainer));

            TTaggedHeapContainer* ptr = new (p) TTaggedHeapContainer(std::forward<Args>(args)...);

            return std::unique_ptr<TTaggedHeapContainer, allocator_deleter<TTaggedHeapContainer, typename TTaggedHeapContainer::allocator_type>>(ptr);
        }

        //-------------------------------------------------------------------------
        template <typename TGenericHeapContainer,
            typename TMemoryPolicy = default_global_memory_policy_t<TGenericHeapContainer>,
            typename... Args>
        std::unique_ptr<TGenericHeapContainer, allocator_deleter<TGenericHeapContainer, typename TGenericHeapContainer::allocator_type>> placement_new(Args&&... args)
        {
            auto heap = TMemoryPolicy::get_heap();

            void* p = heap->allocate(sizeof(TGenericHeapContainer));

            TGenericHeapContainer* ptr = new (p) TGenericHeapContainer(std::forward<Args>(args)...);

            return std::unique_ptr<TGenericHeapContainer, allocator_deleter<TGenericHeapContainer, typename TGenericHeapContainer::allocator_type>>(ptr);
        }
    }
}