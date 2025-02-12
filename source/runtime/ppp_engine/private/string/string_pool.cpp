#include "string/string_pool.h"

#include "string/string_entry.h"
#include "string/string_id.h"

#include <functional>
#include <utility>
#include <unordered_map>

namespace ppp
{
    namespace string
    {
        using entry_map = std::unordered_map<string_id, string_entry>;

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
        string_id store(u64 hash, std::string_view newCharacters)
        {
            string_id entry_id = string_id(hash);

            string_entry entry(newCharacters);
            auto it = get_entries().find(entry_id);
            if (it != std::cend(get_entries()))
            {
                assert(std::strcmp(newCharacters.data(), it->second.characters().data()) == 0 && "Hash collision");

                return it->first;
            }

            auto result = get_entries().emplace(std::move(entry_id), std::move(entry));

            // if and only if the insertion took place 
            if (result.second)
            {
                // return inserted element
                return result.first->first;
            }

            assert(false, "This path should never be reached, insertion into the string pool failed somehow.");
            return {};
        }

        //-------------------------------------------------------------------------
        std::string_view resolve(const string_id& entryID)
        {
            const string_entry& entry = find(entryID);

            assert(entry.is_valid() && "Entry not found");

            return entry.characters();
        }

        //-------------------------------------------------------------------------
        const string_entry& find(const string_id& entryID)
        {
            auto it = get_entries().find(entryID);
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
            return store(std::hash<std::string_view> {}(characters), characters);
        }
    } // namespace string
} // namespace ppp