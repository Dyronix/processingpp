#include "switch_scene_input_processor.h"

#include "sierra_layer.h"
#include "sierra_engine_context.h"
#include "sierra_player_state.h"

#include "ecs/ecs_scene_manager.h"

#include "util/log.h"

#include <string>

namespace ppp
{
    //-------------------------------------------------------------------------
    switch_scene_input_processor::switch_scene_input_processor(sierra_engine_context* context)
        :_context(context)
    {
    }

    //-------------------------------------------------------------------------
    bool switch_scene_input_processor::on_input(const input_event& evt)
    {
        switch (evt.type)
        {
        case input_event_type::KEY_PRESS: return handle_key_press(evt.keyboard);
        }

        return false;
    }

    //-------------------------------------------------------------------------
    bool switch_scene_input_processor::handle_key_press(const keyboard_event& evt)
    {
        switch (evt.code)
        {
        case key_code::KEY_0:
            std::string active_scene_naame = _context->scene_manager.active_scene_name();
            std::string new_scene_scene = active_scene_naame == "main"
                ? "menu"
                : "main";

            log::info("Going to switch scene: {} -> {}", active_scene_naame, new_scene_scene);
            _context->scene_manager.set_active_scene(new_scene_scene);
            return true;
        }

        return false;
    }
}