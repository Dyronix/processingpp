#pragma once

#include "memory/allocators/tagged_heap_allocator.h"

#include <memory>

namespace ppp
{
    //--------------------------------------------------------------
    // Custom deleter that calls the destructor and then deallocates the memory using the provided Allocator.
    template <typename T, typename Allocator = memory::persistent_global_tagged_allocator<T>>
    struct allocator_deleter 
    {
        void operator()(T* ptr) const 
        {
            if (ptr) 
            {
                ptr->~T();
                Allocator alloc;
                alloc.deallocate(ptr, 1);
            }
        }

        // Conversion operator to allow converting to an allocator_deleter for another type U
        // with an arbitrary allocator OtherAllocator. This conversion is only enabled if:
        // 1. T* is convertible to U*, and
        // 2. Allocator is convertible to OtherAllocator.
        template <typename U, typename OtherAllocator>
        operator allocator_deleter<U, OtherAllocator>() const 
        {
            static_assert(std::is_convertible<T*, U*>::value, "Pointer type conversion not allowed");
            static_assert(std::is_convertible<Allocator, OtherAllocator>::value, "Allocator type conversion not allowed");

            return allocator_deleter<U, OtherAllocator>();
        }
    };

    //--------------------------------------------------------------
    // Factory function to create an object using the Allocator and wrap it in a std::unique_ptr with a custom deleter.
    template <typename T, typename Allocator = memory::persistent_global_tagged_allocator<T>, typename... Args>
    std::unique_ptr<T, allocator_deleter<T, Allocator>> make_unique(Args&&... args) 
    {
        Allocator alloc;
        T* raw_ptr = alloc.allocate(1);
        new (raw_ptr) T(std::forward<Args>(args)...);
        return std::unique_ptr<T, allocator_deleter<T, Allocator>>(raw_ptr, allocator_deleter<T, Allocator>{});
    }

    //--------------------------------------------------------------
    // Type aliases for persistent allocators.
    template <typename T>
    using global_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::persistent_global_tagged_allocator<T>>>;

    template <typename T>
    using graphics_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::persistent_graphics_tagged_allocator<T>>>;

    template <typename T>
    using fileio_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::persistent_fileio_tagged_allocator<T>>>;

    // Type aliases for staging allocators.
    template <typename T>
    using staging_global_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::staging_global_tagged_allocator<T>>>;

    template <typename T>
    using staging_graphics_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::staging_graphics_tagged_allocator<T>>>;

    template <typename T>
    using staging_fileio_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::staging_fileio_tagged_allocator<T>>>;

    // Type aliases for debug allocators.
    template <typename T>
    using debug_global_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::debug_global_tagged_allocator<T>>>;

    template <typename T>
    using debug_graphics_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::debug_graphics_tagged_allocator<T>>>;

    template <typename T>
    using debug_fileio_unique_ptr = std::unique_ptr<T, allocator_deleter<T, memory::debug_fileio_tagged_allocator<T>>>;

}