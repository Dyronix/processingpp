#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>

namespace ppp
{
    namespace fileio
    {
        void add_wildcard(const pool_string& wildcard, const pool_string& value);

        pool_string resolve_path(std::string_view filename);

        bool exists(std::string_view filename);

        temp_blob read_binary_file(std::string_view filename);
        pool_string read_text_file(std::string_view filename);
    }
}