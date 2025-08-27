#pragma once

#include <flecs.h>

#include <memory>

namespace ppp
{
    class sierra_layer;
    class istate;

    class sierra_input_handler
    {
    public:
        sierra_input_handler(sierra_layer* layer);
        ~sierra_input_handler();

    private:
        void register_mouse_input_events();
        void register_keyboard_input_events();

        void change_to_idle();
        void change_to_placement();

        istate* _active_state;

        std::unique_ptr<istate> _idle_state;
        std::unique_ptr<istate> _placement_state;
    };
}