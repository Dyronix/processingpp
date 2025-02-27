#pragma once

#include "render/opengl/render_gl_function_library.h"

namespace ppp
{
    namespace render
    {
        namespace opengl
        {
            class api
            {
            public:
                static ifunction_library& instance();
            };
        }
    }
}