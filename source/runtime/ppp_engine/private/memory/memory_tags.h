#pragma once

#include "util/types.h"

namespace ppp
{
    namespace memory
    {
        namespace tags
        {
            static const u32 graphics = BIT(0);
            static const u32 fileio = BIT(1);
            static const u32 global = BIT(2);

            template<typename T, typename = void>
            struct has_memory_tag : std::false_type {};

            template<typename T>
            struct has_memory_tag<T, std::void_t<typename T::memory_tag>> : std::true_type {};

            template<typename T>
            static constexpr bool has_memory_tag_v = has_memory_tag<T>::value;

            template<typename T, typename = void>
            struct has_allocator : std::false_type {};

            template<typename T>
            struct has_allocator<T, std::void_t<typename T::allocator_type>> : std::true_type {};

            template<typename T>
            static constexpr bool has_allocator_v = has_allocator<T>::value;

            template<typename, typename = void>
            struct default_memory_tag
            {
                static constexpr u32 memory_tag = tags::global;
            };

            template<typename T>
            struct default_memory_tag<T, std::enable_if_t<has_memory_tag_v<T>>>
            {
                static constexpr u32 memory_tag = T::memory_tag;
            };

            template<typename T>
            struct default_memory_tag<T, std::enable_if_t<!has_memory_tag_v<T>&& has_allocator_v<T>>>
            {
                static constexpr u32 memory_tag = default_memory_tag<typename T::allocator_type>::memory_tag;
            };

            template<typename T>
            static constexpr u32 default_memory_tag_v = default_memory_tag<T>::memory_tag;
        }
    }
}