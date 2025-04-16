#include "json.h"
#include "fileio/fileio.h"
#include "fileio/vfs.h"

namespace ppp
{
    json load_text_json(std::string_view path)
    {
        auto data = fileio::read_text_file(path);

        return nlohmann::json::parse(data);
    }

    json load_binary_json(std::string_view path, binary_json_type binary_type)
    {
        auto data = fileio::read_binary_file(path);

        json j;

        switch (binary_type)
        {
        case binary_json_type::MESSAGE_PACK: j = json::from_msgpack(data); break;
        case binary_json_type::CBOR: j = json::from_cbor(data); break;
        case binary_json_type::UBJSON: j = json::from_ubjson(data); break;
        case binary_json_type::BJSON: j = json::from_bson(data); break;
        case binary_json_type::BJDATA: j = json::from_bjdata(data); break;
        }

        return j;
    }

    void save_json(std::string_view path, json obj)
    {
        std::string json_string = obj.dump(4);
        temp_string out_path = vfs::resolve_path(path);

        std::ofstream out_file(out_path.data(), std::ios::out | std::ios::trunc);
        if (out_file.is_open())
        {
            log::info("Writing file to path: {}", out_path.data());
            out_file << json_string;
            out_file.close();
        }
        else
        {
            log::error("Error opening file for writing: {}", out_path);
        }
    }
}