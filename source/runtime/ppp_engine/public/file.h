#pragma once

#include <vector>
#include <string>

namespace ppp
{
    namespace file
    {
        std::vector<std::byte> load_binary_file(std::string_view file_path);
        std::vector<std::byte> load_text_file(std::string_view file_path);
    }
}