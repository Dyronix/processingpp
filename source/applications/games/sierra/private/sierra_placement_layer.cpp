#include "sierra_placement_layer.h"

#include "ecs/components/ecs_placement_component.h"
#include "ecs/components/ecs_placement_template_component.h"

#include "ecs/systems/ecs_placement_system.h"

#include "shapes.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_placement_layer::sierra_placement_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "placement"_sid, 1, false)
    {
    }

    //-------------------------------------------------------------------------
    void sierra_placement_layer::on_attached()
    {
        // Nothing to implement
    }
    //-------------------------------------------------------------------------
    void sierra_placement_layer::on_enable()
    {
        load_placement_template();
        load_placement_handler();

        create_system(&ecs::register_placement_system);
    }

    //-------------------------------------------------------------------------
    void sierra_placement_layer::load_placement_handler()
    {
        _placement_handler = create_entity("placement_handler");

        context()->player_state.subscribe_start_placement([&]()
        {
            _placement_handler.add<ecs::placement_component>();
            ecs::render_properties_component& properties = _placement_template.get_mut<ecs::render_properties_component>();
            properties.visible = true;
        });

        context()->player_state.subscribe_stop_placement([&]()
        {
            _placement_handler.remove<ecs::placement_component>();
            ecs::render_properties_component& properties = _placement_template.get_mut<ecs::render_properties_component>();
            properties.visible = false;
        });
    }

    //-------------------------------------------------------------------------
    void sierra_placement_layer::load_placement_template()
    {
        _placement_template = create_entity("placement_template");
        _placement_template.set<ecs::transform_component>({
            {0, 0, 0},                       /*.position */
            {1.0f, 2.0f, 1.0f},                 /*.scale */
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
            });
        _placement_template.set<ecs::render_properties_component>({ false }); // hide to start
        _placement_template.set<ecs::shape_component>({
            []() { ppp::box(50.0f, 50.0f, 50.0f); } /*.draw_fn */
            });
        _placement_template.set<ecs::fill_color_component>({
          {
            0,      /*.red */
            0,      /*.green */
            255,    /*.blue */
            255     /*.alpha */
          }});
        _placement_template.add<ecs::placement_template_component>();
    }
}