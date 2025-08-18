#pragma once

#include "sierra_input_processor.h"

#include "ecs/components/ecs_components.h"

namespace ppp
{
    class sierra_layer;

    class ui_input_processor : public isierra_input_processor
    {
    public:
        ui_input_processor(sierra_layer* layer);

        bool on_input(const input_event& evt);

    private:
        using picking_query = flecs::query<
            const class ecs::rect_transform_component,
            const class ecs::bounding_box_2d_component,
            const class ecs::sorting_layer_component,
            const class ecs::pickable_component>;

        picking_query _picking_query;
    };
}