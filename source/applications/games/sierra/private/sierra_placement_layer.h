#pragma once

#include "sierra_layer.h"

#include "ecs/components/ecs_components.h"
#include "ecs/systems/ecs_systems.h"

#include <flecs.h>

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
        flecs::entity spawn_tower(const char* tag, const glm::vec3& position = {0.0f, 0.0f, 0.0f}, bool visible = false);

        void load_placement_handler();
        void load_placement_template();
        void load_placement_finisher();

        flecs::entity _placement_handler;
        flecs::entity _placement_template;

        std::vector<flecs::entity> _towers;
    };
}