#include "grid/grid.h"

#include "ecs/components/ecs_components.h"

namespace ppp
{
  namespace
  {
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
  }

  //-------------------------------------------------------------------------
  grid::grid(flecs::world& world, const json& matrix, const level_builder_config2& config, const vec2& origin)
  {
    m_world = &world;
    //m_width = width;
    //m_height = height;
    m_cell_size = config.tile_size;
    m_origin = origin;

    for (const auto& row : matrix)
    {
      for (const auto& tile : row)
      {
        s32 x = tile["x"];
        s32 z = tile["z"];
        std::string_view type = tile["type"];
        tile_type tile_type = tile_type_from_string(type);
        flecs::entity new_cell = create_new_cell({ x, z }, tile_type, config);
        m_cells.push_back(new_cell);
      }

      m_width = matrix.size();
      m_height = row.size();
    }

    // second pass to initialize cells that reference others
    for (const auto& row : matrix)
    {
      for (const auto& tile : row)
      {
        glm::vec2 pos{};
        pos.x = tile["x"];
        pos.y = tile["z"];
        std::string_view type = tile["type"];
        tile_type tile_type = tile_type_from_string(type);

        flecs::entity cell_entity = get_grid_cell_entity_at_index(pos);

        if (tile_type == tile_type::begin)
        {
          ecs::begin_component& begin_comp = cell_entity.get_mut<ecs::begin_component>();
          glm::vec2 target_pos{};
          target_pos.x = tile["target"]["x"];
          target_pos.y = tile["target"]["z"];
          begin_comp.target = get_grid_cell_entity_at_index(target_pos);
        }
      }
    }

    assign_neighbours(m_cells, m_width, m_height);
  }

  //-------------------------------------------------------------------------
  flecs::entity grid::create_new_cell(vec2i pos, tile_type tileType, const level_builder_config2& config)
  {
    std::string cell_name;
    cell_name += "cell_";
    cell_name += std::to_string(pos.x);
    cell_name += "_";
    cell_name += std::to_string(pos.y);

    auto e_cell = m_world->entity(cell_name.c_str());
    f32 final_x = ((pos.x * config.tile_size) + config.xoffset) + (pos.x * config.tile_spacing);
    f32 final_y = 0.0f;
    f32 final_z = ((pos.y * config.tile_size) + config.zoffset) + (pos.y * config.tile_spacing);

    glm::vec3 center = { final_x, final_y, final_z };
    e_cell.set<ecs::transform_component>({ center, {1, 1, 1}, glm::quat(1, 0, 0, 0) });
    e_cell.set<ecs::tile_component>({ tileType });

    switch (tileType)
    {
    case ppp::tile_type::grass:
      e_cell.add<ecs::grass_component>();
      break;
    case ppp::tile_type::path:
      e_cell.add<ecs::path_component>();
      break;
    case ppp::tile_type::water:
      e_cell.add<ecs::water_component>();
      break;
    case ppp::tile_type::begin:
      e_cell.add<ecs::begin_component>();
      break;
    case ppp::tile_type::end:
      e_cell.add<ecs::end_component>();
      break;
    }
    e_cell.set<grid_cell_component>(
      {
        pos,
        this,
        1.0f // traveral cost
      });

    return e_cell;
  }

  //-------------------------------------------------------------------------
  flecs::entity grid::get_grid_cell_entity_at_index(const vec2i& index) const
  {
    if (is_valid_index(index) == false)
    {
      return flecs::entity::null();
    }

    int linear_index = index.x + (m_width * index.y);

    return m_cells[linear_index];
  }
  //-------------------------------------------------------------------------
  const grid_cell_component* grid::get_grid_cell_at_index(const vec2i& index) const
  {
    flecs::entity entity = get_grid_cell_entity_at_index(index);
    if (!entity.is_valid())
    {
      return nullptr;
    }

    return &entity.get<grid_cell_component>();
  }
  //-------------------------------------------------------------------------
  const grid_cell_component* grid::get_grid_cell_at_world_location(const vec2& location) const
  {
    auto index = get_cell_index_from_world_location(location);
    if (!index)
    {
      return nullptr;
    }

    return get_grid_cell_at_index(*index);
  }

  //-------------------------------------------------------------------------
  std::optional<vec2i> grid::get_cell_index_from_world_location(const vec2& world_location) const
  {
    vec2i index;

    index.x = std::floor((world_location - m_origin).x / m_cell_size);
    index.y = std::floor((world_location - m_origin).y / m_cell_size);

    if (is_valid_index(index) == false)
    {
      return {};
    }

    return index;
  }

  //-------------------------------------------------------------------------
  bool grid::is_valid_index(const vec2i& index) const
  {
    return index.x >= 0 && index.y >= 0 && index.x < m_width && index.y < m_height;
  }

  //-------------------------------------------------------------------------
  int grid::width() const
  {
    return m_width;
  }
  //-------------------------------------------------------------------------
  int grid::height() const
  {
    return m_height;
  }

  //-------------------------------------------------------------------------
  float grid::cell_size() const
  {
    return m_cell_size;
  }

  //-------------------------------------------------------------------------
  const glm::vec2& grid::origin() const
  {
    return m_origin;
  }

  //-------------------------------------------------------------------------
  std::vector<const grid_cell_component*> grid::cells() const
  {
    std::vector<const grid_cell_component*> all_cells;
    all_cells.reserve(m_cells.size());

    for (auto& cell : m_cells)
    {
      all_cells.push_back(&cell.get<grid_cell_component>());
    }

    return all_cells;
  }

  //--------------------------------------------------------------------------
  std::vector<flecs::entity> grid::get_neighbours(grid_cell_entities& cells, int x, int y, int width, int height)
  {
    auto get_cell = [&cells, width](int x, int y)
      {
        int index = x + (y * width);

        return cells[index];
      };

    std::vector<flecs::entity> neighbours;

    if (x - 1 >= 0)
    {
      // left
      neighbours.push_back(get_cell(x - 1, y));
      //// left down
      //if (y - 1 >= 0) neighbours.push_back(get_cell(x - 1, y - 1));
      //// left up
      //if (y + 1 < height) neighbours.push_back(get_cell(x - 1, y + 1));
    }

    if (x + 1 < width)
    {
      // right
      neighbours.push_back(get_cell(x + 1, y));
      //// right down
      //if (y - 1 >= 0) neighbours.push_back(get_cell(x + 1, y - 1));
      //// right up
      //if (y + 1 < height) neighbours.push_back(get_cell(x + 1, y + 1));
    }

    // down
    if (y - 1 >= 0) neighbours.push_back(get_cell(x, y - 1));
    // up
    if (y + 1 < height) neighbours.push_back(get_cell(x, y + 1));

    return neighbours;
  }
  //--------------------------------------------------------------------------
  void grid::assign_neighbours(grid_cell_entities& cells, int width, int height)
  {
    for (int x = 0; x < width; ++x)
    {
      for (int y = 0; y < height; ++y)
      {
        int index = x + (y * width);

        grid_cell_component grid_cell = cells[index].get<grid_cell_component>();
        grid_cell.neighbours = get_neighbours(cells, x, y, width, height);
        cells[index].set(grid_cell);
      }
    }
  }

}