#pragma once

#include <flecs.h>

#include <string>

namespace ppp
{
    class layer_stack;

    namespace imgui
    {
        void draw_inspector(const ecs_world_stats_t& world_stats, const std::string& active_scene_name, layer_stack& stack);
    }
}