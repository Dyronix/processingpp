#pragma once

#include <string>

namespace ppp
{
    namespace string
    {
        class string_entry;
        class string_id;

        namespace string_pool
        {
            std::string_view resolve(const string_id& entryID);

            const string_entry& find(const string_id& entryID);

            string_id make_and_store(std::string_view characters);
        }
    }
}