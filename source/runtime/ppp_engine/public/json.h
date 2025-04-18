/**
 * @file json.h
 * @brief JSON utilities using nlohmann::json.
 */
#pragma once

#include <nlohmann/json.hpp>

namespace ppp
{
    using json = nlohmann::json;

    /**
     * @brief Supported binary JSON formats for serialization/deserialization.
     */
    enum class binary_json_type : std::uint8_t
    {
        MESSAGE_PACK,
        CBOR,        
        UBJSON,      
        BJSON,       
        BJDATA       
    };

    /**
     * @brief Load JSON data from a text file.
     * @param path Filesystem path to the JSON text file.
     * @return Parsed json object.
     */
    json load_text_json(std::string_view path);

    /**
     * @brief Load JSON data from a binary file of specified format.
     * @param path Filesystem path to the binary JSON file.
     * @param binary_type Format of the binary JSON (default: BJSON).
     * @return Parsed json object.
     */
    json load_binary_json(std::string_view path, binary_json_type binary_type = binary_json_type::BJSON);

    /**
     * @brief Save a json object to a text file with pretty printing.
     * @param path Filesystem path where the JSON will be written.
     * @param obj JSON object to serialize and save.
     */
    void save_json(std::string_view path, json obj);
}