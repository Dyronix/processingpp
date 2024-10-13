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
            T* ptr;       // Pointer to the first element of the array
            u64 size;     // Size of the array

        public:
            weak_array(T* arr_ptr, u64 arr_size)
                : ptr(arr_ptr)
                , size(arr_size)
            {
            }

            weak_array(std::initializer_list<T> list)
                : ptr(const_cast<T*>(list.begin()))
                , size(list.size())
            {
            }

            T& operator[](u64 index)
            {
                if (index >= size)
                {
                    log::error("Index out of bounds");
                    exit(EXIT_FAILURE);
                }
                return ptr[index];
            }

            const T& operator[](u64 index) const
            {
                if (index >= size)
                {
                    log::error("Index out of bounds");
                    exit(EXIT_FAILURE);
                }
                return ptr[index];
            }

            u64 size() const
            {
                return size;
            }

            T* data()
            {
                return ptr;
            }

            const T* data() const
            {
                return ptr;
            }
        };
    }
}