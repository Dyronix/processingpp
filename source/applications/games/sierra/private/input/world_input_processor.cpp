#include "input/world_input_processor.h"

#include "ecs/components/ecs_transform_component.h"
#include "ecs/components/ecs_bounding_box_component.h"
#include "ecs/components/ecs_sorting_layer_component.h"
#include "ecs/components/ecs_pickable_component.h"
#include "ecs/components/ecs_picked_component.h"

#include "ecs/ecs_scene_manager.h"
#include "ecs/ecs_scene.h"

#include "sierra_engine_context.h"

#include "util/log.h"

#include "render/render.h"

#include "events.h"
#include "ray.h"
#include "camera.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    world_input_processor::world_input_processor(sierra_engine_context* context)
    {
        _picking_query = context->scene_manager.active_scene()->world().query_builder<
            const ecs::transform_component,
            const ecs::bounding_box_component,
            const ecs::pickable_component>().build();
    }

    //-------------------------------------------------------------------------
    bool world_input_processor::on_input(const input_event& evt)
    {
        switch (evt.type)
        {
        case input_event_type::MOUSE_PRESS:
            return handle_left_mouse_button_click(evt.mouse_button);
        }

        return false;
    }

    //-------------------------------------------------------------------------
    bool world_input_processor::handle_left_mouse_button_click(const mouse_button_event& evt)
    {
        if (_picking_query)
        {
            ray r = screen_to_world(evt.x, evt.y, render::scissor_rect()->width, render::scissor_rect()->height);

            auto closest_entity = flecs::entity{};
            auto closest_t = std::numeric_limits<f32>::min();

            std::vector<flecs::entity> pick;
            std::vector<flecs::entity> unpick;

            _picking_query.each(
                [&](flecs::entity& e, 
                    const ecs::transform_component& transform, 
                    const ecs::bounding_box_component& box, 
                    const ecs::pickable_component& /*pickable*/)
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