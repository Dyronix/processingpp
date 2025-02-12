#pragma once

#include "memory/tagged_heap.h"
#include "memory/tagged_heap_tags.h"

namespace ppp
{
    namespace memory
    {
        template<typename T, u32 allocator_tag> 
        class tagged_allocator
        {
        public:
            using value_type = T;

            //-------------------------------------------------------------------------
            tagged_allocator()
                :m_tag(allocator_tag)
            {}

            //-------------------------------------------------------------------------
            template<class U, u32 other_allocator_tag>
            tagged_allocator(const tagged_allocator <U, other_allocator_tag>& other) noexcept
            {
                m_tag = other.tag();
            }

            //-------------------------------------------------------------------------
            value_type* allocate(u64 n) noexcept
            {
                return static_cast<value_type*>(get_tagged_heap()->allocate(m_tag, n * sizeof(value_type)));
            }

            //-------------------------------------------------------------------------
            void deallocate(value_type* p, u64 /*n*/) noexcept
            {
                get_tagged_heap()->deallocate(allocator_tag, p);
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
                using other = tagged_allocator<U, allocator_tag>;
            };

        public:
            u32 tag() const noexcept { return m_tag; }

        private:
            u32 m_tag;
        };

        //-------------------------------------------------------------------------
        template<class T, u32 allocator_tag, class U, u32 other_allocator_tag>
        bool operator==(const tagged_allocator <T, allocator_tag>& lhs, const tagged_allocator <U, other_allocator_tag>& rhs) { return lhs.tag() == rhs.tag(); }
        //-------------------------------------------------------------------------
        template<class T, u32 allocator_tag, class U, u32 other_allocator_tag>
        bool operator!=(const tagged_allocator <T, allocator_tag>& lhs, const tagged_allocator <U, other_allocator_tag>& rhs) { return lhs.tag() != rhs.tag(); }
    }
}