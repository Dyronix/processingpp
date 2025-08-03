#include "sierra_main_layer.h"

#include "ecs/systems/ecs_systems.h"
#include "ecs/ecs_pipeline_tags.h"

#include "shapes.h"
#include "environment.h"
#include "util/log.h"

#include <optional>

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
        spawn_camera();
        spawn_enemy();
        spawn_tower();
        spawn_trigger();

        init_systems();
    }

    void sierra_main_layer::spawn_camera()
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
    void sierra_main_layer::spawn_enemy()
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
      e_enemy.set<enemy_component>({ _enemy_radius * 0.5f });
      e_enemy.set<enemy_state>({ 100 });
    }
    void sierra_main_layer::spawn_tower()
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
      e_tower.set<tower_component>({ 2.0f, 1000.0f });
      e_tower.set<tower_state>({ 0.0f });
    }
    void sierra_main_layer::spawn_bullet(const glm::vec3& pos, const glm::vec3 targetPos)
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
      e_bullet.set<bullet_component>({ _bullet_radius, 2000.0f, 20 });
      e_bullet.set<bullet_state>({ glm::normalize(targetPos - pos) });
    }
    void sierra_main_layer::spawn_trigger()
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
      e_trigger.add<enemy_goal_trigger_component>();
    }
    void sierra_main_layer::init_systems()
    {
      create_system(&ecs::register_orbit_camera_system);

      create_system([this](flecs::world& world)
        {
          return world.system<ecs::transform_component, const enemy_component, enemy_state>("enemy update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](ecs::transform_component& t, const enemy_component& ec, enemy_state& es)
              {
                move_enemy(t);
                detect_bullet(world, t, ec, es);
                detect_end(world, t, ec);
              });
        });
      create_system([this](flecs::world& world)
        {
          return world.system<const enemy_state>("enemy destruction")
            .kind<ecs::tick_pipeline>()
            .with<enemy_component>()
            .each(
              [this, world](flecs::entity enemyEntity, const enemy_state& es)
              {
                destroy_enemy(enemyEntity, es);
              }
            );
        });

      create_system([this](flecs::world& world)
        {
          return world.system<const ecs::transform_component, const tower_component, tower_state>("tower update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](const ecs::transform_component& t, const tower_component& tc, tower_state& ts)
              {
                tower_select_target(world, t, tc, ts);
                tower_shoot_update(world, t, tc, ts);
              });
        });

      create_system([this](flecs::world& world)
        {
          return world.system<const bullet_component, const bullet_state, ecs::transform_component>("bullet update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](const bullet_component& bc, const bullet_state& bs, ecs::transform_component& t)
              {
                t.position += bs.direction * bc.speed * world.delta_time();
              });
        });
    }

    void sierra_main_layer::move_enemy(ecs::transform_component& enemyTransform)
    {
      enemyTransform.position.y += sin(_current_time);
    }
    void sierra_main_layer::detect_bullet(const flecs::world& world, ecs::transform_component& enemyTransform, const enemy_component& ec, enemy_state& es)
    {
      for_each_bullet(world, [&](flecs::entity bulletEntity)
        {
          ecs::transform_component trigger_transform = bulletEntity.get<ecs::transform_component>();
          bullet_component bc = bulletEntity.get<bullet_component>();
          f32 distance = glm::length((trigger_transform.position - enemyTransform.position));
          if (distance < (ec.radius + bc.radius))
          {
            bulletEntity.destruct();
            es.health -= bc.damage;
            //ppp::log::info("overlapping!");
          }
        });
    }
    void sierra_main_layer::detect_end(const flecs::world& world, ecs::transform_component& enemyTransform, const enemy_component& ec)
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
    void sierra_main_layer::destroy_enemy(flecs::entity enemyEntity, const enemy_state& es)
    {
      if (es.health <= 0)
      {
        enemyEntity.destruct();
      }
    }

    void sierra_main_layer::tower_shoot_update(const flecs::world& world, const ecs::transform_component& t, const tower_component& tc, tower_state& ts)
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
        static int bullet_counter = 0;
        std::string bullet_tag = "bullet_component_" + std::to_string(bullet_counter++);
      }
    }
    void sierra_main_layer::tower_select_target(const flecs::world& world, const ecs::transform_component& t, const tower_component& tc, tower_state& ts)
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
      // Nothing to implement
      _current_time += dt;
    }
}