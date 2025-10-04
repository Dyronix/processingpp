#pragma once

#include <functional>

namespace ppp
{
    namespace ecs
    {
        struct clickable_component
        {
            std::function<void()> on_click;
        };
    }
}