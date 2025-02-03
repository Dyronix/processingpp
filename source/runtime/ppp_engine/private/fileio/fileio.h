#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>

namespace ppp
{
    namespace fileio
    {
        void add_wildcard(const fileio_string& wildcard, const fileio_string& value);

        fileio_string resolve_path(std::string_view filename);

        bool exists(std::string_view filename);

        blob read_binary_file(std::string_view filename);
        fileio_string read_text_file(std::string_view filename);
    }
}