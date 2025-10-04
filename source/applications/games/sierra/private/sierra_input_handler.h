#pragma once

#include "sierra_input_processor.h"

#include <flecs.h>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace ppp
{
    struct input_event;

    struct istate
    {
        //-------------------------------------------------------------------------
        bool on_input(const input_event& evt);

        std::vector<std::unique_ptr<isierra_input_processor>> processors;
    };

    class sierra_input_handler
    {
    public:
        void init();

        void push_input_state(const std::string& str, std::unique_ptr<istate> state);
        void push_active_input_state(const std::string& str);

    private:
        void register_mouse_input_events();
        void register_keyboard_input_events();

        istate* _active_state;

        std::unordered_map<std::string, std::unique_ptr<istate>> _input_states;
    };
}