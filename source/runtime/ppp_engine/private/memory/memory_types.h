#pragma once

#include "memory/scratch_pool_allocator.h"
#include "memory/string_pool_allocator.h"
#include "memory/tagged_allocator.h"
#include "memory/tagged_heap_tags.h"

#include <unordered_map>
#include <unordered_set>
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

    // Type alias for a vector that uses the scratch memory pool
    template <typename T>
    using temp_vector = std::vector<T, memory::scratch_pool_allocator<T>>;

    // Type alias for a vector that uses the 'geometry' tag
    using geometry_blob = std::vector<std::byte, memory::tagged_allocator<std::byte, memory::tags::geometry>>;
    // Type alias for a vector that uses the 'graphics' tag
    using graphics_blob = std::vector<std::byte, memory::tagged_allocator<std::byte, memory::tags::graphics>>;
    // Type alias for a vector that uses the 'frame' tag
    using frame_blob = std::vector<std::byte, memory::tagged_allocator<std::byte, memory::tags::frame>>;
    // Type alias for a vector that uses the 'fileio' tag
    using fileio_blob = std::vector<std::byte, memory::tagged_allocator<std::byte, memory::tags::fileio>>;
    // Type alias for a vector that uses the 'resources' tag
    using resources_blob = std::vector<std::byte, memory::tagged_allocator<std::byte, memory::tags::resources>>;
    // Type alias for a vector that uses the 'global' tag
    using global_blob = std::vector<std::byte, memory::tagged_allocator<std::byte, memory::tags::global>>;

    // Type alias for a vector that uses the scratch memory pool
    using temp_blob = std::vector<std::byte, memory::scratch_pool_allocator<std::byte>>;

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

    // Type alias for a unordered_map that uses the scratch memory pool
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using temp_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::scratch_pool_allocator<std::pair<const TKey, TValue>>>;

    // Sets
    // Type alias for a unordered_set that uses the 'geometry' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using geometry_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_allocator<TKey, memory::tags::geometry>>;
    // Type alias for a unordered_set that uses the 'graphics' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using graphics_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_allocator<TKey, memory::tags::graphics>>;
    // Type alias for a unordered_set that uses the 'frame' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using frame_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_allocator<TKey, memory::tags::frame>>;
    // Type alias for a unordered_set that uses the 'filio' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using fileio_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_allocator<TKey, memory::tags::fileio>>;
    // Type alias for a unordered_set that uses the 'resources' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using resources_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_allocator<TKey, memory::tags::resources>>;
    // Type alias for a unordered_set that uses the 'global' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using global_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_allocator<TKey, memory::tags::global>>;

    // Type alias for a unordered_set that uses the scratch memory pool
    template<typename TKey, typename THasher = std::hash<TKey>>
    using temp_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::scratch_pool_allocator<TKey>>;

    // String Streams
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

    // Type alias for a basic_ostringstream that uses the string pool
    using pool_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::string_pool_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'geometry' tag
    using geometry_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_ostringstream that uses the 'frame' tag
    using frame_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::frame>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_ostringstream that uses the 'resources' tag
    using resources_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::global>>;

    // Type alias for a basic_istringstream that uses the string pool
    using pool_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::string_pool_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'geometry' tag
    using geometry_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_ostringstream that uses the 'frame' tag
    using frame_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::frame>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_ostringstream that uses the 'resources' tag
    using resources_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_allocator<char, memory::tags::global>>;

    // Type alias for a basic_ostringstream that uses the string pool
    using pool_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::string_pool_allocator<char>>;

    // Strings
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

    // Type alias for a basic_string that uses the string pool
    using pool_string = std::basic_string<char, std::char_traits<char>, memory::string_pool_allocator<char>>;
}