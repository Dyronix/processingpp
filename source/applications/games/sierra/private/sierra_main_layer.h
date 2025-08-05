#pragma once

#include "sierra_layer.h"
#include "ecs/components/ecs_components.h"
#include "vector.h"
#include "environment.h"
#include "json.h"

#include <optional>
#include <unordered_map>

namespace ppp
{
  class grid_cell_component;

  struct level_builder_config2
  {
    s32 tile_size = 1;
    s32 tile_spacing = 0;
    s32 xoffset = 0;
    s32 zoffset = 0;
  };

  class igrid
  {
  public:
    igrid() = default;
    virtual ~igrid() = default;

    igrid(const igrid& other) = default;
    igrid(igrid&& other) = default;

    igrid& operator=(const igrid& other) = default;
    igrid& operator=(igrid&& other) = default;

  public:
    //-------------------------------------------------------------------------
    virtual const grid_cell_component* get_grid_cell_at_index(const vec2i& index) const = 0;
    //-------------------------------------------------------------------------
    virtual const grid_cell_component* get_grid_cell_at_world_location(const glm::vec2& location) const = 0;

    //-------------------------------------------------------------------------
    virtual std::optional<vec2i> get_cell_index_from_world_location(const glm::vec2& world_location) const = 0;

    //-------------------------------------------------------------------------
    virtual bool is_valid_index(const vec2i& index) const = 0;

  public:
    //-------------------------------------------------------------------------
    virtual int width() const = 0;
    //-------------------------------------------------------------------------
    virtual int height() const = 0;

    //-------------------------------------------------------------------------
    virtual float cell_size() const = 0;

    //-------------------------------------------------------------------------
    virtual const glm::vec2& origin() const = 0;
  };

  struct grid_cell_component
  {
    vec2i pos;
    igrid* grid;
    std::vector<flecs::entity> neightbours;

  public:
    glm::vec2 get_world_location() const
    {
      auto world_location = grid->get_grid_cell_at_world_location(pos);
      if (world_location)
      {
        return pos;
      }

      print("Cell index not found [X, Y][%d, %d]", pos.x, pos.y);
      return vec2{ 0.0f, 0.0f };
    }
    std::string to_string() const
    {
      std::string debug_text;

      debug_text += "[";
      debug_text += std::to_string(pos.x);
      debug_text += ", ";
      debug_text += std::to_string(pos.y);
      debug_text += "]";

      return debug_text;
    }
  };

  class grid : public igrid
  {
  public:
    //-------------------------------------------------------------------------
    grid(flecs::world& world, const json& matrix, const level_builder_config2& config, const vec2& origin);

    //-------------------------------------------------------------------------
    flecs::entity create_new_cell(vec2i pos, tile_type tileType, const level_builder_config2& config);

    //-------------------------------------------------------------------------
    const grid_cell_component* get_grid_cell_at_index(const vec2i& index) const override;
    //-------------------------------------------------------------------------
    const grid_cell_component* get_grid_cell_at_world_location(const vec2& location) const override;

    //-------------------------------------------------------------------------
    std::optional<vec2i> get_cell_index_from_world_location(const vec2& world_location) const override;

    //-------------------------------------------------------------------------
    bool is_valid_index(const vec2i& index) const override;

    //-------------------------------------------------------------------------
    int width() const override;
    //-------------------------------------------------------------------------
    int height() const override;

    //-------------------------------------------------------------------------
    float cell_size() const override;

    //-------------------------------------------------------------------------
    const vec2& origin() const override;

    //-------------------------------------------------------------------------
    std::vector<const grid_cell_component*> cells() const;

  private:
    using grid_cell_entities = std::vector<flecs::entity>;

    //--------------------------------------------------------------------------
    std::vector<flecs::entity> get_neighbours(grid_cell_entities& cells, int x, int y, int width, int height);
    //--------------------------------------------------------------------------
    void assign_neighbours(grid_cell_entities& cells, int width, int height);

  private:
    int m_width{ 0 };
    int m_height{ 0 };
    float m_cell_size{ 0 };
    vec2 m_origin{};
    grid_cell_entities m_cells;
    flecs::world* m_world;
  };





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
      void init_enemy_spawn_points();

      void load_level();

      // entity Creation
      void create_camera();
      void create_enemy(const glm::vec3& pos, const std::vector<flecs::entity>* path);
      void create_tower();
      void create_bullet(const glm::vec3& pos, const glm::vec3 targetPos);
      void create_trigger();

      void spawn_enemies();

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
      f32 _enemy_radius = 50.0f;
      f32 _bullet_radius = 5.0f;
      f32 _current_time;

      std::vector<ecs::transform_component> _begin_transforms;
      std::unordered_map<flecs::entity_t, std::vector<flecs::entity>> m_start_to_path;
    };
}