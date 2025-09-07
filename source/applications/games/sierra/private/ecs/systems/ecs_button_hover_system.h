#pragma once

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        flecs::entity register_button_hover_enter_system(flecs::world& world);
        flecs::entity register_button_hover_exit_system(flecs::world& world);
    }
}