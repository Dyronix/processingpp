#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>

namespace ppp
{
    namespace fileio
    {
        fileio_blob read_binary_file(std::string_view filename);
        fileio_string read_text_file(std::string_view filename);
    }
}