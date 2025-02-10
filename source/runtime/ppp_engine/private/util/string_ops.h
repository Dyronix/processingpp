#pragma once

#include "memory/tagged_allocator.h"
#include "memory/tagged_heap_tags.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <xutility>

namespace ppp
{
    namespace string
    {
        //-----------------------------------------------------------------------------
        template <typename T>
        T string_replace(const T& subject, const T& search,const T& replace)
        {
            T result(subject);

            size_t pos = 0;

            while ((pos = subject.find(search, pos)) != T::npos)
            {
                result.replace(pos, search.length(), replace);
                pos += search.length();
            }

            return result;
        }

        //-----------------------------------------------------------------------------
        template <typename T>
        pool_vector<T> split_string(const std::string_view& str, const T& delimiters)
        {
            pool_vector<T> tokens;

            T token;

            // Create a set of delimiters (we assume the delimiter characters are always plain 'char's)
            pool_set<char> delimiter_set(delimiters.begin(), delimiters.end());

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
            template <typename TUnsignedInt>
            char* u_integral_to_buffer(char* r_next, TUnsignedInt u_val)
            {
                static_assert(std::is_unsigned_v<TUnsignedInt>, "TUnsignedInt must be unsigned");

#ifdef _WIN64
                auto u_val_trunc = u_val;
#else
                assert(sizeof(TUnsignedInt) <= 4);
#endif

                do 
                {
                    *--r_next = static_cast<char>('0' + (u_val_trunc % 10));
                    u_val_trunc /= 10;
                } while (u_val_trunc != 0);

                return r_next;
            }

            //-----------------------------------------------------------------------------
            template <typename TString, typename TConvert>
            TString integral_to_string(TConvert val)
            {
                static_assert(std::is_integral_v<TConvert>, "T must be integral");

                using TUnsigned = std::make_unsigned_t<TConvert>;

                // Buffer large enough for 64-bit values (and a sign) plus the terminating null.
                char buffer[21];
                char* const buffer_end = std::end(buffer);
                char* r_next = buffer_end;
                TUnsigned u_val = static_cast<TUnsigned>(val);

                if (val < 0) 
                {
                    // Convert the absolute value then add the '-' sign.
                    r_next = u_integral_to_buffer(r_next, static_cast<TUnsigned>(0) - u_val);
                    *--r_next = '-';
                }
                else 
                {
                    r_next = u_integral_to_buffer(r_next, u_val);
                }

                return TString(r_next, buffer_end);
            }

            //-----------------------------------------------------------------------------
            template <typename TString, typename TConvert>
            TString u_integral_to_string(TConvert val) 
            {
                static_assert(std::is_integral_v<TConvert>, "TConvert must be integral");
                static_assert(std::is_unsigned_v<TConvert>, "TConvert must be unsigned");
                
                char buffer[21];
                char* const buffer_end = std::end(buffer);
                char* r_next = u_integral_to_buffer(buffer_end, val);

                return TString(r_next, buffer_end);
            }
        }

        //-----------------------------------------------------------------------------
        template <typename TString>
        TString to_string(int val)
        {
            return internal::integral_to_string<TString, int>(val);
        }

        //-----------------------------------------------------------------------------
        template <typename TString>
        TString to_string(unsigned int val)
        {
            return internal::u_integral_to_string<TString, unsigned int>(val);
        }

        inline float stof(const std::string_view& _Str, size_t* _Idx = nullptr) {
            int& _Errno_ref = errno; // Nonzero cost, pay it once
            const char* _Ptr = _Str.data();
            char* _Eptr;
            _Errno_ref = 0;
            const float _Ans = _CSTD strtof(_Ptr, &_Eptr);

            if (_Ptr == _Eptr) {
                std::_Xinvalid_argument("invalid stof argument");
            }

            if (_Errno_ref == ERANGE) {
                std::_Xout_of_range("stof argument out of range");
            }

            if (_Idx) {
                *_Idx = static_cast<size_t>(_Eptr - _Ptr);
            }

            return _Ans;
        }

        inline int stoi(const std::string_view& _Str, size_t* _Idx = nullptr, int _Base = 10) {
            int& _Errno_ref = errno; // Nonzero cost, pay it once
            const char* _Ptr = _Str.data();
            char* _Eptr;
            _Errno_ref = 0;
            const long _Ans = _CSTD strtol(_Ptr, &_Eptr, _Base);

            if (_Ptr == _Eptr) {
                std::_Xinvalid_argument("invalid stoi argument");
            }

            if (_Errno_ref == ERANGE) {
                std::_Xout_of_range("stoi argument out of range");
            }

            if (_Idx) {
                *_Idx = static_cast<size_t>(_Eptr - _Ptr);
            }

            return static_cast<int>(_Ans);
        }
    }
}