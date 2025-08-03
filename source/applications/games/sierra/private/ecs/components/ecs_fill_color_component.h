#pragma once

#include "model.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace ecs
    {
        struct fill_color_component
        {
            glm::ivec4 color;
            glm::ivec4 highlight;
        };
    }
}