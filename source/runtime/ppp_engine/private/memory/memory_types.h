#pragma once

#include "memory/allocators/scratch_heap_allocator.h"
#include "memory/allocators/frame_heap_allocator.h"
#include "memory/allocators/tagged_heap_allocator.h"

#include "memory/memory_tags.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <string>
#include <map>

namespace ppp
{
    // Vectors
    // Type alias for a vector that uses the 'geometry' tag
    template <typename T>
    using geometry_vector = std::vector<T, memory::tagged_heap_allocator<T, memory::tags::geometry>>;
    // Type alias for a vector that uses the 'graphics' tag
    template <typename T>
    using graphics_vector = std::vector<T, memory::tagged_heap_allocator<T, memory::tags::graphics>>;
    // Type alias for a vector that uses the 'fileio' tag
    template <typename T>
    using fileio_vector = std::vector<T, memory::tagged_heap_allocator<T, memory::tags::fileio>>;
    // Type alias for a vector that uses the 'resources' tag
    template <typename T>
    using resources_vector = std::vector<T, memory::tagged_heap_allocator<T, memory::tags::resources>>;
    // Type alias for a vector that uses the 'global' tag
    template <typename T>
    using global_vector = std::vector<T, memory::tagged_heap_allocator<T, memory::tags::global>>;
    // Type alias for a vector that uses the 'init' tag
    template <typename T>
    using init_vector = std::vector<T, memory::tagged_heap_allocator<T, memory::tags::initialization>>;

    // Type alias for a vector that uses the frame allocator
    template <typename T>
    using temp_vector = std::vector<T, memory::frame_heap_allocator<T>>;

    // Type alias for a vector that uses the 'geometry' tag
    using geometry_blob = std::vector<std::byte, memory::tagged_heap_allocator<std::byte, memory::tags::geometry>>;
    // Type alias for a vector that uses the 'graphics' tag
    using graphics_blob = std::vector<std::byte, memory::tagged_heap_allocator<std::byte, memory::tags::graphics>>;
    // Type alias for a vector that uses the 'fileio' tag
    using fileio_blob = std::vector<std::byte, memory::tagged_heap_allocator<std::byte, memory::tags::fileio>>;
    // Type alias for a vector that uses the 'resources' tag
    using resources_blob = std::vector<std::byte, memory::tagged_heap_allocator<std::byte, memory::tags::resources>>;
    // Type alias for a vector that uses the 'global' tag
    using global_blob = std::vector<std::byte, memory::tagged_heap_allocator<std::byte, memory::tags::global>>;
    // Type alias for a vector that uses the 'init' tag
    using init_blob = std::vector<std::byte, memory::tagged_heap_allocator<std::byte, memory::tags::initialization>>;

    // Type alias for a vector that uses the frame allocator
    using temp_blob = std::vector<std::byte, memory::frame_heap_allocator<std::byte>>;

    // Hash maps
    // Type alias for a unordered_map that uses the 'geometry' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using geometry_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::geometry>>;
    // Type alias for a unordered_map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using graphics_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::graphics>>;
    // Type alias for a unordered_map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using fileio_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::fileio>>;
    // Type alias for a unordered_map that uses the 'resources' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using resources_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::resources>>;
    // Type alias for a unordered_map that uses the 'global' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using global_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::global>>;
    // Type alias for a unordered_map that uses the 'init' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using init_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::initialization>>;

    // Type alias for a unordered_map that uses the frame allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using temp_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::frame_heap_allocator<std::pair<const TKey, TValue>>>;

    // Maps
    // Type alias for a map that uses the 'geometry' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using geometry_map = std::map<TKey, TValue, TPr, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::geometry>>;
    // Type alias for a map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using graphics_map = std::map<TKey, TValue, TPr, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::graphics>>;
    // Type alias for a map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using fileio_map = std::map<TKey, TValue, TPr, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::fileio>>;
    // Type alias for a map that uses the 'resources' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using resources_map = std::map<TKey, TValue, TPr, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::resources>>;
    // Type alias for a map that uses the 'global' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using global_map = std::map<TKey, TValue, TPr, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::global>>;
    // Type alias for a map that uses the 'init' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using init_map = std::map<TKey, TValue, TPr, memory::tagged_heap_allocator<std::pair<const TKey, TValue>, memory::tags::initialization>>;

    // Type alias for a map that uses the frame allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using temp_map = std::map<TKey, TValue, TPr, memory::frame_heap_allocator<std::pair<const TKey, TValue>>>;

    // Sets
    // Type alias for a unordered_set that uses the 'geometry' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using geometry_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<TKey, memory::tags::geometry>>;
    // Type alias for a unordered_set that uses the 'graphics' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using graphics_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<TKey, memory::tags::graphics>>;
    // Type alias for a unordered_set that uses the 'filio' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using fileio_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<TKey, memory::tags::fileio>>;
    // Type alias for a unordered_set that uses the 'resources' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using resources_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<TKey, memory::tags::resources>>;
    // Type alias for a unordered_set that uses the 'global' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using global_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<TKey, memory::tags::global>>;
    // Type alias for a unordered_set that uses the 'init' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using init_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::tagged_heap_allocator<TKey, memory::tags::initialization>>;

    // Type alias for a unordered_set that uses the frame allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using temp_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::frame_heap_allocator<TKey>>;

    // String Streams
    // Type alias for a basic_ostringstream that uses the 'geometry' tag
    using geometry_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_ostringstream that uses the 'resources' tag
    using resources_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::global>>;
    // Type alias for a basic_ostringstream that uses the 'init' tag
    using init_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::initialization>>;

    // Type alias for a basic_ostringstream that uses the frame allocator
    using temp_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::frame_heap_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'geometry' tag
    using geometry_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_ostringstream that uses the 'resources' tag
    using resources_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::global>>;
    // Type alias for a basic_ostringstream that uses the 'init' tag
    using init_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::initialization>>;

    // Type alias for a basic_istringstream that uses the frame allocator
    using temp_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::frame_heap_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'geometry' tag
    using geometry_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_ostringstream that uses the 'resources' tag
    using resources_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::global>>;
    // Type alias for a basic_ostringstream that uses the 'init' tag
    using init_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::initialization>>;

    // Type alias for a basic_stringstream that uses the frame allocator
    using temp_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::frame_heap_allocator<char>>;

    // Strings
    // Type alias for a basic_string that uses the 'geometry' tag
    using geometry_string = std::basic_string<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::geometry>>;
    // Type alias for a basic_string that uses the 'graphics' tag
    using graphics_string = std::basic_string<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::graphics>>;
    // Type alias for a basic_string that uses the 'fileio' tag
    using fileio_string = std::basic_string<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::fileio>>;
    // Type alias for a basic_string that uses the 'resources' tag
    using resources_string = std::basic_string<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::resources>>;
    // Type alias for a basic_string that uses the 'global' tag
    using global_string = std::basic_string<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::global>>;
    // Type alias for a basic_string that uses the 'init' tag
    using init_string = std::basic_string<char, std::char_traits<char>, memory::tagged_heap_allocator<char, memory::tags::initialization>>;

    // Type alias for a basic_string that uses the frame allocator
    using temp_string = std::basic_string<char, std::char_traits<char>, memory::frame_heap_allocator<char>>;
}