#include "sierra_input_handler.h"
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
    struct istate
    {
        //-------------------------------------------------------------------------
        bool on_input(const input_event& evt)
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

        std::vector<std::unique_ptr<isierra_input_processor>> processors;
    };

    struct idle_state : public istate
    {
        //-------------------------------------------------------------------------
        idle_state(sierra_layer* owning_layer) 
            : istate()
        {
            processors.push_back(std::make_unique<ui_input_processor>(owning_layer));
            processors.push_back(std::make_unique<world_input_processor>(owning_layer));
            processors.push_back(std::make_unique<placement_input_processor>(owning_layer));
        }
    };
    struct placement_state : public istate
    {
        //-------------------------------------------------------------------------
        placement_state(sierra_layer* owning_layer) 
            : istate()
        {
            processors.push_back(std::make_unique<ui_input_processor>(owning_layer));
            processors.push_back(std::make_unique<placement_input_processor>(owning_layer));
        }
    };

    //-------------------------------------------------------------------------
    sierra_input_handler::sierra_input_handler(sierra_layer* layer)
    {
        _idle_state = std::make_unique<idle_state>(layer);
        _placement_state = std::make_unique<placement_state>(layer);

        change_to_idle();

        layer->context()->player_state.subscribe_start_placement([&]() { change_to_placement(); });
        layer->context()->player_state.subscribe_stop_placement([&]() { change_to_idle(); });

        register_mouse_input_events();
        register_keyboard_input_events();
    }
    //-------------------------------------------------------------------------
    sierra_input_handler::~sierra_input_handler() = default;

    //-------------------------------------------------------------------------
    void sierra_input_handler::register_mouse_input_events()
    {
        add_mouse_moved_callback([&](float x, float y)
        {
            input_event evt;
            evt.type = input_event_type::MOUSE_MOTION;
            evt.mouse_motion.x = x;
            evt.mouse_motion.y = y;
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

    //-------------------------------------------------------------------------
    void sierra_input_handler::change_to_idle()
    {
        _active_state = _idle_state.get();
    }
    //-------------------------------------------------------------------------
    void sierra_input_handler::change_to_placement()
    {
        _active_state = _placement_state.get();
    }
}