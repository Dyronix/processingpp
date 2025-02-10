#pragma once

#include "memory/memory_types.h"

namespace ppp
{
    namespace vfs
    {
        void add_wildcard(const pool_string& wildcard, const pool_string& value);

        pool_string resolve_path(std::string_view filename);

        bool exists(std::string_view filename);
    }
}