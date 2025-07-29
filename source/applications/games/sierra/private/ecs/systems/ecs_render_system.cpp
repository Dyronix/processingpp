#include "ecs/systems/ecs_render_system.h"

#include "ecs/components/ecs_transform_component.h"
#include "ecs/components/ecs_shape_component.h"
#include "ecs/components/ecs_fill_color_component.h"
#include "ecs/components/ecs_model_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "transform.h"
#include "shapes.h"
#include "model.h"
#include "color.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        void register_shape_render_system(flecs::world& world)
        {
            world.system<const transform_component, const shape_component, const fill_color_component>()
                .kind<draw_pipeline>()
                .each([](flecs::entity e, const transform_component& t, const shape_component& s, const fill_color_component& fill) 
                {
                    ppp::fill(fill.r, fill.g, fill.b, fill.a);

                    ppp::push();
                    ppp::transform(t.position, t.rotation, t.scale);                   
                    s.draw_fn();
                    ppp::pop();
                });
        }

        //-------------------------------------------------------------------------
        void register_model_render_system(flecs::world& world)
        {
            world.system<const transform_component, const model_component>()
                .kind<draw_pipeline>()
                .each([](flecs::entity e, const transform_component& t, const model_component& m) 
                {
                    ppp::push();
                    ppp::transform(t.position, t.rotation, t.scale);
                    ppp::draw(m.id);
                    ppp::pop();
                });
        }
    }
}