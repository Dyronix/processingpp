#pragma once

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        void register_shape_3d_render_system(flecs::world& world);
        void register_shape_2d_render_system(flecs::world& world);
        void register_model_render_system(flecs::world& world);
    }
}