#pragma once

#include "sierra_input_processor.h"

#include "ecs/components/ecs_components.h"

#include "input/ui_picker.h"

#include <flecs.h>

namespace ppp
{
    class sierra_engine_context;

    struct mouse_motion_event;
    struct mouse_button_event;

    class ui_input_processor : public isierra_input_processor
    {
    public:
        ui_input_processor(sierra_engine_context* context);

        bool on_input(const input_event& evt);

    private:
        bool handle_mouse_motion(const mouse_motion_event& evt);
        bool handle_left_mouse_button_click(const mouse_button_event& evt);

        ui_picker _picker;

        flecs::entity _hovered;
    };
}