#pragma once

#include "util/types.h"

#include <functional>
#include <ostream>
#include <cstddef>

namespace ppp
{
    namespace string
    {
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
            static constexpr string_id create_invalid()
            {
                return string_id(s_none_state_hash_val);
            }

            //-------------------------------------------------------------------------
            /**
             * Create an None string_id.
             */
            constexpr string_id()
                : m_comparison_hash(create_invalid())
            {
            }

            //-------------------------------------------------------------------------
            /**
             * Create a runtime generated string_id.
             */
            constexpr explicit string_id(u64 entryID)
                : m_comparison_hash(entryID)
            {
            }

            //-------------------------------------------------------------------------
            /**
             * Create an string_id with characters.
             */
            constexpr explicit string_id(std::string_view stringView)
                : m_comparison_hash(std::hash<std::string_view> {}(stringView))
            {
            }

            //-------------------------------------------------------------------------
            /**
             * Retrieve the hashed value as u32
             */
            constexpr operator u32() const // NOLINT(google-explicit-constructor)
            {
                return static_cast<u32>(m_comparison_hash);
            }
            //-------------------------------------------------------------------------
            /**
             * Is the hash value valid
             */
            constexpr explicit operator bool() const
            {
                return is_none() == false; // NOLINT(readability-simplify-boolean-expr)
            }

            //-------------------------------------------------------------------------
            constexpr bool operator<(const string_id& rhs) const
            {
                return m_comparison_hash < rhs.m_comparison_hash;
            }
            //-------------------------------------------------------------------------
            constexpr bool operator>(const string_id& rhs) const
            {
                return rhs.m_comparison_hash < m_comparison_hash;
            }

            //-------------------------------------------------------------------------
            constexpr bool operator==(const string_id& other) const
            {
                return m_comparison_hash == other.m_comparison_hash;
            }
            //-------------------------------------------------------------------------
            constexpr bool operator!=(const string_id& other) const
            {
                return !(*this == other);
            }

            //-------------------------------------------------------------------------
            /** True for string_id() and string_id("Invalid string_id") */
            constexpr bool is_none() const
            {
                return m_comparison_hash == s_none_state_hash_val;
            }

            //-------------------------------------------------------------------------
            /**
             * Retrieve the hashed value
             */
            constexpr u32 value() const
            {
                return static_cast<u32>(m_comparison_hash);
            }

        private:
            /** Hash for invalid string_id into string pool */
            static constexpr u32 s_none_state_hash_val = 0;

            /** Hash into the string_id hash table */
            u64 m_comparison_hash;
        };

        string_id store_sid(std::string_view characters);
        std::string_view restore_sid(const string_id& sid);

        bool operator==(std::string_view s, const string_id& sid);
        bool operator!=(std::string_view s, const string_id& sid);
        bool operator==(const string_id& sid, std::string_view s);
        bool operator!=(const string_id& sid, std::string_view s);
    }
}

constexpr ppp::string::string_id operator""_sid(const char* string, size_t size)
{
    return ppp::string::string_id(std::string_view(string, static_cast<u32>(size))); // NOLINT(cppcoreguidelines-narrowing-conversions)
}

std::ostream& operator<<(std::ostream& os, const ppp::string::string_id& stringID);

// custom specialization of std::hash can be injected in namespace std
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