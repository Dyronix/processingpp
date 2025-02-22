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
                auto& entries = get_entries();

                // We create a string id from the incoming characters here 
                // to check if the entry is already available
                string_id temp_id = string_id(new_characters);

                auto it = entries.find(temp_id);
                if (it != std::cend(entries))
                {
                    assert(std::strcmp(new_characters.data(), it->second.data()) == 0 && "Hash collision");

                    return it->first;
                }

                // We need to store the actual charaters in a new string here to make sure the
                // string is retained in memory
                string_entry entry(new_characters);
                // We store a view to the string entry we created to pass to a new string_id
                // to make sure the "debug string" remains valid within the string_id
                // as this one is actually stored within the string pool
                std::string_view sv_entry = entry;

                auto result = entries.emplace(string_id(sv_entry), std::move(entry));

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