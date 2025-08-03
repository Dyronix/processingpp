#pragma once

#include "sierra_layer.h"

#include "ecs/components/ecs_components.h"
#include "ecs/systems/ecs_systems.h"

#include <vector>

namespace ppp
{
    class sierra_level_layer: public sierra_layer
    {
    public:
        sierra_level_layer(sierra_engine_context* ctx);

    protected:
        void on_attached() override;
        void on_enable() override;

    private:
        void load_level();
        void load_picking_query();

        void on_mouse_button_click();

        std::vector<flecs::entity> _level_grid;

        flecs::query<const class ecs::transform_component, const class ecs::bounding_box_component, const class ecs::pickable_component> _picking_query;
    };
}