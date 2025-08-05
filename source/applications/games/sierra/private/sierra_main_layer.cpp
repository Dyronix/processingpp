#include "sierra_main_layer.h"

#include "ecs/systems/ecs_systems.h"

#include "ecs/ecs_pipeline_tags.h"

#include "shapes.h"
#include "environment.h"
#include "util/log.h"
#include "json.h"
#include "camera.h"

#include <optional>
#include <sstream>

namespace ppp
{
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
    
    void sierra_main_layer::create_camera()
    {
      auto e_camera = create_entity("orbit_camera");

      e_camera.set<ecs::transform_component>({
          {20, -40, 400},                     /*.position */
          {1.0f, 1.0f, 1.0f},                 /*.scale */
          glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
        });

      ecs::camera_component camera_comp;
      camera_comp.tag = camera::tags::perspective();
      camera_comp.type = ecs::projection_type::PERSPECTIVE;
      camera_comp.fovy = 55.0f;
      camera_comp.aspect_ratio = (f32)window_width() / (f32)window_height();
      camera_comp.near_clip = 0.1f;
      camera_comp.far_clip = 2000.0f;

      e_camera.set<ecs::camera_component>(camera_comp);
      e_camera.set<ecs::orbit_control_component>({
          200.0f, /*.zoom_sensitivity */
          0.5f,   /*.panning_sensitivity */
          0.5f,   /*.rotation_sensitivity */
          1.0f,   /*.min_zoom */
          600.0f  /*.max_zoom */
        });
      e_camera.add<ecs::active_camera_component>();
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