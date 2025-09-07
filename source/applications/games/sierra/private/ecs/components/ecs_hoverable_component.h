#pragma once

#include <functional>

namespace ppp
{
    namespace ecs
    {
        struct hoverable_component
        {
            std::function<void()> on_hover_enter;
            std::function<void()> on_hover_exit;
        };
    }
}