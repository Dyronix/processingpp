#include "string/string_pool.h"

#include "string/string_id.h"

#include "memory/memory_types.h"

#include <functional>
#include <utility>
#include <unordered_map>

namespace ppp
{
    namespace string
    {
        namespace string_pool
        {
            using entry_map = global_hash_map<string_id, string_entry>;

            //-------------------------------------------------------------------------
            entry_map load_entry_map()
            {
                entry_map map;

                string_id entry_id;
                string_entry entry("Invalid string_id");

                map.emplace(std::move(entry_id), std::move(entry));

                return map;
            }

            //-------------------------------------------------------------------------
            entry_map& get_entries()
            {
                static entry_map entries = load_entry_map();
                return entries;
            }

            //-------------------------------------------------------------------------
            string_id store(std::string_view new_characters)
            {
                string_id entry_id = string_id(new_characters);

                auto it = get_entries().find(entry_id);
                if (it != std::cend(get_entries()))
                {
                    assert(std::strcmp(new_characters.data(), it->second.data()) == 0 && "Hash collision");

                    return it->first;
                }

                auto result = get_entries().emplace(std::move(entry_id), new_characters);

                // if and only if the insertion took place 
                if (result.second)
                {
                    // return inserted element
                    return result.first->first;
                }

                assert(false && "This path should never be reached, insertion into the string pool failed somehow.");
                return {};
            }

            //-------------------------------------------------------------------------
            std::string_view resolve(const string_id& entry_id)
            {
                const string_entry& entry = find(entry_id);

                assert(!entry.empty() && "Entry not found");

                return entry;
            }

            //-------------------------------------------------------------------------
            const string_entry& find(const string_id& entry_id)
            {
                auto it = get_entries().find(entry_id);
                if (it == std::cend(get_entries()))
                {
                    it = get_entries().find(string_id::create_invalid());

                    assert(it != std::cend(get_entries()) && "string_id::is_none() not present");
                }

                return it->second;
            }

            //-------------------------------------------------------------------------
            string_id make_and_store(std::string_view characters)
            {
                return store(characters);
            }
        }
    } // namespace string
} // namespace ppp