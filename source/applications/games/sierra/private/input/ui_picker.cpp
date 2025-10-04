#include "input/ui_picker.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    ui_picker::ui_picker(flecs::world& world)
    {
        _picking_query = world.query_builder<
            const ecs::rect_transform_component,
            const ecs::bounding_box_2d_component,
            const ecs::sorting_layer_component,
            const ecs::pickable_component>().build();
    }

    //-------------------------------------------------------------------------
    flecs::entity ui_picker::pick(f32 mx, f32 my) const
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
                        if (mx > aabb_min.x && mx < aabb_max.x && my > aabb_min.y && my < aabb_max.y)
                        {
                            closest_entity = e;
                            closest_t = t;
                        }
                    }
                });

            return closest_entity;
        }
    }
}