#pragma once

#include "model.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace ecs
    {
        struct fill_color_component
        {
            glm::ivec4 color{ 255,255,255,255 };
            glm::ivec4 highlight{ 255,255,255,255 };
        };
    }
}