#include "sierra_main_layer.h"

#include "ecs/systems/ecs_systems.h"
#include "ecs/ecs_pipeline_tags.h"

#include "shapes.h"
#include "environment.h"
#include "util/log.h"
#include "json.h"

#include <optional>
#include <sstream>

namespace ppp
{
	struct level_builder_config2
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
  flecs::entity create_tile(sierra_layer* layer, s32 x, s32 z, const std::string& type, const level_builder_config2& config)
  {
    std::stringstream name;
    name << type << x << z << "2";

    flecs::entity e = layer->create_entity(name.str().c_str());

    f32 final_x = ((x * config.tile_size) + config.xoffset) + (x * config.tile_spacing);
    f32 final_y = 0.0f;
    f32 final_z = ((z * config.tile_size) + config.zoffset) + (z * config.tile_spacing);

    glm::vec3 center = { final_x, final_y, final_z };
    tile_type tile_type = tile_type_from_string(type);
    e.set<ecs::transform_component>({ center, {1, 1, 1}, glm::quat(1, 0, 0, 0) });
    e.set<ecs::shape_component>({ [=]() { box((f32)config.tile_size); } });
    e.set<ecs::fill_color_component>({ color_from_tile_type(tile_type), {255, 255, 255, 255} });

    f32 half_extent = config.tile_size * 0.5f;
    e.set<ecs::bounding_box_component>({ glm::vec3(-half_extent), glm::vec3(half_extent) });
    e.set<ecs::tile_component>({ tile_type });
    e.add<ecs::pickable_component>();

    return e;
  }

  //-------------------------------------------------------------------------
  std::vector<flecs::entity> build_from_json(sierra_layer* layer, const json& level_json, const level_builder_config2& config)
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
    sierra_main_layer::sierra_main_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "main"_sid, 0, false)
      , _current_time(0.0f)
    {}

    //-------------------------------------------------------------------------
    void sierra_main_layer::on_enable()
    {
        create_camera();
        create_enemy();
        create_tower();
        create_trigger();

        init_systems();
    }

    void sierra_main_layer::load_level()
    {
      json demo_level = load_text_json("local:content/json/demo_level.json");

      level_builder_config2 cfg;
      cfg.tile_size = demo_level["tile-size"];
      cfg.tile_spacing = demo_level["tile-spacing"];
      cfg.xoffset = -cfg.tile_size * 2;
      cfg.zoffset = -cfg.tile_size * 2;

      //_grid = build_from_json(this, demo_level, cfg);
    }

    void sierra_main_layer::create_camera()
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
    void sierra_main_layer::create_enemy()
    {
      auto e_enemy = create_entity("enemy");

      e_enemy.set<ecs::transform_component>({
          {100, 0, 0},                          /*.position */
          {0.5f, 0.5f, 0.5f },                 /*.scale */
          glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
        });
      e_enemy.set<ecs::shape_component>({
          [this]() { ppp::sphere(_enemy_radius); } /*.draw_fn */
        });
      e_enemy.set<ecs::fill_color_component>({
        {255,    /*.red */
          0,      /*.green */
          0,      /*.blue */
          255     /*.alpha */
          }
        });
      e_enemy.set<ecs::enemy_component>({ _enemy_radius * 0.5f });
      e_enemy.set<ecs::enemy_state>({ 100 });
    }
    void sierra_main_layer::create_tower()
    {
      auto e_tower = create_entity("tower");

      // Tower with bullet_component
      // ------------------------------------------------------------------------------------------------------------------------------------
      e_tower.set<ecs::transform_component>({
          {-200, 0, 0},                          /*.position */
          {1.0f, 2.0f, 1.0f},                 /*.scale */
          glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
        });
      e_tower.set<ecs::shape_component>({
          []() { ppp::box(50.0f, 50.0f, 50.0f); } /*.draw_fn */
        });
      e_tower.set<ecs::fill_color_component>({
        {
          0,      /*.red */
          0,      /*.green */
          255,    /*.blue */
          255     /*.alpha */
        }
        });
      e_tower.set<ecs::tower_component>({ 2.0f, 1000.0f });
      e_tower.set<ecs::tower_state>({ 0.0f });
    }
    void sierra_main_layer::create_bullet(const glm::vec3& pos, const glm::vec3 targetPos)
    {
			static int bullet_counter = 0;
			std::string bullet_tag = "bullet_component_" + std::to_string(bullet_counter++);
      auto e_bullet = create_entity(bullet_tag.c_str());

      e_bullet.set<ecs::transform_component>({
          pos,                          /*.position */
          {0.1f, 0.1f, 0.1f },                 /*.scale */
          glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
        });
      e_bullet.set<ecs::shape_component>({
          []() { ppp::sphere(50.0f); } /*.draw_fn */
        });
      e_bullet.set<ecs::fill_color_component>({
        {255,    /*.red */
          255,      /*.green */
          255,      /*.blue */
          255     /*.alpha */
          }
        });
      e_bullet.set<ecs::bullet_component>({ _bullet_radius, 2000.0f, 20 });
      e_bullet.set<ecs::bullet_state>({ glm::normalize(targetPos - pos) });
    }
    void sierra_main_layer::create_trigger()
    {
      auto e_trigger = create_entity("trigger");
      e_trigger.set<ecs::transform_component>({
          {200, 0, 0},                          /*.position */
          {0.5f, 0.5f, 0.5f },                 /*.scale */
          glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
        });
      e_trigger.set<ecs::shape_component>({
          []() { ppp::box(75.0f, 75.0f, 75.0f); } /*.draw_fn */
        });
      e_trigger.set<ecs::fill_color_component>({
        {
          255,    /*.red */
          255,      /*.green */
          255,      /*.blue */
          0     /*.alpha */
          }
        });
      e_trigger.add<ecs::enemy_goal_trigger_component>();
    }
    void sierra_main_layer::init_systems()
    {
      create_system(&ecs::register_orbit_camera_system);
      create_system([this](flecs::world& world)
        {
          return world.system<ecs::transform_component, const ecs::enemy_component, ecs::enemy_state>("enemy update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](ecs::transform_component& t, const ecs::enemy_component& ec, ecs::enemy_state& es)
              {
                move_enemy(t);
                detect_bullet(world, t, ec, es);
                detect_end(world, t, ec);
              });
        });
      create_system([this](flecs::world& world)
        {
          return world.system<const ecs::enemy_state>("enemy destruction")
            .kind<ecs::tick_pipeline>()
            .with<ecs::enemy_component>()
            .each(
              [this, world](flecs::entity enemyEntity, const ecs::enemy_state& es)
              {
                destroy_enemy(enemyEntity, es);
              }
            );
        });
      create_system([this](flecs::world& world)
        {
          return world.system<const ecs::transform_component, const ecs::tower_component, ecs::tower_state>("tower update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](const ecs::transform_component& t, const ecs::tower_component& tc, ecs::tower_state& ts)
              {
                tower_select_target(world, t, tc, ts);
                tower_shoot_update(world, t, tc, ts);
              });
        });
      create_system([this](flecs::world& world)
        {
          return world.system<const ecs::bullet_component, const ecs::bullet_state, ecs::transform_component>("bullet update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](const ecs::bullet_component& bc, const ecs::bullet_state& bs, ecs::transform_component& t)
              {
                t.position += bs.direction * bc.speed * world.delta_time();
              });
        });
    }

    void sierra_main_layer::move_enemy(ecs::transform_component& enemyTransform)
    {
      enemyTransform.position.y += sin(_current_time);
    }
    void sierra_main_layer::detect_bullet(const flecs::world& world, ecs::transform_component& enemyTransform, const ecs::enemy_component& ec, ecs::enemy_state& es)
    {
      for_each_bullet(world, [&](flecs::entity bulletEntity)
        {
          ecs::transform_component trigger_transform = bulletEntity.get<ecs::transform_component>();
          ecs::bullet_component bc = bulletEntity.get<ecs::bullet_component>();
          f32 distance = glm::length((trigger_transform.position - enemyTransform.position));
          if (distance < (ec.radius + bc.radius))
          {
            bulletEntity.destruct();
            es.health -= bc.damage;
            //ppp::log::info("overlapping!");
          }
        });
    }
    void sierra_main_layer::detect_end(const flecs::world& world, ecs::transform_component& enemyTransform, const ecs::enemy_component& ec)
    {
      for_each_end_trigger(world, [&](flecs::entity endTriggerEntity)
        {
          ecs::transform_component trigger_transform = endTriggerEntity.get<ecs::transform_component>();
          f32 distance = glm::length((trigger_transform.position - enemyTransform.position));
          if (distance < ec.radius)
          {
            //ppp::log::info("overlapping!");
          }
        });
    }
    void sierra_main_layer::destroy_enemy(flecs::entity enemyEntity, const ecs::enemy_state& es)
    {
      if (es.health <= 0)
      {
        enemyEntity.destruct();
      }
    }

    void sierra_main_layer::tower_shoot_update(const flecs::world& world, const ecs::transform_component& t, const ecs::tower_component& tc, ecs::tower_state& ts)
    {
      if (!ts.target.is_valid())
      {
        return;
      }

      f32 inv_shoot_rate = 1.0f / tc.shoot_rate;
      if (_current_time - ts.last_fire_time > inv_shoot_rate)
      {
        ppp::log::info("Shooting!");
        ts.last_fire_time = _current_time;
        ecs::transform_component target_transform = ts.target.get<ecs::transform_component>();
        create_bullet(t.position, target_transform.position);
      }
    }
    void sierra_main_layer::tower_select_target(const flecs::world& world, const ecs::transform_component& t, const ecs::tower_component& tc, ecs::tower_state& ts)
    {
      if (ts.target.is_valid())
      {
        ecs::transform_component enemy_tranform = ts.target.get<ecs::transform_component>();
        f32 distance = glm::length(enemy_tranform.position - t.position);

        if (distance < tc.range)
        {
          return;
        }
        else
        {
          ts.target = flecs::entity::null();
        }
      }

      std::optional<flecs::entity> closest_enemy;
      f32 min_distance = std::numeric_limits<f32>::max();
      for_each_enemy_component(world, [&](flecs::entity enemyEntity)
        {
          ecs::transform_component enemy_tranform = enemyEntity.get<ecs::transform_component>();
          f32 distance = glm::length(enemy_tranform.position - t.position);
          if (distance < tc.range && (!closest_enemy.has_value() || distance < min_distance))
          {
            closest_enemy = enemyEntity;
            min_distance = distance;
          }
        });

      if (closest_enemy)
      {
        ts.target = closest_enemy.value();
      }
    }

    void sierra_main_layer::on_tick(f32 dt)
    {
      _current_time += dt;

      static bool has_init = false;
      if (!has_init)
      {
        // Initialize all the tiles so we know where to spawn the enemy from and we know which direction he has to go in
        flecs::world& world = context()->scene_manager.active_scene()->world();
        world.query_builder()
          .with<ecs::tile_component>()
          .each([&](const flecs::entity tileEntity)
            {
              ecs::tile_component tile_comp = tileEntity.get<ecs::tile_component>();

              if (tile_comp.type == tile_type::begin)
              {
                ecs::transform_component transform = tileEntity.get<ecs::transform_component>();
                _begin_transforms.push_back(transform);
              }
            });

        has_init = true;
      }
    }
}