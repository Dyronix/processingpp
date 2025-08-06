#pragma once

#include "sierra_layer.h"
#include "ecs/components/ecs_components.h"
#include "vector.h"
#include "environment.h"
#include "json.h"
#include "object_factory.h"
#include "wave_system.h"

#include <optional>
#include <unordered_map>

namespace ppp
{
  class grid_cell_component;

  class sierra_main_layer : public sierra_layer
    {
    public:
        sierra_main_layer(sierra_engine_context* ctx);

    protected:
        void on_enable() override;
        void on_tick(f32 dt) override;

    private:
      // initialization
      void init_systems();

      void load_level();
      void create_camera();

      // systems
      void move_enemy(ecs::transform_component& enemyTransform, const ecs::enemy_component& ec, ecs::enemy_state& es);
      void detect_bullet(const flecs::world& world, ecs::transform_component& enemyTransform, const ecs::enemy_component& ec, ecs::enemy_state& es);
      void detect_end(const flecs::world& world, ecs::transform_component& enemyTransform, const ecs::enemy_component& ec);
      void destroy_enemy(flecs::entity enemyEntity, const ecs::enemy_state& es);
      
      void tower_shoot_update(const flecs::world& world, const ecs::transform_component& t, const ecs::tower_component& tc, ecs::tower_state& ts);
      void tower_select_target(const flecs::world& world, const ecs::transform_component& t, const ecs::tower_component& tc, ecs::tower_state& ts);

      template <typename Func>
      void for_each_enemy_component(const flecs::world& world, const Func& func)
      {
        world.query_builder()
          .with<ecs::enemy_component>()
          .each(func);
      }
      template <typename Func>
      void for_each_end_trigger(const flecs::world& world, const Func& func)
      {
        world.query_builder()
          .with<ecs::enemy_goal_trigger_component>()
          .each(func);
      }
      template <typename Func>
      void for_each_bullet(const flecs::world& world, const Func& func)
      {
        world.query_builder()
          .with<ecs::bullet_component>()
          .each(func);
      }

    private:
      f32 _enemy_radius;
      f32 _bullet_radius;
      f32 _current_time;
      f32 _enemy_spawn_offset;
      s32 _num_lives;

      object_factory _object_factory;
      std::unique_ptr<wave_system> _wave_system;
    };
}