#pragma once

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace fileio
    {
        void add_wildcard(const std::string& wildcard, const std::string& value);
        std::string resolve_path(const std::string& filename);

        bool exists(const std::string& filename);

        blob read_binary_file(const std::string& filename);
        std::string read_text_file(const std::string& filename);
    }
}