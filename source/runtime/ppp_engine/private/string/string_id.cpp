#include "string/string_id.h"
#include "string/string_pool.h"

#include "util/log.h"

namespace ppp
{
    namespace string
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            string_id make_and_store(std::string_view characters)
            {
                if (characters.empty())
                {
                    log::warn("Trying to create a string_id with an empty string, this is not allowed.");
                    return string_id::create_invalid();
                }

                return string_pool::make_and_store(characters);
            }
        } // namespace internal

        //-------------------------------------------------------------------------
        string_id store_sid(std::string_view characters)
        {
            return string_pool::make_and_store(characters);
        }
        //-------------------------------------------------------------------------
        std::string_view restore_sid(const string_id& sid)
        {
            return string_pool::resolve(sid);
        }

        //-------------------------------------------------------------------------
        bool operator==(std::string_view s, const string_id& sid)
        {
            return string_id(s) == sid;
        }
        //-------------------------------------------------------------------------
        bool operator!=(std::string_view s, const string_id& sid)
        {
            return string_id(s) != sid;
        }
        //-------------------------------------------------------------------------
        bool operator==(const string_id& sid, std::string_view s)
        {
            return s == sid;
        }
        //-------------------------------------------------------------------------
        bool operator!=(const string_id& sid, std::string_view s)
        {
            return s != sid;
        }
    }
}

//-------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const ppp::string::string_id& stringID)
{
    os << ppp::string::restore_sid(stringID);

    return os;
}