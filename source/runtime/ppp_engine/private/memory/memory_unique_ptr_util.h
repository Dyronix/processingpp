#pragma once

#include "memory/allocators/heap_allocator_deleter.h"

#include <memory>

namespace ppp
{
    namespace memory
    {
        //--------------------------------------------------------------
        // Factory function to create an object using the Allocator and wrap it in a std::unique_ptr with a custom deleter.
        template <typename T, typename Allocator, typename... Args>
        std::unique_ptr<T, allocator_deleter<T, Allocator>> make_unique(Args&&... args)
        {
            Allocator alloc;
            T* raw_ptr = alloc.allocate(1);
            new (raw_ptr) T(std::forward<Args>(args)...);
            return std::unique_ptr<T, allocator_deleter<T, Allocator>>(raw_ptr, allocator_deleter<T, Allocator>{});
        }
    }
}