#pragma once

#include "memory/tagged_allocator.h"
#include "memory/tagged_heap_tags.h"

#include <string>
#include <vector>
#include <unordered_set>

namespace ppp
{
    namespace string
    {
        template <u32 allocator_tag>
        std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>
            string_replace(
                const std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>& subject,
                const std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>& search,
                const std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>& replace)
        {
            using string_type = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>;

            string_type result(subject);

            size_t pos = 0;

            while ((pos = subject.find(search, pos)) != string_type::npos)
            {
                result.replace(pos, search.length(), replace);
                pos += search.length();
            }

            return result;
        }

        template <u32 allocator_tag>
        std::vector<std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>>
            split_string(
                const std::basic_string_view<char>& str,
                const std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>& delimiters)
        {
            using string_type = std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>;

            std::vector<string_type> tokens;
            string_type token;

            // Create a set of delimiters (we assume the delimiter characters are always plain 'char's)
            std::unordered_set<char, std::hash<char>, std::equal_to<char>, memory::tagged_allocator<char, allocator_tag>> delimiter_set(delimiters.begin(), delimiters.end());

            for (char ch : str)
            {
                if (delimiter_set.count(ch))
                {
                    // If we encounter a delimiter and the current token is non-empty, store it.
                    if (!token.empty())
                    {
                        tokens.push_back(token);
                        token.clear();
                    }
                }
                else
                {
                    // Otherwise, append the character to the current token.
                    token.push_back(ch);
                }
            }

            // Add any remaining token.
            if (!token.empty())
            {
                tokens.push_back(token);
            }

            return tokens;
        }

        namespace internal
        {
            //-----------------------------------------------------------------------------
            // Internal: Convert an unsigned integer into characters in a buffer.
            // The conversion writes digits backwards into the provided buffer.
            template <typename CharT, typename UIntTy>
            CharT* u_integral_to_buffer(CharT* r_next, UIntTy u_val) 
            {
                static_assert(std::is_unsigned_v<UIntTy>, "UIntTy must be unsigned");

#ifdef _WIN64
                auto u_val_trunc = u_val;
#else
                // On non‐WIN64 platforms, if the unsigned type is “big” (more than 32 bits),
                // convert it in chunks (to avoid 64‐bit divisions).
                if constexpr (sizeof(UIntTy) > 4) {
                    while (u_val > 0xFFFFFFFFU) {
                        auto chunk = static_cast<unsigned long>(u_val % 1000000000);
                        u_val /= 1000000000;
                        for (int idx = 0; idx < 9; ++idx) {
                            *--r_next = static_cast<CharT>('0' + (chunk % 10));
                            chunk /= 10;
                        }
                    }
                }
                auto u_val_trunc = static_cast<unsigned long>(u_val);
#endif

                do {
                    *--r_next = static_cast<CharT>('0' + (u_val_trunc % 10));
                    u_val_trunc /= 10;
                } while (u_val_trunc != 0);

                return r_next;
            }

            template <typename CharT, u32 allocator_tag, typename T>
            std::basic_string<CharT, std::char_traits<CharT>, memory::tagged_allocator<CharT, allocator_tag>>
                integral_to_string(T val) 
            {
                static_assert(std::is_integral_v<T>, "T must be integral");

                using string_type = std::basic_string<CharT, std::char_traits<CharT>, memory::tagged_allocator<CharT, allocator_tag>>;
                using UnsignedT = std::make_unsigned_t<T>;

                // Buffer large enough for 64-bit values (and a sign) plus the terminating null.
                CharT buffer[21];
                CharT* const buffer_end = std::end(buffer);
                CharT* r_next = buffer_end;
                UnsignedT u_val = static_cast<UnsignedT>(val);

                if (val < 0) {
                    // Convert the absolute value then add the '-' sign.
                    r_next = u_integral_to_buffer(r_next, static_cast<UnsignedT>(0) - u_val);
                    *--r_next = '-';
                }
                else {
                    r_next = u_integral_to_buffer(r_next, u_val);
                }

                return string_type(r_next, buffer_end);
            }

            template <typename CharT, u32 allocator_tag, typename T>
            std::basic_string<CharT, std::char_traits<CharT>, memory::tagged_allocator<CharT, allocator_tag>>
                u_integral_to_string(T val) 
            {
                static_assert(std::is_integral_v<T>, "T must be integral");
                static_assert(std::is_unsigned_v<T>, "T must be unsigned");
                using string_type = std::basic_string<CharT, std::char_traits<CharT>, memory::tagged_allocator<CharT, allocator_tag>>;

                CharT buffer[21];
                CharT* const buffer_end = std::end(buffer);
                CharT* r_next = u_integral_to_buffer(buffer_end, val);

                return string_type(r_next, buffer_end);
            }
        }

        template <u32 allocator_tag>
        std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>
            to_string(int val)
        {
            return internal::integral_to_string<char, allocator_tag>(val);
        }

        template <u32 allocator_tag>
        std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>>
            to_string(unsigned int val)
        {
            return internal::u_integral_to_string<char, allocator_tag>(val);
        }
    }
}