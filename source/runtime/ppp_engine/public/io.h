#pragma once

#include <vector>
#include <string>

namespace ppp
{
    namespace io
    {
        // Loads a binary file
        std::vector<std::byte> load_binary_file(std::string_view file_path);
        // Loads a text file
        std::vector<std::byte> load_text_file(std::string_view file_path);
    }
}