#include "ecs/ecs_scene.h"
#include "ecs/ecs_pipeline_tags.h"
#include "ecs/systems/ecs_systems.h"
#include "ecs/components/ecs_components.h"

#include "material.h"

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

            // Add render properties component when shape is added
            _world.observer<shape_component>()
                .event(flecs::OnAdd)
                .each([](flecs::entity& e, shape_component&)
            {
                if (!e.has<render_properties_component>())
                {
                    e.add<render_properties_component>();
                }
                if (!e.has<material_component>())
                {
                    e.set<material_component>({ material::tags::unlit::color(), true, true });
                }
            });
            // Add render properties component when model is added
            _world.observer<model_component>()
                .event(flecs::OnAdd)
                .each([](flecs::entity& e, model_component&)
            {
                if (!e.has<render_properties_component>())
                {
                    e.add<render_properties_component>();
                }
                if (!e.has<material_component>())
                {
                    e.set<material_component>({ material::tags::unlit::color(), true, true });
                }
            });
            // Add sorting layer component when a rect transform component is added
            _world.observer<rect_transform_component>()
                .event(flecs::OnAdd)
                .each([](flecs::entity& e, rect_transform_component&)
            {
                if (!e.has<sorting_layer_component>())
                {
                    e.add<sorting_layer_component>();
                }
            });

            // init
            register_camera_init_system(_world);
            // draw
            register_shape_3d_render_system(_world);
            register_shape_2d_render_system(_world);
            register_model_render_system(_world);

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
        flecs::entity scene::create_entity(const std::string& name) const
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
        void scene::begin_frame()
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void scene::tick(f32 dt) 
        {
            if (_tick_pipeline)
            {
                _world.set_pipeline(_tick_pipeline);
                _world.progress(dt);
            }
        }

        //-------------------------------------------------------------------------
        void scene::draw() 
        {
            if (_draw_pipeline)
            {
                _world.set_pipeline(_draw_pipeline);
                _world.progress(0.0f);
            }
        }

        //-------------------------------------------------------------------------
        void scene::end_frame()
        {
            ecs_world_stats_get(_world.c_ptr(), &_world_stats);
        }
    }
}