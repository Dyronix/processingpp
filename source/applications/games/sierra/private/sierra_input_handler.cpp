#include "sierra_input_handler.h"
#include "sierra.h"
#include "sierra_layer.h"
#include "sierra_engine_context.h"
#include "sierra_player_state.h"
#include "sierra_input_event.h"
#include "sierra_input_processor.h"

#include "input/world_input_processor.h"
#include "input/ui_input_processor.h"
#include "input/placement_input_processor.h"

#include "ecs/components/ecs_components.h"

#include "render/render.h"

#include "ray.h"
#include "camera.h"
#include "events.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    bool istate::on_input(const input_event& evt)
    {
        for (auto& processor : processors)
        {
            if (processor->on_input(evt))
            {
                return true;
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------
    void sierra_input_handler::init()
    {
        register_mouse_input_events();
        register_keyboard_input_events();
    }

    //-------------------------------------------------------------------------S
    void sierra_input_handler::push_input_state(const std::string& str, std::unique_ptr<istate> state)
    {
        auto it = _input_states.find(str);
        if (it != std::cend(_input_states))
        {
            log::warn("Input state with tag \"{}\" already present.", str);
            return;
        }

        _input_states.emplace(str, std::move(state));
    }
    //-------------------------------------------------------------------------
    void sierra_input_handler::push_active_input_state(const std::string& str)
    {
        auto it = _input_states.find(str);
        if (it == std::cend(_input_states))
        {
            log::warn("Input state with tag \"{}\" not present.", str);
            return;
        }

        _active_state = it->second.get();
    }

    //-------------------------------------------------------------------------
    void sierra_input_handler::register_mouse_input_events()
    {
        add_mouse_moved_callback([&](float x, float y)
        {
            input_event evt;
            evt.type = input_event_type::MOUSE_MOTION;
            evt.mouse_motion.x = mouse_x();
            evt.mouse_motion.y = mouse_y();
            _active_state->on_input(evt);
        });

        add_mouse_pressed_callback([&](mouse_code code)
        {
            input_event evt;
            evt.type = input_event_type::MOUSE_PRESS;
            evt.mouse_button.button = code;
            evt.mouse_button.x = mouse_x();
            evt.mouse_button.y = mouse_y();
            _active_state->on_input(evt);
        });

        add_mouse_released_callback([&](mouse_code code)
        {
            input_event evt;
            evt.type = input_event_type::MOUSE_RELEASE;
            evt.mouse_button.button = code;
            evt.mouse_button.x = mouse_x();
            evt.mouse_button.y = mouse_y();
            _active_state->on_input(evt);
        });

        add_mouse_verticel_wheel_callback([&](float scroll)
        {
            input_event evt;
            evt.type = input_event_type::MOUSE_V_WHEEL;
            evt.mouse_v_wheel.wheel_v = scroll;
            evt.mouse_v_wheel.x = mouse_x();
            evt.mouse_v_wheel.y = mouse_y();
            _active_state->on_input(evt);
        });

        add_mouse_horizontal_wheel_callback([&](float scroll)
        {
            input_event evt;
            evt.type = input_event_type::MOUSE_H_WHEEL;
            evt.mouse_h_wheel.wheel_h = scroll;
            evt.mouse_h_wheel.x = mouse_x();
            evt.mouse_h_wheel.y = mouse_y();
            _active_state->on_input(evt);
        });
    }
    //-------------------------------------------------------------------------
    void sierra_input_handler::register_keyboard_input_events()
    {
        add_key_pressed_callback([&](key_code code)
        {
            input_event evt;
            evt.type = input_event_type::KEY_PRESS;
            evt.keyboard.code = code;
            _active_state->on_input(evt);
        });

        add_key_released_callback([&](key_code code)
        {
            input_event evt;
            evt.type = input_event_type::KEY_RELEASE;
            evt.keyboard.code = code;
            _active_state->on_input(evt);
        });

        add_key_down_callback([&](key_code code)
        {
            input_event evt;
            evt.type = input_event_type::KEY_DOWN;
            evt.keyboard.code = code;
            _active_state->on_input(evt);
        });
    }
}