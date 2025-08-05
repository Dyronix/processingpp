#include "ecs/systems/ecs_camera_init_system.h"

#include "ecs/components/ecs_camera_component.h"
#include "ecs/components/ecs_transform_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "camera.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        void register_camera_init_system(flecs::world& world)
        {
            flecs::system sys_entity = world.system<const transform_component, const camera_component>()
                .kind<init_pipeline>()
                .each([=](flecs::entity e, const transform_component& transform, const camera_component& camera)
            {
                if (camera.type == projection_type::PERSPECTIVE)
                {
                    perspective(camera.fovy, camera.aspect_ratio, camera.near_clip, camera.far_clip, camera.tag);
                }
                else
                {
                    ortho(camera.left, camera.right, camera.bottom, camera.top, camera.near_clip, camera.far_clip, camera.tag);
                }

                set_scene_camera(
                    transform.position.x, transform.position.y, transform.position.z,
                    0, 0, 0,
                    0, 1, 0,
                    camera.tag
                );
            });            
        }
    }
}