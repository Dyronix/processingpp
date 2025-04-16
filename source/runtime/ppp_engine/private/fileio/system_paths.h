#pragma once

#include "memory/memory_types.h"

namespace ppp
{
    namespace vfs
    {
        std::string_view executable_filepath();
        std::string_view executable_directory();
    }
}