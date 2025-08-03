#pragma once

#include "sierra_layer.h"
#include "ecs/components/ecs_components.h"

namespace ppp
{
  class sierra_main_layer : public sierra_layer
    {
    public:
        sierra_main_layer(sierra_engine_context* ctx);

    protected:
        void on_enable() override;
        void on_tick(f32 dt) override;

    private:
      void spawn_camera();
      void spawn_enemy();
      void spawn_tower();
      void spawn_bullet(const glm::vec3& pos, const glm::vec3 targetPos);
      void spawn_trigger();
      void init_systems();

      void move_enemy(ecs::transform_component& enemyTransform);
      void detect_bullet(const flecs::world& world, ecs::transform_component& enemyTransform, const enemy_component& ec, enemy_state& es);
      void detect_end(const flecs::world& world, ecs::transform_component& enemyTransform, const enemy_component& ec);
      void destroy_enemy(flecs::entity enemyEntity, const enemy_state& es);
      
      void tower_shoot_update(const flecs::world& world, const ecs::transform_component& t, const tower_component& tc, tower_state& ts);
      void tower_select_target(const flecs::world& world, const ecs::transform_component& t, const tower_component& tc, tower_state& ts);

      template <typename Func>
      void for_each_enemy_component(const flecs::world& world, const Func& func)
      {
        world.query_builder()
          .with<enemy_component>()
          .each(func);
      }
      template <typename Func>
      void for_each_end_trigger(const flecs::world& world, const Func& func)
      {
        world.query_builder()
          .with<enemy_goal_trigger_component>()
          .each(func);
      }
      template <typename Func>
      void for_each_bullet(const flecs::world& world, const Func& func)
      {
        world.query_builder()
          .with<bullet_component>()
          .each(func);
      }

    private:
      f32 _enemy_radius = 50.0f;
      f32 _bullet_radius = 5.0f;
      f32 _current_time;
    };
}