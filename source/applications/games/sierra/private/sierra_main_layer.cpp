#include "sierra_main_layer.h"

#include "ecs/components/ecs_components.h"
#include "ecs/systems/ecs_systems.h"

#include "shapes.h"
#include "environment.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_main_layer::sierra_main_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "main"_sid, 0, false)
    {}

    //-------------------------------------------------------------------------
    void sierra_main_layer::on_enable()
    {
        // Make Camera
        {
            auto e_camera = create_entity("orbit_camera");

            e_camera.set<ecs::transform_component>({
                {20, -40, 400},                     /*.position */
                {1.0f, 1.0f, 1.0f},                 /*.scale */
                glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
                });
            e_camera.set<ecs::camera_component>({
                "",                                         /*.tag */
                ecs::projection_type::PERSPECTIVE,          /*.type */
                55.0f,                                      /*.fovy */
                (f32)window_width() / (f32)window_height(), /*.aspect_ratio */
                0.1f,                                       /*.near_clip */
                2000.0f                                     /*.far_clip */
                });
            e_camera.set<ecs::orbit_control_component>({
                200.0f, /*.zoom_sensitivity */
                0.5f,   /*.panning_sensitivity */
                0.5f,   /*.rotation_sensitivity */
                1.0f,   /*.min_zoom */
                600.0f  /*.max_zoom */
                });
        }

        create_system(&ecs::register_orbit_camera_system);
    }
}