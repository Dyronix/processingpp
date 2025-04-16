#include "io.h"
#include "fileio/fileio.h"

namespace ppp
{
    std::vector<std::byte> load_binary_file(std::string_view file_path)
    {
        auto binary_content = fileio::read_binary_file(file_path);

        std::vector<std::byte> output;

        output.resize(sizeof(binary_content[0]) * binary_content.size());

        memcpy(output.data(), binary_content.data(), binary_content.size());

        return output;
    }

    std::vector<std::byte> load_text_file(std::string_view file_path)
    {
        auto text_content = fileio::read_text_file(file_path);

        std::vector<std::byte> output;

        output.resize(sizeof(text_content[0]) * text_content.size());

        memcpy(output.data(), text_content.data(), text_content.size());

        return output;
    }
}