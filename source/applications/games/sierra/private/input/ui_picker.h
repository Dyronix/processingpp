#pragma once

#include "ecs/components/ecs_components.h"

#include <flecs.h>

namespace ppp
{
    class ui_picker
    {
    public:
        ui_picker(flecs::world& world);

        flecs::entity pick(f32 mx, f32 my) const;

    private:
        using picking_query = flecs::query<
            const class ecs::rect_transform_component,
            const class ecs::bounding_box_2d_component,
            const class ecs::sorting_layer_component,
            const class ecs::pickable_component>;

        picking_query _picking_query;
    };
}