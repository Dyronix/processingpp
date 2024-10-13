#pragma once

#include "util/log.h"
#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        template<typename T>
        class weak_array
        {
        private:
            T* m_ptr;       // Pointer to the first element of the array
            u64 m_size;     // Size of the array

        public:
            weak_array(T* arr_ptr, u64 arr_size)
                : m_ptr(arr_ptr)
                , m_size(arr_size)
            {
            }

            weak_array(std::initializer_list<T> list)
                : m_ptr(const_cast<T*>(list.begin()))
                , m_size(list.size())
            {
            }

            T& operator[](u64 index)
            {
                if (index >= m_size)
                {
                    log::error("Index out of bounds");
                    exit(EXIT_FAILURE);
                }
                return m_ptr[index];
            }

            const T& operator[](u64 index) const
            {
                if (index >= m_size)
                {
                    log::error("Index out of bounds");
                    exit(EXIT_FAILURE);
                }
                return m_ptr[index];
            }

            u64 size() const
            {
                return m_size;
            }

            T* data()
            {
                return m_ptr;
            }

            const T* data() const
            {
                return m_ptr;
            }
        };
    }
}