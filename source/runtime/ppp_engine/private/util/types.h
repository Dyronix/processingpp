#pragma once

#include <atomic>
#include <vector>
#include <array>

#include <assert.h>

using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

static_assert(sizeof(s8) == 1, "int8 must be 1 byte big");      // NOLINT
static_assert(sizeof(s16) == 2, "int16 must be 2 bytes big");   // NOLINT
static_assert(sizeof(s32) == 4, "int32 must be 4 bytes big");   // NOLINT
static_assert(sizeof(s64) == 8, "int64 must be 8 bytes big");   // NOLINT

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using hash_id = s32;

static_assert(sizeof(u8) == 1, "uint8 must be 1 byte big");    // NOLINT
static_assert(sizeof(u16) == 2, "uint16 must be 2 bytes big"); // NOLINT
static_assert(sizeof(u32) == 4, "uint32 must be 4 bytes big"); // NOLINT
static_assert(sizeof(u64) == 8, "uint64 must be 8 bytes big"); // NOLINT

using slong = signed long;
using ulong = unsigned long;

static_assert(sizeof(slong) == sizeof(s32), "long should be the same size as int32");
static_assert(sizeof(ulong) == sizeof(u32), "ulong should be the same size as uint32");

using char8 = char;
using char16 = char16_t;
using char32 = char32_t;
using tchar = wchar_t;

static_assert(sizeof(char8) == 1, "char8 must be 1 byte big");   // NOLINT
static_assert(sizeof(char16) == 2, "char16 must be 2 byte big"); // NOLINT
static_assert(sizeof(char32) == 4, "char32 must be 4 byte big"); // NOLINT

// with MSVC, wchar_t is 2 bytes big, while with clang and gcc, it's 4 bytes big
static_assert(sizeof(tchar) == 2, "tchar must be 2 bytes big");

using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4, "float32 must be 4 bytes big"); // NOLINT
static_assert(sizeof(f64) == 8, "float64 must be 8 bytes big"); // NOLINT

using uptr = uintptr_t;
using sptr = intptr_t;

static_assert(sizeof(uptr) == sizeof(void*), "uptr must be the same size as a pointer"); // NOLINT
static_assert(sizeof(sptr) == sizeof(void*), "sptr must be the same size as a pointer"); // NOLINT


// allow single threaded platforms to avoid use of atomic
#if PPP_WEB || PPP_WINDOWS
#define PPP_SINGLE_THREADED 1
#else
#define PPP_SINGLE_THREADED 0
#endif

#if PPP_SINGLE_THREADED
using a_u8 = u8;
using a_u32 = u32;
using a_u64 = u64;
using a_bool = bool;
using a_s32 = s32;
#define ppp_atomic_load(a) a
#else
using a_u8 = std::atomic<u8>;
using a_u32 = std::atomic<u32>;
using a_u64 = std::atomic<u64>;
using a_bool = std::atomic<bool>;
using a_s32 = std::atomic<s32>;
#define ppp_atomic_load(a) a.load()
#endif

namespace ppp
{
    using blob = std::vector<std::byte>;

    namespace render
    {
        using index = u32;
        using texture_id = u32;

        struct face
        {
            std::array<render::index, 3> fvs;

            //-------------------------------------------------------------------------
            render::index& operator[](size_t i) 
            {
#ifdef _DEBUG  // Use appropriate macro for your build system
                assert(i < fvs.size() && "Index out of bounds");
#endif
                return fvs[i];
            }

            //-------------------------------------------------------------------------
            const render::index& operator[](size_t i) const 
            {
#ifdef _DEBUG
                assert(i < fvs.size() && "Index out of bounds");
#endif
                return fvs[i];
            }

            //-------------------------------------------------------------------------
            constexpr size_t size() const noexcept 
            {
                return fvs.size();
            }

            //-------------------------------------------------------------------------
            // Iterator support for range-based for loops (non-const version)
            auto begin() noexcept { return fvs.begin(); }
            auto end() noexcept { return fvs.end(); }

            //-------------------------------------------------------------------------
            // Iterator support for range-based for loops (const version)
            auto begin() const noexcept { return fvs.begin(); }
            auto end() const noexcept { return fvs.end(); }
        };
    }

    namespace conversions
    {
        namespace internal
        {
            inline f32 saturate(f32 f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
        }

        inline u32 f32_to_uint8(f32 val) { return (u32)(internal::saturate(val) * 255.0f + 0.5f); }
    }

    // store a single bit in an unsigned integer
    constexpr u32 BIT(u32 x)
    {
        return 1u << x;
    }

    // Alias for the type used to represent unique object type IDs.
    using object_type_tag = u64;

    // type_tag class template, used to generate a unique ID for each distinct type T.
    template <typename T>
    class type_tag
    {
    public:
        // The value() method returns the unique ID for the type T.
        static object_type_tag value()
        {
            return m_id;
        }

    private:
        static object_type_tag m_id;
    };

    // The next() method returns the next unique ID.
    inline object_type_tag next()
    {
        // 'next_id' is initialized to 0 the first time this function is called.
        // It is incremented after each use to ensure every type gets a unique ID.
        static object_type_tag current_id = 0;
        return current_id++;
    }

    template<typename T>
    object_type_tag type_tag<T>::m_id = next();
}