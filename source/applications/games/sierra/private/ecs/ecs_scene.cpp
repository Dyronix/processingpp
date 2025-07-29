#include "ecs/ecs_scene.h"
#include "ecs/ecs_pipeline_tags.h"
#include "ecs/systems/ecs_systems.h"
#include "ecs/components/ecs_components.h"

#include <flecs.h>

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        scene::scene()
            : _world()
        {
            _init_pipeline = _world.pipeline()
                .with(flecs::System)
                .with<init_pipeline>()
                .build();
            _tick_pipeline = _world.pipeline()
                .with(flecs::System)
                .with<tick_pipeline>()
                .build();
            _draw_pipeline = _world.pipeline()
                .with(flecs::System)
                .with<draw_pipeline>()
                .build();

            _world.component<transform_component>();
            _world.component<shape_component>();
            _world.component<fill_color_component>();
            _world.component<model_component>();
            _world.component<camera_component>();
            _world.component<orbit_control_component>();

            // init
            register_camera_init_system(_world);
            // draw
            register_shape_render_system(_world);
            register_model_render_system(_world);
            // tick
            register_orbit_camera_system(_world);

#ifdef _DEBUG
            _world.import<flecs::stats>();
#endif
        }

        //-------------------------------------------------------------------------
        scene::~scene() 
        {
            // Flecs will cleanup automatically
        }

        //-------------------------------------------------------------------------
        flecs::entity scene::create_entity(const std::string& name) 
        {
            // Named entities help debugging
            return name.empty()
                ? _world.entity()
                : _world.entity(name.c_str());
        }

        //-------------------------------------------------------------------------
        void scene::init()
        {
            if (_init_pipeline)
            {
                _world.set_pipeline(_init_pipeline);
                _world.progress(0.0f);
            }
        }

        //-------------------------------------------------------------------------
        void scene::tick(f32 dt) 
        {
            if (_tick_pipeline)
            {
                _world.set_pipeline(_tick_pipeline);
                _world.progress(dt);

                ecs_world_stats_get(_world.c_ptr(), &_tick_stats);
            }
        }

        //-------------------------------------------------------------------------
        void scene::draw() 
        {
            if (_draw_pipeline)
            {
                _world.set_pipeline(_draw_pipeline);
                _world.progress(0.0f);

                ecs_world_stats_get(_world.c_ptr(), &_draw_stats);
            }
        }
    }
}