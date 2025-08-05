#include "sierra_placement_layer.h"

#include "ecs/components/ecs_placement_component.h"
#include "ecs/components/ecs_placement_template_component.h"

#include "ecs/systems/ecs_placement_system.h"

#include "util/log.h"

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
        load_placement_finisher();

        create_system(&ecs::register_placement_system);

        // Respawn towers after disabling the layer
        if (!_tower_locations.empty())
        {
            for (int i = 0; i < _tower_locations.size(); ++i)
            {
                std::string tag = std::string("dynamic_tower") + std::to_string(i);
                auto t = spawn_tower(tag.c_str(), _tower_locations[i], true);
                t.set<ecs::tower_component>({ 2.0f, 1000.0f });
                t.set<ecs::tower_state>({ 0.0f });

            }
        }
    }
    
    //-------------------------------------------------------------------------
    flecs::entity sierra_placement_layer::spawn_tower(const char* tag, const glm::vec3& position, bool visible)
    {
        auto e = create_entity(tag);
        e.set<ecs::transform_component>({
            position,                           /*.position */
            {0.8f, 2.0f, 0.8f},                 /*.scale */
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
            });
        e.set<ecs::render_properties_component>({ visible }); // hide to start
        e.set<ecs::shape_component>({
            []() { ppp::box(50.0f, 50.0f, 50.0f); } /*.draw_fn */
            });
        e.set<ecs::fill_color_component>({
          {
            0,      /*.red */
            0,      /*.green */
            255,    /*.blue */
            255     /*.alpha */
          } });

        glm::vec3 half_extents = glm::vec3(0.8f * 50.0f, 2.0f * 50.0f, 0.8f * 50.0f) * 0.5f;
        e.set<ecs::bounding_box_component>({ -half_extents,half_extents });

        return e;
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
        _placement_template = spawn_tower("placement_template");
        _placement_template.add<ecs::placement_template_component>();
    }
    //-------------------------------------------------------------------------
    void sierra_placement_layer::load_placement_finisher()
    {
        static int s_tower_count = 0;
        context()->player_state.subscribe_try_spawn_tower([&]()
        {
            auto& transform = _placement_template.get<ecs::transform_component>();

            std::string tag = std::string("dynamic_tower") + std::to_string(s_tower_count);
            log::info("Spawning tower at: {},{},{}", transform.position.x, transform.position.y, transform.position.z);
            auto t = spawn_tower(tag.c_str(), transform.position, true);
            t.set<ecs::tower_component>({ 2.0f, 1000.0f });
            t.set<ecs::tower_state>({ 0.0f });

            _tower_locations.push_back(transform.position);
            ++s_tower_count;
        });
    }
}