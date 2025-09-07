#pragma once

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        flecs::entity register_button_click_system(flecs::world& world);
    }
}