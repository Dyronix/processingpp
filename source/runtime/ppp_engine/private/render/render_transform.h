#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

namespace ppp
{
    namespace render
    {
        namespace transform
        {
            void push();
            void pop();

            void rotate(f32 angle);
            void scale(const glm::vec2& scale);
            void translate(const glm::vec2& translate);

            glm::mat4 active_world();
        }
    }
}