#pragma once

#include <string>

namespace ppp
{
    namespace vfs
    {
        std::string_view executable_filepath();
        std::string_view executable_directory();
    }
}