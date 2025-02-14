#pragma once

#include "memory/memory_size.h"
#include "memory/memory_manager.h"

namespace ppp
{
    namespace memory
    {
        template <typename T>
        class global_heap_allocator
        {
        public:
            using value_type = T;

            //-------------------------------------------------------------------------
            explicit global_heap_allocator() noexcept
            {
            }

            //-------------------------------------------------------------------------
            template <typename U>
            global_heap_allocator(const global_heap_allocator<U>& other) noexcept
            {
                // Nothing to implement
            }

            //-------------------------------------------------------------------------
            T* allocate(u64 n) noexcept
            {
                u64 total_size = n * sizeof(T);

                return static_cast<T*>(heap()->allocate(memory_size(total_size)));
            }

            //-------------------------------------------------------------------------
            void deallocate(T* p, u64 /*n*/) noexcept
            {
                // Will be cleared all at once
            }

            //-------------------------------------------------------------------------
            /*
             * The rebind template is required to allow the STL containers to use
             * this allocator with different types. For example, when a container
             * internally needs an allocator for a type different from T (such as its
             * internal node type), it uses the rebind mechanism to create an allocator
             * instance for that type.
             *
             * Without this template, the standard library's allocator_traits and
             * internal mechanisms (like _Replace_first_parameter) would fail to create
             * an allocator for the required type, resulting in compile-time errors.
             *
             * In short, the rebind mechanism ensures that your allocator is compatible
             * with the full range of operations expected by STL containers.
             */
            template <typename U>
            struct rebind
            {
                using other = global_heap_allocator<U>;
            };

        public:
            iheap* heap() noexcept { return memory_manager::instance().get_global_heap(); }

        private:
            // Allow other global_heap_allocator instantiations (with different types) access to m_heap.
            template <typename U>
            friend class global_heap_allocator;
        };

        // Equality comparison operators for the allocator.
        // These compare the underlying memory pool pointers.
        template <typename T, typename U>
        bool operator==(const global_heap_allocator<T>& a, const global_heap_allocator<U>& b) noexcept
        {
            return a.heap() == b.heap();
        }

        template <typename T, typename U>
        bool operator!=(const global_heap_allocator<T>& a, const global_heap_allocator<U>& b) noexcept
        {
            return !(a == b);
        }

    } // namespace memory
} // namespace ppp
