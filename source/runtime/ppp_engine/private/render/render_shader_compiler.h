#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <string>

namespace ppp
{
    namespace render
    {
        namespace shaders
        {       
            u32 create_shader_program(std::string_view vs_source, std::string_view fs_source);
            u32 create_shader_program(std::string_view vs_source, std::string_view fs_source, std::string_view geometry_source);
        }
    }
}