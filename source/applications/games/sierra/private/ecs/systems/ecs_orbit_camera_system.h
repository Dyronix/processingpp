#pragma once

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        flecs::entity register_orbit_camera_system(flecs::world& world);
    }
}