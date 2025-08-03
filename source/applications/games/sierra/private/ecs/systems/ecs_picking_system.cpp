#include "ecs/systems/ecs_orbit_camera_system.h"

#include "ecs/components/ecs_transform_component.h"
#include "ecs/components/ecs_bounding_box_component.h"
#include "ecs/components/ecs_pickable_component.h"
#include "ecs/components/ecs_picked_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "camera.h"
#include "events.h"
#include "environment.h"

#include "util/types.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        flecs::entity register_picking_system(flecs::world& world)
        {
            return world.system<const ecs::transform_component, const ecs::bounding_box_component, const ecs::pickable_component>()
                .each([](flecs::entity& e, const ecs::transform_component& transform, const ecs::bounding_box_component& box, const ecs::pickable_component& /*pickable*/)
            {
                ray r = screen_to_world(mouse_x(), mouse_y(), window_width(), window_height());

                glm::vec3 half_extent = transform.scale * 0.5f;
                glm::vec3 aabb_min = transform.position - half_extent;
                glm::vec3 aabb_max = transform.position + half_extent;

                f32 t;
                if (ray_intersects_aabb(r.origin, r.dir, aabb_min, aabb_max, t))
                {
                    if (e.has<ecs::picked_component>())
                    {
                        e.remove<ecs::picked_component>();
                    }
                    else
                    {
                        e.add<ecs::picked_component>();
                    }
                }
            });
        }
    }
}