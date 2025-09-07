#include "placement_input_processor.h"

#include "sierra_layer.h"
#include "sierra_engine_context.h"
#include "sierra_player_state.h"

#include "util/log.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    placement_input_processor::placement_input_processor(sierra_engine_context* context)
        :_context(context)
    {}

    //-------------------------------------------------------------------------
    bool placement_input_processor::on_input(const input_event& evt)
    {
        switch (evt.type)
        {
        case input_event_type::KEY_PRESS: return handle_key_press(evt.keyboard);
        case input_event_type::MOUSE_PRESS: return handle_mouse_press(evt.mouse_button);
        }

        return false;
    }

    //-------------------------------------------------------------------------
    bool placement_input_processor::handle_mouse_press(const mouse_button_event& evt)
    {
        switch (evt.button)
        {
        case mouse_code::BUTTON_LEFT:
            log::info("Going to try and spawn a tower");
            _context->player_state.try_spawn_tower();
            return true;
        }

        return false;
    }
    //-------------------------------------------------------------------------
    bool placement_input_processor::handle_key_press(const keyboard_event& evt)
    {
        switch (evt.code)
        {
        case key_code::KEY_SPACE:
            log::info("Going to change to placement state: {}", !_context->player_state.is_placing());
            _context->player_state.toggle_placement();
            return true;
        }

        return false;
    }
}