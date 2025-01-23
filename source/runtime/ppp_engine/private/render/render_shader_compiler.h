#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {       
            u32 create_shader_program(const char* vs_source, const char* fs_source);
            u32 create_shader_program(const char* vs_source, const char* fs_source, const char* geometry_source);
        }
    }
}