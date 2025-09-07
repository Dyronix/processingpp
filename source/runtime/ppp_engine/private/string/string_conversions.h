#pragma once

#include "util/types.h"

#include <string>
#include <string_view>
#include <xutility>

namespace ppp
{
    namespace string
    {
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
                auto u_val_trunc = u_val;
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
        TString to_string(s32 val)
        {
            return internal::integral_to_string<TString, s32>(val);
        }

        //-----------------------------------------------------------------------------
        template <typename TString>
        TString to_string(u32 val)
        {
            return internal::u_integral_to_string<TString, u32>(val);
        }

        //-----------------------------------------------------------------------------
        inline f32 stof(const std::string_view& s_view, u64* index = nullptr)
        {
            int& error_nr_ref = errno; // Nonzero cost, pay it once

            const char* ptr = s_view.data();
            char* error_ptr;
            error_nr_ref = 0;
            const f32 answer = strtof(ptr, &error_ptr);

            if (ptr == error_ptr)
            {
                std::_Xinvalid_argument("invalid stof argument");
            }

            if (error_nr_ref == ERANGE)
            {
                std::_Xout_of_range("stof argument out of range");
            }

            if (index)
            {
                *index = static_cast<u64>(error_ptr - ptr);
            }

            return answer;
        }

        //-----------------------------------------------------------------------------
        inline int stoi(const std::string_view& s_view, u64* index = nullptr, int _Base = 10)
        {
            int& error_nr_ref = errno; // Nonzero cost, pay it once

            const char* ptr = s_view.data();
            char* error_ptr;
            error_nr_ref = 0;
            const slong answer = strtol(ptr, &error_ptr, _Base);

            if (ptr == error_ptr)
            {
                std::_Xinvalid_argument("invalid stoi argument");
            }

            if (error_nr_ref == ERANGE)
            {
                std::_Xout_of_range("stoi argument out of range");
            }

            if (index)
            {
                *index = static_cast<u64>(error_ptr - ptr);
            }

            return static_cast<int>(answer);
        }
    }
}