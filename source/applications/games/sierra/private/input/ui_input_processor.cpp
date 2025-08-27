#include "input/ui_input_processor.h"

#include "sierra_layer.h"

#include "util/log.h"

#include "events.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    ui_input_processor::ui_input_processor(sierra_layer* layer)
    {
        _picking_query = layer->create_query<
            const ecs::rect_transform_component,
            const ecs::bounding_box_2d_component,
            const ecs::sorting_layer_component,
            const ecs::pickable_component>();
    }

    //-------------------------------------------------------------------------
    bool ui_input_processor::on_input(const input_event& evt)
    {
        switch (evt.type)
        {
        case input_event_type::MOUSE_PRESS:
            return handle_left_mouse_button_click(evt.mouse_button);
        }

        return false;
    }

    //-------------------------------------------------------------------------
    bool ui_input_processor::handle_left_mouse_button_click(const mouse_button_event& evt)
    {
        if (_picking_query)
        {
            auto closest_entity = flecs::entity{};
            auto closest_t = std::numeric_limits<s32>::min();

            _picking_query.each(
                [&](flecs::entity& e,
                    const ecs::rect_transform_component& transform,
                    const ecs::bounding_box_2d_component& box,
                    const ecs::sorting_layer_component& sorting_layer,
                    const ecs::pickable_component& /*pickable*/)
            {
                glm::vec2 aabb_min = transform.position + box.min;
                glm::vec2 aabb_max = transform.position + box.max;

                s32 t = sorting_layer.index;
                if (t > closest_t)
                {
                    if (evt.x > aabb_min.x && evt.x < aabb_max.x && evt.y > aabb_min.y && evt.y < aabb_max.y)
                    {
                        closest_entity = e;
                        closest_t = t;
                    }
                }
            });

            if (closest_entity.is_valid())
            {
                log::info("Picked entity: {}", closest_entity.name().c_str());

                if (closest_entity.has<ecs::picked_component>())
                {
                    closest_entity.remove<ecs::picked_component>();
                }
                else
                {
                    closest_entity.add<ecs::picked_component>();
                }

                return true;
            }
        }

        return false;
    }
}