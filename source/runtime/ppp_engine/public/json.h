#pragma once

#include <nlohmann/json.hpp>

namespace ppp
{
    using json = nlohmann::json;
    
    enum class binary_json_type : std::uint8_t
    {
        MESSAGE_PACK,
        CBOR,
        UBJSON,
        BJSON,
        BJDATA
    };

    json load_text_json(std::string_view path);
    json load_binary_json(std::string_view path, binary_json_type binary_type = binary_json_type::BJSON);

    void save_json(std::string_view path, json obj);
}