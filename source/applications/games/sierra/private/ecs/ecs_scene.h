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

            flecs::entity create_entity(const std::string& name = "") const;

            void init();
            void begin_frame();
            void tick(f32 deltaTime);
            void draw();
            void end_frame();

            flecs::world& world() { return _world; }

            const ecs_world_stats_t& world_stats() const { return _world_stats; }

        private:
            flecs::world _world;

            flecs::entity _init_pipeline;
            flecs::entity _tick_pipeline;
            flecs::entity _draw_pipeline;

            ecs_world_stats_t _world_stats = {};
        };
    }
}