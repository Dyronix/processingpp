#pragma once

#include "memory/tagged_allocator.h"
#include "memory/tagged_heap_tags.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <type_traits>
#include <string_view>

namespace ppp
{
    namespace string
    {
        //-----------------------------------------------------------------------------
        template <typename TString>
        auto string_replace(const TString& subject, const TString& search, const TString& replace) -> TString
        {
            typedef typename TString::allocator_type TAllocator;

            TString result(subject, TAllocator{}); // Ensure the correct allocator is used

            size_t pos = 0;

            while ((pos = result.find(search, pos)) != TString::npos)  // Should search inside `result`, not `subject`
            {
                result.replace(pos, search.length(), replace);
                pos += replace.length();  // Ensure we move past the replaced section
            }

            return result;
        }

        //-----------------------------------------------------------------------------
        template <typename TString>
        auto split_string(const std::string_view& str, const TString& delimiters) -> std::vector<TString, typename TString::allocator_type>
        {
            typedef typename TString::value_type TChar;
            typedef typename TString::allocator_type TAllocator;

            typedef std::vector<TString, TAllocator> TVector;
            typedef std::unordered_set<TChar, std::less<TChar>, TAllocator> TUnorderedSet;

            TVector tokens(TAllocator{});
            TUnorderedSet delimiter_set(delimiters.begin(), delimiters.end(), TAllocator{});

            TString token(TAllocator{});

            for (TChar ch : str)
            {
                if (delimiter_set.count(ch))
                {
                    if (!token.empty())
                    {
                        tokens.push_back(std::move(token));
                        token = TString(TAllocator{}); // Reset token with allocator
                    }
                }
                else
                {
                    token.push_back(ch);
                }
            }

            if (!token.empty())
            {
                tokens.push_back(std::move(token));
            }

            return tokens;
        }
    }
}