#pragma once

#include <glm/glm.hpp>

namespace ppp
{
    namespace ecs
    {
        struct bounding_box_component
        {
            glm::vec3 min;
            glm::vec3 max;
        };
    }
}