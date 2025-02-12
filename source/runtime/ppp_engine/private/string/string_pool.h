#pragma once

#include <string>

namespace ppp
{
    namespace string
    {
        class StringEntry;
        class StringID;

        namespace string_pool
        {
            std::string_view resolve(const StringID& entryID);

            const StringEntry& find(const StringID& entryID);

            StringID make_and_store(std::string_view characters);
        }
    }
}