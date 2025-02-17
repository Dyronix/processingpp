#pragma once

#include <memory/memory_types.h>

#include <string>

namespace ppp
{
    namespace string
    {
        using string_entry = global_string;

        class string_id;

        namespace string_pool
        {
            std::string_view resolve(const string_id& entryID);

            const string_entry& find(const string_id& entryID);

            string_id make_and_store(std::string_view characters);
        }
    }
}