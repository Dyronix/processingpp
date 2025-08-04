#include "sierra_input_handler.h"
#include "sierra_layer.h"
#include "sierra_engine_context.h"
#include "sierra_player_state.h"

#include "ecs/components/ecs_components.h"

#include "render/render.h"

#include "ray.h"
#include "camera.h"

namespace ppp
{
    struct idle_state : public istate
    {
        //-------------------------------------------------------------------------
        idle_state(sierra_layer* owning_layer) 
            : istate(owning_layer)
        {
            _picking_query = owning_layer->create_query<const ecs::transform_component, const ecs::bounding_box_component, const ecs::pickable_component>();
        }

        //-------------------------------------------------------------------------
        void handle_key_press(key_code code) override
        {
            switch (code)
            {
            case key_code::KEY_SPACE: space_button_press();
            }
        }

        //-------------------------------------------------------------------------
        void handle_mouse_button_click(mouse_code code) override
        {
            switch (code)
            {
            case mouse_code::BUTTON_LEFT: handle_left_mouse_button_click(); break;
            }
        }

        //-------------------------------------------------------------------------
        void handle_left_mouse_button_click()
        {
            ray r = screen_to_world(mouse_x(), mouse_y(), render::scissor_rect()->width, render::scissor_rect()->height);

            flecs::entity closest_entity;
            f32 closest_t = std::numeric_limits<f32>::max();

            if (_picking_query)
            {
                std::vector<flecs::entity> pick;
                std::vector<flecs::entity> unpick;

                _picking_query.each([&](flecs::entity& e, const ecs::transform_component& transform, const ecs::bounding_box_component& box, const ecs::pickable_component& /*pickable*/)
                {
                    glm::vec3 half_extent = transform.scale * 0.5f;
                    glm::vec3 aabb_min = transform.position + box.min;
                    glm::vec3 aabb_max = transform.position + box.max;

                    f32 t;
                    if (ray_intersects_aabb(r.origin, r.dir, aabb_min, aabb_max, t))
                    {
                        if (t < closest_t)
                        {
                            closest_t = t;
                            closest_entity = e;
                        }
                    }
                });

                if (closest_entity.is_valid())
                {
                    if (closest_entity.has<ecs::picked_component>())
                    {
                        closest_entity.remove<ecs::picked_component>();
                    }
                    else
                    {
                        closest_entity.add<ecs::picked_component>();
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        void space_button_press()
        {
            _owning_layer->context()->player_state.toggle_placement();
            log::info("Is in placement state: {}", _owning_layer->context()->player_state.is_placing());
        }

        flecs::query<const class ecs::transform_component, const class ecs::bounding_box_component, const class ecs::pickable_component> _picking_query;
    };
    struct placement_state : public istate
    {
        //-------------------------------------------------------------------------
        placement_state(sierra_layer* owning_layer) 
            : istate(owning_layer)
        {
        }

        //-------------------------------------------------------------------------
        void handle_key_press(key_code code) override
        {
            switch (code)
            {
            case key_code::KEY_SPACE: space_button_press();
            }
        }

        //-------------------------------------------------------------------------
        void handle_mouse_button_click(mouse_code code) override
        {
            switch (code)
            {
            case mouse_code::BUTTON_LEFT: handle_left_mouse_button_click(); break;
            }
        }

        //-------------------------------------------------------------------------
        void handle_left_mouse_button_click()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void space_button_press()
        {
            _owning_layer->context()->player_state.toggle_placement();
            log::info("Is in placement state: {}", _owning_layer->context()->player_state.is_placing());
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
    }

    //-------------------------------------------------------------------------
    void sierra_input_handler::handle_key_press(key_code code)
    {
        if (_active_state)
        {
            _active_state->handle_key_press(code);
        }
    }  
    //-------------------------------------------------------------------------
    void sierra_input_handler::handle_mouse_button_click(mouse_code code)
    {
        if (_active_state)
        {
            _active_state->handle_mouse_button_click(code);
        }
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