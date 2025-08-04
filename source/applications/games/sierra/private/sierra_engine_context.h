#pragma once

#include "ecs/ecs_scene_manager.h"
#include "ecs/ecs_scene.h"

#include "layers/layer_stack.h"
#include "layers/layer.h"

#include "sierra_player_state.h"

namespace ppp
{
    struct sierra_engine_context
    {
        ecs::scene_manager scene_manager;

        layer_stack layer_stack;

        sierra_player_state player_state;
    };
}