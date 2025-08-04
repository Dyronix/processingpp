#pragma once

#include "events.h"

#include <flecs.h>

#include <memory>

namespace ppp
{
    class sierra_layer;

    namespace ecs
    {
        struct transform_component;
        struct bounding_box_component;
        struct pickable_component;
    }

    struct istate
    {
        istate(sierra_layer* owning_layer) 
            : _owning_layer(owning_layer) 
        {}
        virtual ~istate() = default;

        virtual void handle_key_press(key_code code) = 0;
        virtual void handle_mouse_button_click(mouse_code code) = 0;

        sierra_layer* _owning_layer;
    };

    class sierra_input_handler
    {
    public:
        sierra_input_handler(sierra_layer* layer);

        void handle_key_press(key_code code);
        void handle_mouse_button_click(mouse_code code);

    private:
        void change_to_idle();
        void change_to_placement();

        istate* _active_state;

        std::unique_ptr<istate> _idle_state;
        std::unique_ptr<istate> _placement_state;
    };
}