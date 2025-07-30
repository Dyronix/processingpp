#pragma once

#include <flecs.h>

namespace ppp
{
    namespace imgui
    {
        void draw_stats(const ecs_world_stats_t& world_stats);
    }
}