#include "ecs/systems/ecs_orbit_camera_system.h"

#include "ecs/components/ecs_camera_component.h"
#include "ecs/components/ecs_camera_orbit_control_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "camera.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        flecs::entity register_orbit_camera_system(flecs::world& world)
        {
            return world.system<const camera_component, const orbit_control_component>()
                .kind<tick_pipeline>()
                .each([](const camera_component& cam, const orbit_control_component& orbit)
                {
                    orbit_control({
                        orbit.zoom_sensitivity, /*.zoom_sensitivity */
                        orbit.rotation_sensitivity, /*.rotation_sensitivity */
                        orbit.panning_sensitivity, /*.panning_sensitivity */
                        orbit.min_zoom, /*.min_zoom */
                        orbit.max_zoom /*.max_zoom */
                    }, cam.tag);
                });
        }
    }
}