#pragma once

#include "util/types.h"

#include <flecs.h>

#include <string>

namespace ppp
{
    namespace ecs
    {
        class scene
        {
        public:
            scene();
            ~scene();

            flecs::entity create_entity(const std::string& name = "");

            void init();
            void tick(f32 deltaTime);
            void draw();

            flecs::world& world() { return _world; }

            const ecs_world_stats_t& tick_stats() const { return _tick_stats; }
            const ecs_world_stats_t& draw_stats() const { return _draw_stats; }

        private:
            flecs::world _world;

            flecs::entity _init_pipeline;
            flecs::entity _tick_pipeline;
            flecs::entity _draw_pipeline;

            ecs_world_stats_t _tick_stats = {};
            ecs_world_stats_t _draw_stats = {};
        };
    }
}