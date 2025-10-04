#include "sierra_ui_layer.h"

#include "ecs/systems/ecs_button_click_system.h"
#include "ecs/systems/ecs_button_hover_system.h"

#include "util/log.h"

#include "shapes.h"
#include "camera.h"
#include "environment.h"
#include "events.h"
#include "material.h"

#include <flecs.h>

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_ui_layer::sierra_ui_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "ui"_sid, 500, false)
    {
    }

    //-------------------------------------------------------------------------
    void sierra_ui_layer::on_attached()
    {
        // Nothing to implement
    }
    //-------------------------------------------------------------------------
    void sierra_ui_layer::on_enable()
    {
        create_system(&ecs::register_button_click_system);
        create_system(&ecs::register_button_hover_enter_system);
        create_system(&ecs::register_button_hover_exit_system);

        create_button("button0", { 400.0f, 200.0f }, { 255, 0, 0, 255 });
        create_button("button1", { 600.0f, 200.0f }, { 0, 255, 0, 255 });
        create_button("button2", { 800.0f, 200.0f }, { 0, 0, 255, 255 });
    }

    //-------------------------------------------------------------------------
    void sierra_ui_layer::create_button(const char* tag, const glm::vec2& position, const glm::vec4& color)
    {
        auto e = create_entity(tag);
        e.set<ecs::rect_transform_component>({
            position,                   /*.position */
            {1.0f, 1.0f},               /*.scale */
            0.0f                        /*.rotation */
            });
        e.set<ecs::render_properties_component>({ true });
        e.set<ecs::shape_component>({[]() { ppp::rect(150.0f, 150.0f); }});
        e.set<ecs::fill_color_component>({ color });
        e.set<ecs::material_component>({ material::tags::unlit::ui_texture(), false, false });
        e.add<ecs::pickable_component>();
        e.set<ecs::hoverable_component>({
            []() {log::info("button hover enter."); },
            []() {log::info("button hover exit."); }
        });
        e.set<ecs::clickable_component>({
            []() {log::info("button click.");}
        });

        f32 half_extent = 150.0f * 0.5f;
        e.set<ecs::bounding_box_2d_component>({ glm::vec2(-half_extent), glm::vec2(half_extent) });
    }
}