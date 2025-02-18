#pragma once

#include "memory/heaps/tagged_heap.h"
#include "memory/memory_tags.h"
#include "memory/allocators/heap_allocator_policy.h"

namespace ppp
{
    namespace memory
    {
        template<typename T, typename TMemoryPolicy, u32 allocator_tag_v>
        class tagged_heap_allocator
        {
        public:
            using value_type = T;
            using memory_policy = TMemoryPolicy;
            static constexpr u32 allocator_tag = allocator_tag_v;

            //-------------------------------------------------------------------------
            tagged_heap_allocator()
                :m_tag(allocator_tag)
            {
            }

            //-------------------------------------------------------------------------
            template<class U, typename TOtherMemoryPolicy, u32 other_allocator_tag>
            tagged_heap_allocator(const tagged_heap_allocator <U, TOtherMemoryPolicy, other_allocator_tag>& other) noexcept
            {
                m_tag = other.tag();
            }

            //-------------------------------------------------------------------------
            value_type* allocate(u64 n) noexcept
            {
                return static_cast<value_type*>(heap()->allocate(m_tag, n * sizeof(value_type)));
            }

            //-------------------------------------------------------------------------
            void deallocate(value_type* p, u64 /*n*/) noexcept
            {
                // Will be cleared all at once or block by block
            }

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
                using other = tagged_heap_allocator<U, TMemoryPolicy, allocator_tag>;
            };

        public:
            u32 tag() const noexcept { return m_tag; }
            tagged_heap* heap() noexcept { return memory_policy::get_heap(); }

        private:
            u32 m_tag;
        };

        //-----------------------------------------------------------------------------
        // Equality comparison operators.
        //-----------------------------------------------------------------------------

        // For stateless memory policies, all instances using the same tag are equivalent.
        template<class T, typename TMemoryPolicy, u32 allocator_tag, class U, typename TOtherMemoryPolicy, u32 other_allocator_tag>
        bool operator==(const tagged_heap_allocator<T, TMemoryPolicy, allocator_tag>& lhs, const tagged_heap_allocator<U, TOtherMemoryPolicy, other_allocator_tag>& rhs) noexcept
        {
            return lhs.tag() == rhs.tag();
        }

        template<class T, typename TMemoryPolicy, u32 allocator_tag, class U, typename TOtherMemoryPolicy, u32 other_allocator_tag>
        bool operator!=(const tagged_heap_allocator<T, TMemoryPolicy, allocator_tag>& lhs, const tagged_heap_allocator<U, TOtherMemoryPolicy, other_allocator_tag>& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        //-----------------------------------------------------------------------------
        // Type aliases for convenience.
        //-----------------------------------------------------------------------------

        // Allocator using the persistent tagged heap.
        template<typename T>
        using persistent_graphics_tagged_allocator = tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::graphics>;
        template<typename T>
        using persistent_fileio_tagged_allocator = tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::fileio>;
        template<typename T>
        using persistent_global_tagged_allocator = tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::global>;

        // Allocator using the staging tagged heap.
        template<typename T>
        using staging_graphics_tagged_allocator = tagged_heap_allocator<T, staging_tagged_policy, memory::tags::graphics>;
        template<typename T>
        using staging_fileio_tagged_allocator = tagged_heap_allocator<T, staging_tagged_policy, memory::tags::fileio>;
        template<typename T>
        using staging_global_tagged_allocator = tagged_heap_allocator<T, staging_tagged_policy, memory::tags::global>;

        // Allocator using the debug tagged heap.
        template<typename T>
        using debug_graphics_tagged_allocator = tagged_heap_allocator<T, debug_tagged_policy, memory::tags::graphics>;
        template<typename T>
        using debug_fileio_tagged_allocator = tagged_heap_allocator<T, debug_tagged_policy, memory::tags::fileio>;
        template<typename T>
        using debug_global_tagged_allocator = tagged_heap_allocator<T, debug_tagged_policy, memory::tags::global>;
    }
}