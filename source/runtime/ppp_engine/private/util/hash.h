#pragma once

#include "util/types.h"

namespace ppp
{
    namespace utils
    {
        template <typename T>
        inline u64 hash_combine(u64 seed, const T& v)
        {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            return seed;
        }
    }
}