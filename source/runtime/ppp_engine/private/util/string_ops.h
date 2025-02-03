#pragma once

#include "memory/tagged_allocator.h"
#include "memory/tagged_heap_tags.h"

#include <string>
#include <vector>

namespace ppp
{
    namespace string
    {
        template<typename TString>
        TString string_replace(const TString& subject, const TString& search, const TString& replace)
        {
            TString result(subject);

            size_t pos = 0;

            while ((pos = subject.find(search, pos)) != TString::npos)
            {
                result.replace(pos, search.length(), replace);
                pos += search.length();
            }

            return result;
        }

        std::vector<std::string> split_string(const std::string& str, const std::string& delimiters);

        template <class _Elem, class _UTy>
        _NODISCARD _Elem* _UIntegral_to_buff(_Elem* _RNext, _UTy _UVal)
        {
            // format _UVal into buffer *ending at* _RNext
            static_assert(std::is_unsigned_v<_UTy>, "_UTy must be unsigned");

            #ifdef _WIN64
            auto _UVal_trunc = _UVal;
            #else // ^^^ _WIN64 / !_WIN64 vvv

            constexpr bool _Big_uty = sizeof(_UTy) > 4;
            if constexpr (_Big_uty) { // For 64-bit numbers, work in chunks to avoid 64-bit divisions.
                while (_UVal > 0xFFFFFFFFU) {
                    auto _UVal_chunk = static_cast<unsigned long>(_UVal % 1000000000);
                    _UVal /= 1000000000;

                    for (int _Idx = 0; _Idx != 9; ++_Idx) {
                        *--_RNext = static_cast<_Elem>('0' + _UVal_chunk % 10);
                        _UVal_chunk /= 10;
                    }
                }
            }

            auto _UVal_trunc = static_cast<unsigned long>(_UVal);
            #endif // _WIN64

            do {
                *--_RNext = static_cast<_Elem>('0' + _UVal_trunc % 10);
                _UVal_trunc /= 10;
            } while (_UVal_trunc != 0);
            return _RNext;
        }

        template <class _Elem, u32 allocator_tag, class _Ty>
        _NODISCARD std::basic_string<_Elem, std::char_traits<_Elem>, memory::tagged_allocator<_Elem, allocator_tag>> _Integral_to_string(const _Ty _Val)
        {
            // convert _Val to string
            static_assert(std::is_integral_v<_Ty>, "_Ty must be integral");
            using _UTy = std::make_unsigned_t<_Ty>;
            _Elem _Buff[21]; // can hold -2^63 and 2^64 - 1, plus NUL
            _Elem* const _Buff_end = _STD end(_Buff);
            _Elem* _RNext = _Buff_end;
            const auto _UVal = static_cast<_UTy>(_Val);
            if (_Val < 0) {
                _RNext = _UIntegral_to_buff(_RNext, 0 - _UVal);
                *--_RNext = '-';
            }
            else {
                _RNext = _UIntegral_to_buff(_RNext, _UVal);
            }

            return std::basic_string<_Elem, std::char_traits<_Elem>, memory::tagged_allocator<_Elem, allocator_tag>>(_RNext, _Buff_end);
        }

        // TRANSITION, CUDA - warning: pointless comparison of unsigned integer with zero
        template <class _Elem, u32 allocator_tag, class _Ty>
        _NODISCARD std::basic_string<_Elem, std::char_traits<_Elem>, memory::tagged_allocator<_Elem, allocator_tag>> _UIntegral_to_string(const _Ty _Val)
        {
            // convert _Val to string
            static_assert(std::is_integral_v<_Ty>, "_Ty must be integral");
            static_assert(std::is_unsigned_v<_Ty>, "_Ty must be unsigned");
            _Elem _Buff[21]; // can hold 2^64 - 1, plus NUL
            _Elem* const _Buff_end = _STD end(_Buff);
            _Elem* const _RNext = _UIntegral_to_buff(_Buff_end, _Val);
            return std::basic_string<_Elem, std::char_traits<_Elem>, memory::tagged_allocator<_Elem, allocator_tag>>(_RNext, _Buff_end);
        }

        template <u32 allocator_tag>
        _EXPORT_STD _NODISCARD inline std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>> to_string(int _Val)
        {
            return _Integral_to_string<char, allocator_tag>(_Val);
        }

        template <u32 allocator_tag>
        _EXPORT_STD _NODISCARD inline std::basic_string<char, std::char_traits<char>, memory::tagged_allocator<char, allocator_tag>> to_string(unsigned int _Val)
        {
            return _UIntegral_to_string<char, allocator_tag>(_Val);
        }
    }
}