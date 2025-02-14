#pragma once

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
        auto string_replace(std::string_view subject, std::string_view search, std::string_view replace) -> TString
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
        auto split_string(std::string_view str, std::string_view delimiters)
            -> std::vector<TString, typename std::allocator_traits<typename TString::allocator_type>::template rebind_alloc<TString>>
        {
            using TChar             = typename TString::value_type;
            using TAllocator        = typename TString::allocator_type;

            using TAllocForTString  = typename std::allocator_traits<TAllocator>::template rebind_alloc<TString>;

            using TVector           = std::vector<TString, TAllocForTString>;
            using TUnorderedSet     = std::unordered_set<TChar, std::hash<TChar>, std::equal_to<TChar>, TAllocator>;
            
            TVector         tokens(TAllocForTString{});
            TUnorderedSet   delimiter_set(delimiters.begin(), delimiters.end(), delimiters.size());

            TString         token(TAllocator{});

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