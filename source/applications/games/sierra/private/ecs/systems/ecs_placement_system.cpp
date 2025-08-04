#include "ecs/systems/ecs_orbit_camera_system.h"

#include "ecs/components/ecs_placement_component.h"
#include "ecs/components/ecs_placement_template_component.h"
#include "ecs/components/ecs_transform_component.h"
#include "ecs/components/ecs_bounding_box_component.h"
#include "ecs/components/ecs_pickable_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "events.h"
#include "ray.h"
#include "environment.h"
#include "camera.h"
#include "shapes.h"

#include "render/render.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        flecs::entity register_placement_system(flecs::world& world)
        {
            auto pick_query = world.query<const ecs::transform_component, const ecs::bounding_box_component, const ecs::pickable_component>();
            auto template_query = world.query<ecs::transform_component, const ecs::bounding_box_component, const ecs::placement_template_component>();

            return world.system<const placement_component>()
                .kind<tick_pipeline>()
                .each([pick_query, template_query](const placement_component&)
            {
                ray r = screen_to_world(mouse_x(), mouse_y(), render::scissor_rect()->width, render::scissor_rect()->height);

                pick_query.each([&](const ecs::transform_component& transform, const ecs::bounding_box_component& bbox, const ecs::pickable_component&)
                {
                    glm::vec3 half_extent = transform.scale * 0.5f;
                    glm::vec3 aabb_min = transform.position + bbox.min;
                    glm::vec3 aabb_max = transform.position + bbox.max;

                    f32 t;
                    if (ray_intersects_aabb(r.origin, r.dir, aabb_min, aabb_max, t))
                    {
                        glm::vec3 center_of_hit = (aabb_min + aabb_max) * 0.5f;
                        f32 top_y = aabb_max.y;

                        template_query.each([center_of_hit, top_y](flecs::entity& e, transform_component& transform, const ecs::bounding_box_component& bbox, const placement_template_component&)
                        {
                            glm::vec3 new_position = center_of_hit;

                            f32 half_height = (bbox.max.y - bbox.min.y) * 0.5f;
                            new_position.y = top_y + half_height;

                            transform.position = new_position;
                        });
                    }
                });
            });
        }
    }
}