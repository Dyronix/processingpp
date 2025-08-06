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
            _picking_ui_query = owning_layer->create_query<const ecs::rect_transform_component, const ecs::bounding_box_2d_component, const ecs::sorting_layer_component, const ecs::pickable_component>();
        }

        //-------------------------------------------------------------------------
        void handle_key_press(key_code code) override
        {
            switch (code)
            {
            case key_code::KEY_SPACE: space_button_press(); break;
            case key_code::KEY_W: w_button_press(); break;
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
            int x = mouse_x();
            int y = mouse_y();

            bool picked_entity = false;

            if (_picking_ui_query && !picked_entity)
            {
                flecs::entity closest_entity;
                s32 closest_t = std::numeric_limits<s32>::min();

                std::vector<flecs::entity> pick;
                std::vector<flecs::entity> unpick;

                _picking_ui_query.each([&](flecs::entity& e, const ecs::rect_transform_component& transform, const ecs::bounding_box_2d_component& box, const ecs::sorting_layer_component& sorting_layer, const ecs::pickable_component& /*pickable*/)
                {
                    glm::vec2 aabb_min = transform.position + box.min;
                    glm::vec2 aabb_max = transform.position + box.max;

                    s32 t = sorting_layer.index;
                    if (t > closest_t)
                    {
                        if (x > aabb_min.x && x < aabb_max.x && y > aabb_min.y && y < aabb_max.y)
                        {
                            closest_entity = e;
                            closest_t = t;
                        }
                    }
                });

                if (closest_entity.is_valid())
                {
                    picked_entity = true;
                    log::info("Picked entity: {}", closest_entity.name().c_str());
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

            if (_picking_query && !picked_entity)
            {
                ray r = screen_to_world(x, y, render::scissor_rect()->width, render::scissor_rect()->height);

                flecs::entity closest_entity;
                f32 closest_t = std::numeric_limits<f32>::max();

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
                    picked_entity = true;
                    log::info("Picked entity: {}", closest_entity.name().c_str());
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

        void w_button_press()
        {
          _owning_layer->context()->player_state.start_new_wave();
        }

        flecs::query<const class ecs::transform_component, const class ecs::bounding_box_component, const class ecs::pickable_component> _picking_query;
        flecs::query<const class ecs::rect_transform_component, const class ecs::bounding_box_2d_component, const class ecs::sorting_layer_component, const class ecs::pickable_component> _picking_ui_query;
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
            log::info("Going to try and spawn a tower");

            _owning_layer->context()->player_state.try_spawn_tower();
        }

        //-------------------------------------------------------------------------
        void space_button_press()
        {
            log::info("Going to change to placement state: {}", !_owning_layer->context()->player_state.is_placing());

            _owning_layer->context()->player_state.toggle_placement();
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