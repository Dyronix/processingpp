#pragma once

#include "util/log.h"
#include "util/types.h"

#include <vector>
#include <stack>

namespace ppp
{
    template <typename T, u64 initial_size = 64>
    class object_pool;

    template <typename T>
    class pool_handle 
    {
    public:
        pool_handle(object_pool<T>& pool, T& object)
            : pool(pool), 
            , object(object) 
        {}

        // Access the managed object
        T* operator->() { return &object; }
        T& operator*() { return object; }

        // Disable copying to avoid double release
        pool_handle(const pool_handle&) = delete;
        pool_handle& operator=(const pool_handle&) = delete;

        // Enable moving
        pool_handle(pool_handle&& other) noexcept
            : pool(other.pool)
            , object(other.object) 
        {
            other.valid = false;
        }

        pool_handle& operator=(pool_handle&& other) noexcept 
        {
            if (this != &other) 
            {
                release(); // Release the current object

                pool = other.pool;
                object = other.object;
                valid = other.valid;

                other.valid = false;
            }

            return *this;
        }

        // Automatically release on destruction
        ~pool_handle() 
        {
            release();
        }

    private:
        object_pool<T>&     m_pool;
        T&                  m_object;
        bool                m_valid = true;

        void release() 
        {
            if (m_valid) 
            {
                m_pool.release(object);
                m_valid = false;
            }
        }
    };

    template <typename T, u64 initial_size = 64>
    class object_pool
    {
    public:
        object_pool()
        {
            expand_pool(initial_size);
        }

        //-------------------------------------------------------------------------
        // Acquire an object from the pool
        T& acquire()
        {
            if (m_free_indices.empty()) 
            {
                expand_pool(m_objects.size());
            }

            u64 index = m_free_indices.top();
            
            m_free_indices.pop();
            m_active[index] = true;
            
            return m_objects[index];
        }

        //-------------------------------------------------------------------------
        // Release an object back to the pool
        void release(T& object)
        {
            u64 index = &object - m_objects.data();

            if (index >= m_objects.size() || !m_active[index])
            {
                log::("Attempt to release invalid or already released object");
            }

            m_active[index] = false;
            m_free_indices.push(index);
            object.reset(); // Call a reset method to clear instance-specific data
        }

    private:
        std::vector<T>      m_objects;            // Contiguous storage for m_objects
        std::vector<bool>   m_active;          // Tracks m_active/inactive status
        std::stack<u64>     m_free_indices;    // Stack of free indices for reuse

        //-------------------------------------------------------------------------
        void expand_pool(u64 count)
        {
            u64 old_size = m_objects.size();

            m_objects.resize(old_size + count);
            m_active.resize(old_size + count, false);

            for (u64 i = old_size; i < m_objects.size(); ++i)
            {
                m_free_indices.push(i);
            }
        }
    };
}