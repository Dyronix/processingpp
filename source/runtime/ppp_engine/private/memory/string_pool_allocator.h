#pragma once

#include "memory/string_pool.h"
#include "memory/memory_size.h"

namespace ppp 
{
    namespace memory 
    {
        template <typename T>
        class string_pool_allocator 
        {
        public:
            using value_type = T;

            //-------------------------------------------------------------------------
            explicit string_pool_allocator() noexcept
                : m_pool(get_string_pool())
            {
            }

            //-------------------------------------------------------------------------
            template <typename U>
            string_pool_allocator(const string_pool_allocator<U>& other) noexcept
                : m_pool(other.m_pool)
            {
            }

            //-------------------------------------------------------------------------
            T* allocate(u64 n) noexcept
            {
                u64 total_size = n * sizeof(T);

                return static_cast<T*>(m_pool->allocate(memory_size(total_size)));
            }

            //-------------------------------------------------------------------------
            void deallocate(T* p, u64 /*n*/) noexcept
            {
                m_pool->deallocate(static_cast<void*>(p));
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
                using other = string_pool_allocator<U>;
            };

        public:
            const string_pool* pool() const noexcept { return m_pool; }

        private:
            // Allow other string_pool_allocator instantiations (with different types) access to m_pool.
            template <typename U>
            friend class string_pool_allocator;

            string_pool* m_pool;
        };

        // Equality comparison operators for the allocator.
        // These compare the underlying memory pool pointers.
        template <typename T, typename U>
        bool operator==(const string_pool_allocator<T>& a, const string_pool_allocator<U>& b) noexcept 
        {
            return a.pool() == b.pool();
        }

        template <typename T, typename U>
        bool operator!=(const string_pool_allocator<T>& a, const string_pool_allocator<U>& b) noexcept 
        {
            return !(a == b);
        }

    } // namespace memory
} // namespace ppp
