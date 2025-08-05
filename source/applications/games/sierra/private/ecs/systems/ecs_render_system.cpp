#include "ecs/systems/ecs_render_system.h"

#include "ecs/components/ecs_transform_component.h"
#include "ecs/components/ecs_rect_transform_component.h"
#include "ecs/components/ecs_shape_component.h"
#include "ecs/components/ecs_fill_color_component.h"
#include "ecs/components/ecs_model_component.h"
#include "ecs/components/ecs_picked_component.h" 
#include "ecs/components/ecs_render_properties_component.h"
#include "ecs/components/ecs_material_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "transform.h"
#include "shapes.h"
#include "model.h"
#include "color.h"
#include "material.h"

#include "render/render.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        void register_shape_3d_render_system(flecs::world& world)
        {
            world.system<const transform_component, const shape_component, const fill_color_component, const material_component, const render_properties_component>()
                .kind<draw_pipeline>()
                .each([](flecs::entity& e, const transform_component& t, const shape_component& s, const fill_color_component& fill, const material_component& mat, const render_properties_component& properties)
                {
                    if (!properties.visible)
                    {
                        return;
                    }

                    shader(mat.tag);

                    if (mat.enable_depth_test) { render::enable_depth_test(); }
                    else { render::disable_depth_test(); }
                    if (mat.enable_depth_write) { render::enable_depth_write(); }
                    else { render::disable_depth_write(); }

                    const auto& color = e.has<picked_component>() ? fill.highlight : fill.color;

                    ppp::fill(color.r, color.g, color.b, color.a); // if a shape has NO texture we fill it 
                    ppp::tint(color.r, color.g, color.b, color.a); // if a shape has    textures we tint it.

                    ppp::push();
                    ppp::transform(t.position, t.rotation, t.scale);                   
                    s.draw_fn();
                    ppp::pop();
                });
        }

        //-------------------------------------------------------------------------
        void register_shape_2d_render_system(flecs::world& world)
        {
            world.system<const rect_transform_component, const shape_component, const fill_color_component, const material_component, const render_properties_component>()
                .kind<draw_pipeline>()
                .each([](flecs::entity& e, const rect_transform_component& t, const shape_component& s, const fill_color_component& fill, const material_component& mat, const render_properties_component& properties)
            {
                if (!properties.visible)
                {
                    return;
                }

                shader(mat.tag);

                if (mat.enable_depth_test) { render::enable_depth_test(); }
                else { render::disable_depth_test(); }
                if (mat.enable_depth_write) { render::enable_depth_write(); }
                else { render::disable_depth_write(); }

                const auto& color = e.has<picked_component>() ? fill.highlight : fill.color;

                ppp::fill(color.r, color.g, color.b, color.a); // if a shape has NO texture we fill it 
                ppp::tint(color.r, color.g, color.b, color.a); // if a shape has    textures we tint it.

                ppp::push();
                ppp::transform(t.position, t.rotation, t.scale);
                s.draw_fn();
                ppp::pop();
            });
        }

        //-------------------------------------------------------------------------
        void register_model_render_system(flecs::world& world)
        {
            world.system<const transform_component, const model_component, const material_component, const render_properties_component>()
                .kind<draw_pipeline>()
                .each([](flecs::entity e, const transform_component& t, const model_component& m, const material_component& mat, const render_properties_component& properties)
                {
                    if (!properties.visible)
                    {
                        return;
                    }

                    shader(mat.tag);

                    ppp::push();
                    ppp::transform(t.position, t.rotation, t.scale);
                    ppp::draw(m.id);
                    ppp::pop();
                });
        }
    }
}