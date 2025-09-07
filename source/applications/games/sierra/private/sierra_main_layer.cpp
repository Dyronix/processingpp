#include "sierra_main_layer.h"

#include "ecs/systems/ecs_systems.h"

#include "ecs/ecs_pipeline_tags.h"

#include "shapes.h"
#include "environment.h"
#include "util/log.h"
#include "json.h"
#include "camera.h"

#include "grid/grid.h"
#include "grid/grid_cell.h"
#include "astar_path_resolver.h"

#include <algorithm>
#include <optional>
#include <sstream>

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_main_layer::sierra_main_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "main"_sid, 0, false)
      , _enemy_radius(25.0f)
      , _bullet_radius(5.0f)
      , _current_time(0.0f)
      , _object_factory(context()->scene_manager.active_scene()->world())
      , _enemy_spawn_offset(50.0f)
      , _wave_system()
      , _num_lives(2)
    {}

    //-------------------------------------------------------------------------
    void sierra_main_layer::on_enable()
    {
        create_camera();

        init_systems();

        _wave_system = std::make_unique<wave_system>(context()->scene_manager.active_scene()->world(), wave_config{ 1.0f, _enemy_radius, _enemy_spawn_offset });
        context()->player_state.subsrcibe_start_new_wave([&]()
          {
            static s32 num_to_spawn = 5;
            _wave_system->start_new_wave(num_to_spawn++);
          });
    }
    
    //-------------------------------------------------------------------------
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
    }

    //-------------------------------------------------------------------------
    void sierra_main_layer::init_systems()
    {
      create_system(&ecs::register_orbit_camera_system);
      create_system(&ecs::register_free_camera_system);

      create_system([this](flecs::world& world)
        {
          return world.system<ecs::transform_component, const ecs::enemy_component, ecs::enemy_state>("enemy update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](ecs::transform_component& t, const ecs::enemy_component& ec, ecs::enemy_state& es)
              {
                move_enemy(t, ec, es);
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
      create_system([this](flecs::world& world)
        {
          return world.system<const ecs::transform_component, const ecs::end_component>("end trigger update")
            .kind<ecs::tick_pipeline>()
            .each(
              [this, world](const ecs::transform_component& tc, const ecs::end_component& endComp)
              {
                for_each_enemy_component(world,
                  [&](flecs::entity enemyEntity)
                  {
                    ecs::transform_component enemy_transform = enemyEntity.get<ecs::transform_component>();
                    ecs::enemy_component ec = enemyEntity.get<ecs::enemy_component>();
                    f32 distance = glm::length((tc.position - enemy_transform.position)) - _enemy_spawn_offset;
                    if (distance < ec.radius)
                    {
                      enemyEntity.destruct();
                      _num_lives--;
                      log::info("lost a life!");
                      log::info("num lives left: {}", _num_lives);

                      if (_num_lives == 0)
                      {
                        log::info("GAME OVER!!!");
                      }
                    }
                  });
              });
        });
    }

    //-------------------------------------------------------------------------
    void sierra_main_layer::move_enemy(ecs::transform_component& enemyTransform, const ecs::enemy_component& ec, ecs::enemy_state& es)
    {
      flecs::entity current_target_entity = es.path->operator[](es.path_idx);
      const ecs::transform_component* target_transform = &current_target_entity.get<ecs::transform_component>();
      
      glm::vec2 enemy_pos_2d = { enemyTransform.position.x, enemyTransform.position.z };
      glm::vec2 target_pos_2d = { target_transform->position.x, target_transform->position.z };

      f32 distance = glm::length(target_pos_2d - enemy_pos_2d);
      if (distance < 1.0f && es.path_idx + 1 < es.path->size())
      {
        es.path_idx += 1;
        current_target_entity = es.path->operator[](es.path_idx);
        target_transform = &current_target_entity.get<ecs::transform_component>();
        target_pos_2d = { target_transform->position.x, target_transform->position.z };
      }

      glm::vec2 dir = target_pos_2d - enemy_pos_2d;
      dir = glm::normalize(dir);

      glm::vec3 target_dir = { dir.x, 0.0f, dir.y };
      enemyTransform.position += target_dir * ec.speed * delta_time();
    }
    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void sierra_main_layer::destroy_enemy(flecs::entity enemyEntity, const ecs::enemy_state& es)
    {
      if (es.health <= 0)
      {
        enemyEntity.destruct();
      }
    }

    //-------------------------------------------------------------------------
    void sierra_main_layer::tower_shoot_update(const flecs::world& world, const ecs::transform_component& t, const ecs::tower_component& tc, ecs::tower_state& ts)
    {
      if (!ts.target.is_valid())
      {
        return;
      }

      f32 inv_shoot_rate = 1.0f / tc.shoot_rate;
      if (_current_time - ts.last_fire_time > inv_shoot_rate)
      {
        ts.last_fire_time = _current_time;
        ecs::transform_component target_transform = ts.target.get<ecs::transform_component>();
        _object_factory.create_bullet(t.position, _bullet_radius, target_transform.position);
      }
    }
    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    void sierra_main_layer::on_tick(f32 dt)
    {
      _current_time += dt;

      _wave_system->tick(dt);
    }
}