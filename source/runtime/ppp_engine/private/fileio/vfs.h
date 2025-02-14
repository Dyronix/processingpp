#pragma once

#include "memory/memory_types.h"

#include "string/string_id.h"

namespace ppp
{
    namespace vfs
    {
        void add_wildcard(string::string_id wildcard, std::string_view value);

        temp_string resolve_path(std::string_view filename);

        bool exists(std::string_view filename);
    }
}