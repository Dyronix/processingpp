#pragma once

#include <string>
#include <vector>

namespace ppp
{
    namespace string
    {
        std::string string_replace(const std::string& subject, const std::string& search, const std::string& replace);

        std::vector<std::string> split_string(const std::string& str, const std::string& delimiters);
    }
}