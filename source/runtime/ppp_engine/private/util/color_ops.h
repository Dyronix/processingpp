#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace color
    {
        u32 convert_color(const glm::vec4 color);
        glm::vec4 convert_color(u32 color);
    }
}