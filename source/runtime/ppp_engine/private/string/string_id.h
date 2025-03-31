#pragma once

#include "util/types.h"

#include <functional>
#include <ostream>
#include <cstddef>

namespace ppp
{
    namespace string
    {
        class string_id;

        /**
        * @brief Stores a string and returns its associated string_id.
        * @param characters The string to be stored.
        * @return The generated string_id.
        */
        string_id store_sid(std::string_view characters);

        /**
         * @brief Restores the original string from a string_id.
         * @param sid The string_id to restore.
         * @return The original string view.
         */
        std::string_view restore_sid(const string_id& sid);

        /**
         * The process of generating a standalone string_id object lacks a hash collision check,
         *   which implies that in extremely rare scenarios, different strings may exist with the same hash value.
         *
         * With CRC-32, which generates a 32-bit hash value, the theoretical probability of a collision occurring randomly is approximately 1 in 4.3 billion.
         * However, the likelihood can vary based on the data being hashed.
         *
         * If issues arise when using StringIDs, one possible explanation could be a hash collision.
         *
         * Hash collisions can only be prevented through the utilization of runtime generated StringIDs,
         *   which is achieved by employing the "ppp::string::store_sid" function.
         */
        class string_id
        {
        public:
            //-------------------------------------------------------------------------
            /**
             * Create an invalid string_id.
             */
            static string_id create_invalid()
            {
                return string_id();
            }

            //-------------------------------------------------------------------------
            /**
             * Create an None string_id.
             */
            string_id()
                : m_comparison_hash(0)
#ifdef _DEBUG
                , m_value("invalid string id")
#endif
            {
            }

            //-------------------------------------------------------------------------
            /**
             * Create an string_id with characters.
             */
            explicit string_id(std::string_view string_view)
                : m_comparison_hash(std::hash<std::string_view> {}(string_view))
#ifdef _DEBUG
                , m_value(string_view)
#endif
            {
            }

            //-------------------------------------------------------------------------
            /**
             * Retrieve the hashed value as u32
             */
            operator u64() const // NOLINT(google-explicit-constructor)
            {
                return m_comparison_hash;
            }
            //-------------------------------------------------------------------------
            /**
             * Is the hash value valid
             */
            explicit operator bool() const
            {
                return is_none() == false; // NOLINT(readability-simplify-boolean-expr)
            }

            //-------------------------------------------------------------------------
            bool operator<(const string_id& rhs) const
            {
                return m_comparison_hash < rhs.m_comparison_hash;
            }
            //-------------------------------------------------------------------------
            bool operator>(const string_id& rhs) const
            {
                return rhs.m_comparison_hash < m_comparison_hash;
            }

            //-------------------------------------------------------------------------
            bool operator==(const string_id& other) const
            {
                return m_comparison_hash == other.m_comparison_hash;
            }
            //-------------------------------------------------------------------------
            bool operator!=(const string_id& other) const
            {
                return !(*this == other);
            }

            //-------------------------------------------------------------------------
            /** True for string_id() and string_id("Invalid string_id") */
            bool is_none() const
            {
                return m_comparison_hash == s_none_state_hash_val;
            }

            //-------------------------------------------------------------------------
            /**
             * Retrieve the hashed value
             */
            u64 value() const
            {
                return m_comparison_hash;
            }

            //-------------------------------------------------------------------------
            /**
            * Retrieve the string value
            */
            std::string_view str() const
            {
                return restore_sid(*this);
            }

            //-------------------------------------------------------------------------
            /**
            * Retrieve the string value in a c-style string
            */
            const char* c_str() const
            {
                auto view = str();

                return view.data();
            }

        private:
            /** Hash for invalid string_id into string pool */
            static const u64 s_none_state_hash_val = 0;

            /** Hash into the string_id hash table */
            u64 m_comparison_hash;

#ifdef _DEBUG
            std::string_view m_value;
#endif
        };

        bool operator==(std::string_view s, const string_id& sid);
        bool operator!=(std::string_view s, const string_id& sid);
        bool operator==(const string_id& sid, std::string_view s);
        bool operator!=(const string_id& sid, std::string_view s);
    }
}

//-------------------------------------------------------------------------
// User-defined literal operator to create a string_id from a string literal.
inline ppp::string::string_id operator""_sid(const char* string, size_t size)
{
    return ppp::string::store_sid(std::string_view(string, static_cast<u32>(size))); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

//-------------------------------------------------------------------------
// Outputs the string associated with a string_id. 
std::ostream& operator<<(std::ostream& os, const ppp::string::string_id& sid);

//-------------------------------------------------------------------------
// Custom specialization of std::hash for string_id to allow using it in unordered containers
namespace std
{
    //-------------------------------------------------------------------------
    template <>
    struct hash<ppp::string::string_id>
    {
        u64 operator()(const ppp::string::string_id& s) const noexcept
        {
            return s.value();
        }
    };
} // namespace std