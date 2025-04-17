/**
 * @file input_output.h
 * @brief File loading utilities.
 */
#pragma once

#include <vector>
#include <string>

namespace ppp
{
    /**
     * @brief Load the contents of a binary file into a byte vector.
     * @param file_path Path to the binary file.
     * @return Vector of bytes representing file contents.
     */
    std::vector<std::byte> load_binary_file(std::string_view file_path);

    /**
     * @brief Load the contents of a text file into a byte vector.
     * @param file_path Path to the text file.
     * @return Vector of bytes representing file contents.
     */
    std::vector<std::byte> load_text_file(std::string_view file_path);
}