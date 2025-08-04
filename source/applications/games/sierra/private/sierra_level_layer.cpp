#include "sierra_level_layer.h"
#include "sierra_input_handler.h"

#include "shapes.h"
#include "environment.h"
#include "json.h"
#include "events.h"

#include <sstream>

namespace ppp
{
    //-------------------------------------------------------------------------
    struct level_builder_config
    {
        s32 tile_size = 1;
        s32 tile_spacing = 0;
        s32 xoffset = 0;
        s32 zoffset = 0;
    };

    tile_type tile_type_from_string(std::string_view type)
    {
      if (type == "grass") return ppp::tile_type::grass;
      if (type == "path") return ppp::tile_type::path;
      if (type == "water") return ppp::tile_type::water;
      if (type == "begin") return ppp::tile_type::begin;
      if (type == "end") return ppp::tile_type::end;

      return (tile_type)-1;
    }

    glm::vec4 color_from_tile_type(tile_type type)
    {
      switch (type)
      {
      case ppp::tile_type::grass:  return glm::vec4(0, 255, 0, 255);
      case ppp::tile_type::path:   return glm::vec4(245, 147, 66, 255);
      case ppp::tile_type::water:  return glm::vec4(66, 194, 245, 255);
      case ppp::tile_type::begin:  return glm::vec4(224, 66, 245, 255);
      case ppp::tile_type::end:    return glm::vec4(0, 0, 0, 0);
      default:    return glm::vec4();
      }
    }

    //-------------------------------------------------------------------------
    flecs::entity create_tile(sierra_layer* layer, s32 x, s32 z, const std::string& type, const level_builder_config& config)
    {
        std::stringstream name;
        name << type << x << z;

        flecs::entity e = layer->create_entity(name.str().c_str());

        f32 final_x = ((x * config.tile_size) + config.xoffset) + (x * config.tile_spacing);
        f32 final_y = 0.0f;
        f32 final_z = ((z * config.tile_size) + config.zoffset) + (z * config.tile_spacing);

        glm::vec3 center = { final_x, final_y, final_z };
        tile_type tile_type = tile_type_from_string(type);
        e.set<ecs::transform_component>({ center, {1, 1, 1}, glm::quat(1, 0, 0, 0) });
        e.set<ecs::shape_component>({ [=]() { box((f32)config.tile_size); } });
        e.set<ecs::fill_color_component>({ color_from_tile_type(tile_type), {255, 255, 255, 255}});

        f32 half_extent = config.tile_size * 0.5f;
        e.set<ecs::bounding_box_component>({ glm::vec3(-half_extent), glm::vec3(half_extent) });
        e.add<ecs::pickable_component>();

        return e;
    }
    //-------------------------------------------------------------------------
    std::vector<flecs::entity> build_from_json(sierra_layer* layer, const json& level_json, const level_builder_config& config)
    {
        std::vector<flecs::entity> entities;

        for (const auto& row : level_json["matrix"])
        {
            for (const auto& tile : row)
            {
                s32 x = tile["x"];
                s32 z = tile["z"];
                std::string type = tile["type"];

                auto entity = create_tile(layer, x, z, type, config);
                entities.push_back(entity);
            }
        }

        return entities;
    }

    //-------------------------------------------------------------------------
    sierra_level_layer::sierra_level_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "level"_sid, 1, false)
    {
    }

    //-------------------------------------------------------------------------
    void sierra_level_layer::on_attached()
    {
        add_mouse_pressed_callback(
            [&](mouse_code code)
        {
            _input_handler->handle_mouse_button_click(code);
        });
        
        add_key_pressed_callback(
            [&](key_code code)
        {
            _input_handler->handle_key_press(code);
        });
    }
    //-------------------------------------------------------------------------
    void sierra_level_layer::on_enable()
    {
        load_level();
        
        _input_handler = std::make_unique<sierra_input_handler>(this);
    }

    //-------------------------------------------------------------------------
    void sierra_level_layer::load_level()
    {
        json demo_level = load_text_json("local:content/json/demo_level.json");

        level_builder_config cfg;
        cfg.tile_size = demo_level["tile-size"];
        cfg.tile_spacing = demo_level["tile-spacing"];
        cfg.xoffset = -cfg.tile_size * 2;
        cfg.zoffset = -cfg.tile_size * 2;

        _level_grid = build_from_json(this, demo_level, cfg);
    }
}