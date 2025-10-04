#pragma once

#include "sierra_input_processor.h"

namespace ppp
{
    class sierra_engine_context;

    class placement_input_processor : public isierra_input_processor
    {
    public:
        placement_input_processor(sierra_engine_context* context);

        bool on_input(const input_event& evt) override;

    private:
        bool handle_mouse_press(const mouse_button_event& evt);
        bool handle_key_press(const keyboard_event& evt);

    private:
        sierra_engine_context* _context;
    };
}