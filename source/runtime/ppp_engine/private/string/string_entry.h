#pragma once

#include "util/types.h"

#include <string>
#include <memory>

namespace ppp
{
    namespace string
    {
        class string_entry
        {
        public:
            string_entry(std::string_view characters) noexcept; // NOLINT(google-explicit-constructor)
            string_entry(const string_entry& other) = delete;
            string_entry(string_entry&& other) noexcept;
            ~string_entry();

            string_entry& operator=(const string_entry& other) = delete;
            string_entry& operator=(string_entry&& other) noexcept;

            bool is_valid() const;

            std::string_view characters() const;

            s32 size() const;

        private:
            std::unique_ptr<char[]> m_characters;
            s32 m_size;
        };
    }
}