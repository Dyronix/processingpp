#pragma once

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        flecs::entity register_picking_system(flecs::world& world);
    }
}