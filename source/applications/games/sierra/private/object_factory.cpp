#include "object_factory.h"

#include "util/types.h"
#include "ecs/components/ecs_components.h"
#include "shapes.h"

#include <string>

namespace ppp
{
  object_factory::object_factory(flecs::world& world)
    : m_world(&world)
  {}

	flecs::entity object_factory::create_tower()
	{
    static s32 tower_id = 0;
    std::string tower_tag = "tower_" + std::to_string(tower_id++);
    auto e_tower = m_world->entity(tower_tag.c_str());

    e_tower.set<ecs::transform_component>({
        {-200, 0, 0},                       // position 
        {1.0f, 2.0f, 1.0f},                 // scale 
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   // rotation 
      });
    e_tower.set<ecs::shape_component>({
        []() { ppp::box(50.0f, 50.0f, 50.0f); } // draw_fn
      });
    e_tower.set<ecs::fill_color_component>(
      {
        // normal color
        {
          0,      // red
          0,      // green
          255,    // blue
          255     // alpha
        }
      });
    e_tower.set<ecs::tower_component>({ 2.0f, 1000.0f });
    e_tower.set<ecs::tower_state>({ 0.0f });

    return e_tower;
	}
  flecs::entity object_factory::create_enemy(const glm::vec3& pos, const f32 radius, const std::vector<flecs::entity>& path)
  {
    static int enemy_counter = 0;
    std::string enemy_tag = "enemy_" + std::to_string(enemy_counter++);
    auto e_enemy = m_world->entity(enemy_tag.c_str());

    e_enemy.set<ecs::transform_component>({
        pos,                          /*.position */
        {0.5f, 0.5f, 0.5f },                 /*.scale */
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f)   /*.rotation */
      });
    e_enemy.set<ecs::shape_component>({
        [radius]() { ppp::sphere(radius); } /*.draw_fn */
      });
    e_enemy.set<ecs::fill_color_component>({
      {255,    /*.red */
        0,      /*.green */
        0,      /*.blue */
        255     /*.alpha */
        }
      });
    e_enemy.set<ecs::enemy_component>({ radius * 0.5f, 50.0f });
    e_enemy.set<ecs::enemy_state>({ 100, &path });

    return e_enemy;
  }
  flecs::entity object_factory::create_bullet(const glm::vec3& pos, f32 radius, const glm::vec3& targetPos)
  {
    static int bullet_counter = 0;
    std::string bullet_tag = "bullet_component_" + std::to_string(bullet_counter++);
    auto e_bullet = m_world->entity(bullet_tag.c_str());

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
    e_bullet.set<ecs::bullet_component>({ radius, 2000.0f, 20 });
    e_bullet.set<ecs::bullet_state>({ glm::normalize(targetPos - pos) });

    return e_bullet;
  }
}