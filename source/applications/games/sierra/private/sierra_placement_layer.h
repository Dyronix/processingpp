#pragma once

#include "sierra_layer.h"

#include "ecs/components/ecs_components.h"
#include "ecs/systems/ecs_systems.h"

#include <vector>
#include <memory>

namespace ppp
{
    class sierra_placement_layer : public sierra_layer
    {
    public:
        sierra_placement_layer(sierra_engine_context* ctx);

    protected:
        void on_attached() override;
        void on_enable() override;

    private:
        void load_placement_handler();
        void load_placement_template();

        flecs::entity _placement_handler;
        flecs::entity _placement_template;
    };
}