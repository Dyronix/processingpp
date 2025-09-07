#include "ecs/systems/ecs_free_camera_system.h"

#include "ecs/components/ecs_camera_component.h"
#include "ecs/components/ecs_camera_free_control_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "camera.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        flecs::entity register_free_camera_system(flecs::world& world)
        {
            return world.system<const camera_component, const free_control_component>()
                .kind<tick_pipeline>()
                .each([](const camera_component& cam, const free_control_component& free)
                    {
                        free_control({
                            free.look_sensitivity,  /*.look_sensitivity */
                            free.movement_speed,    /*.movement_speed */
                            free.boost_multiplier,  /*.boost_multiplier */
                            }, cam.tag);
                    });
        }
    }
}