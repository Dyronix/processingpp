#pragma once

#include "memory/tagged_allocator.h"
#include "memory/tagged_heap_tags.h"

#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>

namespace ppp
{
    // Vectors
    // Type alias for a vector that uses the 'geometry' tag
    template <typename T>
    using geometry_vector = std::vector<T, memory::tagged_allocator<T, memory::tags::geometry>>;
    // Type alias for a vector that uses the 'graphics' tag
    template <typename T>
    using graphics_vector = std::vector<T, memory::tagged_allocator<T, memory::tags::graphics>>;
    // Type alias for a vector that uses the 'frame' tag
    template <typename T>
    using frame_vector = std::vector<T, memory::tagged_allocator<T, memory::tags::frame>>;
    // Type alias for a vector that uses the 'fileio' tag
    template <typename T>
    using fileio_vector = std::vector<T, memory::tagged_allocator<T, memory::tags::fileio>>;
    // Type alias for a vector that uses the 'resources' tag
    template <typename T>
    using resources_vector = std::vector<T, memory::tagged_allocator<T, memory::tags::resources>>;
    // Type alias for a vector that uses the 'global' tag
    template <typename T>
    using global_vector = std::vector<T, memory::tagged_allocator<T, memory::tags::global>>;

    // Hash maps
    // Type alias for a unordered_map that uses the 'geometry' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using geometry_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_allocator<std::pair<const TKey, TValue>, memory::tags::geometry>>;
    // Type alias for a unordered_map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using graphics_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_allocator<std::pair<const TKey, TValue>, memory::tags::graphics>>;
    // Type alias for a unordered_map that uses the 'frame' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using frame_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_allocator<std::pair<const TKey, TValue>, memory::tags::frame>>;
    // Type alias for a unordered_map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using fileio_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_allocator<std::pair<const TKey, TValue>, memory::tags::fileio>>;
    // Type alias for a unordered_map that uses the 'resources' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using resources_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_allocator<std::pair<const TKey, TValue>, memory::tags::resources>>;
    // Type alias for a unordered_map that uses the 'global' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using global_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_allocator<std::pair<const TKey, TValue>, memory::tags::global>>;

    // Type alias for a basic_ostringstream that uses the 'geometry' tag
    using geometry_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_ostringstream that uses the 'frame' tag
    using frame_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::frame>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_ostringstream that uses the 'resources' tag
    using resources_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::global>>;

    // Type alias for a basic_string that uses the 'geometry' tag
    using geometry_string = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_string that uses the 'graphics' tag
    using graphics_string = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_string that uses the 'frame' tag
    using frame_string = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::frame>>;
    // Type alias for a basic_string that uses the 'fileio' tag
    using fileio_string = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_string that uses the 'resources' tag
    using resources_string = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_string that uses the 'global' tag
    using global_string = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::global>>;
}