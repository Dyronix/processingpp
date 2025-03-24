#pragma once

#include <glm/glm.hpp>

namespace ppp
{
    namespace geometry
    {
        struct bounding_box
        {
            glm::vec3 min;
            glm::vec3 max;
            glm::vec3 size;
            glm::vec3 offset;

            explicit operator bool() const
            {
                return glm::length(size) > 0;
            }
        };
    }
}