#pragma once

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        void register_shape_render_system(flecs::world& world);
        void register_model_render_system(flecs::world& world);
    }
}