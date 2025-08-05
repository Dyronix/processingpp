#pragma once

#include <glm/glm.hpp>

namespace ppp
{
    namespace ecs
    {
        struct bounding_box_2d_component
        {
            glm::vec2 min;
            glm::vec2 max;
        };
    }
}