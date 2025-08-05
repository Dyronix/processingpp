#pragma once

#include "glm/glm.hpp"
#include "util/types.h"
#include "vector.h"
#include "flecs.h"
#include "json.h"

#include "grid_cell.h"
#include "ecs/components/ecs_tile_component.h"

#include <optional>

namespace ppp
{
  struct level_builder_config2
  {
    s32 tile_size = 1;
    s32 tile_spacing = 0;
    s32 xoffset = 0;
    s32 zoffset = 0;
  };

  class grid
  {
  public:
    //-------------------------------------------------------------------------
    grid(flecs::world& world, const json& matrix, const level_builder_config2& config, const vec2& origin);

    //-------------------------------------------------------------------------
    flecs::entity create_new_cell(vec2i pos, tile_type tileType, const level_builder_config2& config);

    //-------------------------------------------------------------------------
    flecs::entity get_grid_cell_entity_at_index(const vec2i& index) const;
    //-------------------------------------------------------------------------
    const grid_cell_component* get_grid_cell_at_index(const vec2i& index) const;
    //-------------------------------------------------------------------------
    const grid_cell_component* get_grid_cell_at_world_location(const vec2& location) const;

    //-------------------------------------------------------------------------
    std::optional<vec2i> get_cell_index_from_world_location(const vec2& world_location) const;

    //-------------------------------------------------------------------------
    bool is_valid_index(const vec2i& index) const;

    //-------------------------------------------------------------------------
    int width() const;
    //-------------------------------------------------------------------------
    int height() const;

    //-------------------------------------------------------------------------
    float cell_size() const;

    //-------------------------------------------------------------------------
    const vec2& origin() const;

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
}