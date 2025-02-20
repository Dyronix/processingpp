#pragma once

#include "memory/allocators/heap_allocator_deleter.h"

#include <memory>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        template <typename TTaggedHeapContainer,
            typename TMemoryPolicy = TTaggedHeapContainer::allocator_type::memory_policy,
            typename u32 tag = TTaggedHeapContainer::allocator_type::allocator_tag,
            typename... Args>
        std::unique_ptr<TTaggedHeapContainer, allocator_deleter<TTaggedHeapContainer>> tagged_placement_new(Args&&... args)
        {
            auto heap = TMemoryPolicy::get_heap();

            void* p = heap->allocate(tag, sizeof(TTaggedHeapContainer));

            TTaggedHeapContainer* ptr = new (p) TTaggedHeapContainer(std::forward<Args>(args)...);

            return std::unique_ptr<TTaggedHeapContainer, allocator_deleter<TTaggedHeapContainer>>(ptr);
        }

        //-------------------------------------------------------------------------
        template <typename TGenericHeapContainer,
            typename TMemoryPolicy = TGenericHeapContainer::allocator_type::memory_policy,
            typename... Args>
        std::unique_ptr<TGenericHeapContainer, allocator_deleter<TGenericHeapContainer>> placement_new(Args&&... args)
        {
            auto heap = TMemoryPolicy::get_heap();

            void* p = heap->allocate(sizeof(TGenericHeapContainer));

            TGenericHeapContainer* ptr = new (p) TGenericHeapContainer(std::forward<Args>(args)...);

            return std::unique_ptr<TGenericHeapContainer, allocator_deleter<TGenericHeapContainer>>(ptr);
        }
    }
}