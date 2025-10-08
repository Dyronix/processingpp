#include "input/ui_input_processor.h"

#include "ecs/ecs_scene_manager.h"
#include "ecs/ecs_scene.h"

#include "sierra_engine_context.h"

#include "util/log.h"

#include "events.h"

#include <string>

namespace ppp
{
    //-------------------------------------------------------------------------
    ui_input_processor::ui_input_processor(sierra_engine_context* context)
        :_picker(context->scene_manager.active_scene()->world())
    {

    }

    //-------------------------------------------------------------------------
    bool ui_input_processor::on_input(const input_event& evt)
    {
        switch (evt.type)
        {
        case input_event_type::MOUSE_MOTION:
            return handle_mouse_motion(evt.mouse_motion);
        case input_event_type::MOUSE_PRESS:
            return handle_left_mouse_button_click(evt.mouse_button);
        }

        return false;
    }

    //-------------------------------------------------------------------------
    bool ui_input_processor::handle_mouse_motion(const mouse_motion_event& evt)
    {
        flecs::entity now = _picker.pick(evt.x, evt.y);

        // If we’re still on the same entity, we’re done.
        if (now == _hovered) 
        {
            return _hovered.is_valid();
        }

        if (_hovered.is_valid()) 
        {
            _hovered.add<ecs::hover_exit_component>();
        }

        if (now.is_valid() && now.has<ecs::hoverable_component>()) 
        {
            now.add<ecs::hover_enter_component>();
            _hovered = now;
            return true;
        }

        // Nothing hovered now
        _hovered = flecs::entity{};
        return false;
    }

    //-------------------------------------------------------------------------
    bool ui_input_processor::handle_left_mouse_button_click(const mouse_button_event& evt)
    {
        auto e = _picker.pick(evt.x, evt.y);
        if (e.is_valid())
        {
            if (e.has<ecs::picked_component>())
            {
                log::info("Unpick ui entity: {}. x: {}, y: {}", e.name().c_str(), evt.x, evt.y);
                e.remove<ecs::picked_component>();
            }
            else
            {
                log::info("Pick ui entity: {}. x: {}, y: {}", e.name().c_str(), evt.x, evt.y);
                e.add<ecs::picked_component>();

                if (e.has<ecs::clickable_component>())
                {
                    log::info("Click ui entity: {}. x: {}, y: {}", e.name().c_str(), evt.x, evt.y);
                    e.add<ecs::click_component>();
                }
            }

            return true;
        }

        return false;
    }
}