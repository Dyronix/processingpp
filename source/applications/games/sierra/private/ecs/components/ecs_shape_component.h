#pragma once

#include <functional>

namespace ppp
{
    namespace ecs
    {
        struct shape_component
        {
            std::function<void()> draw_fn;
        };
    }
}