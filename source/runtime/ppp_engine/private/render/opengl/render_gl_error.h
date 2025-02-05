#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>

#pragma warning(push)
#pragma warning(disable : 4505)

namespace ppp
{
    namespace render
    {
        pool_string gl_get_error_message(u32 error);

        u32         gl_check_error();

        bool        gl_log_call(const char* function, const char* file, s32 line);
    }
}

#ifdef _DEBUG
    #define GL_CALL(x)                                                      \
        ppp::render::gl_check_error();                                      \
        x;                                                                  \
        if (!ppp::render::gl_log_call(#x, __FILE__, __LINE__))              \
        {                                                                   \
            __debugbreak();                                                 \
        }
#else
    #define GL_CALL(x) x
#endif

#pragma warning(pop)