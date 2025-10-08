#pragma once

#include "sierra_input_processor.h"

namespace ppp
{
    class sierra_engine_context;

    class switch_scene_input_processor : public isierra_input_processor
    {
    public:
        switch_scene_input_processor(sierra_engine_context* context);

        bool on_input(const input_event& evt) override;

    private:
        bool handle_key_press(const keyboard_event& evt);

    private:
        sierra_engine_context* _context;
    };
}