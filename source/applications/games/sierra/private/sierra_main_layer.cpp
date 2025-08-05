#include "sierra_main_layer.h"

#include "ecs/systems/ecs_systems.h"

#include "ecs/ecs_pipeline_tags.h"

#include "shapes.h"
#include "environment.h"
#include "util/log.h"
#include "json.h"
#include "camera.h"

#include <algorithm>
#include <optional>
#include <sstream>

namespace ppp
{


  namespace ecs
  {
		struct grass_component {};
		struct path_component {};
		struct water_component {};
    struct begin_component { flecs::entity target; };
		struct end_component {};
	}

  class astart_path_resolver
  {
    struct path_node
    {
      flecs::entity node;
      const path_node* parent;
      s32 fcost;
    };

  public:
    std::vector<flecs::entity> resolve(flecs::entity start, flecs::entity end)
    {
      std::vector<flecs::entity> path_to_end;
      m_target = end;
      // check if current node is the destination node

      // loop over the neighbours of the current node and calculate the costs

      // g cost: cost it takes to go from the current node to the neighbour
      // h cost: cost it takes to go from the neighbours node to the end node
      // f cost: sum of g cost + h cost

      

      std::vector<std::unique_ptr<path_node>> open_nodes;
      std::vector<std::unique_ptr<path_node>> closed_nodes;
      open_nodes.push_back(std::make_unique<path_node>(path_node{start, nullptr, 0}));
      const grid_cell_component& target_cell = m_target.get<grid_cell_component>();

      path_node* end_node = nullptr;

      while (!open_nodes.empty())
      {
        auto it = std::min_element(open_nodes.begin(), open_nodes.end(), [](const std::unique_ptr<path_node>& lhs, const std::unique_ptr<path_node>& rhs)
          {
            return lhs->fcost < rhs->fcost;
          });

        // move the node from one array to the other, still keeping the pointer around
        std::unique_ptr<path_node> unique_current_path_node = std::move(*it);
        open_nodes.erase(it);
        path_node* current_path_node = closed_nodes.emplace_back(std::move(unique_current_path_node)).get();

        // exit the loop if the target is found
        if (current_path_node->node == m_target)
        {
          end_node = current_path_node;
          break;
        }

        const grid_cell_component& cell = current_path_node->node.get<grid_cell_component>();
        for (flecs::entity neighbour : cell.neighbours)
        {
          if (!neighbour.has<ecs::path_component>() && !neighbour.has<ecs::end_component>())
          {
            continue;
          }

          auto it = std::find_if(closed_nodes.cbegin(), closed_nodes.cend(),
            [neighbour](const std::unique_ptr<path_node>& node)
            {
              return node->node == neighbour;
            });
          if (it != closed_nodes.cend())
          {
            continue;
          }

          s32 hcost = calc_h_cost(neighbour);
          s32 gcost = calc_g_cost(neighbour, m_target);
          s32 fcost = hcost + gcost;
          open_nodes.push_back(std::make_unique<path_node>(path_node{ neighbour, current_path_node, fcost}));
        }
      }

      if (end_node)
      {
        const path_node* path_node = end_node;
        while (path_node)
        {
          path_to_end.push_back(path_node->node);
          path_node = path_node->parent;
        }
        std::reverse(path_to_end.begin(), path_to_end.end());
      }

      //auto find_next_cell = [&](flecs::entity cell)
      //  {
      //    const grid_cell_component& tile = cell.get<grid_cell_component>();
      //    for (flecs::entity neighbour : tile.neighbours)
      //    {
      //      if (std::find(path_to_end.cbegin(), path_to_end.cend(), neighbour) != path_to_end.cend())
      //      {
      //        continue;
      //      }

      //      const grid_cell_component& neightbour_tile = neighbour.get<grid_cell_component>();
      //      if (neighbour.has<ecs::path_component>())
      //      {
      //        return neighbour;
      //      }
      //      if (neighbour.has<ecs::end_component>())
      //      {
      //        return neighbour;
      //      }
      //    }

      //    return flecs::entity::null();
      //  };

      //const grid_cell_component& begin_cell = beginEntity.get<grid_cell_component>();
      //const ecs::begin_component& begin_component = beginEntity.get<ecs::begin_component>();
      //const grid_cell_component& target_cell = begin_component.target.get<grid_cell_component>();

      //flecs::entity current_cell = beginEntity;
      //while (current_cell.is_valid() && !current_cell.has<ecs::end_component>())
      //{
      //  path_to_end.push_back(current_cell);
      //  current_cell = find_next_cell(current_cell);
      //}
      //path_to_end.push_back(current_cell);

      for (flecs::entity path : path_to_end)
      {
        const grid_cell_component& cell = path.get<grid_cell_component>();

        log::info("path: ({}, {})", cell.get_world_location().x, cell.get_world_location().y);
      }

      return path_to_end;
    }

  private:

    // Simple distance calculation that first calculates how far we have to go diagonally
    // Then adds the straight line movement distance
    s32 calc_distance(const grid_cell_component& lhs, const grid_cell_component& rhs)
    {
      const s32 diag_move_cost = 14;
      const s32 straight_move_cost = 10;

      s32 x_distance = std::abs(lhs.get_world_location().x - rhs.get_world_location().x);
      s32 y_distance = std::abs(lhs.get_world_location().y - rhs.get_world_location().y);

      s32 remaining = std::abs(x_distance - y_distance);

      return diag_move_cost * std::min(x_distance, y_distance) + straight_move_cost * remaining;
    }

    s32 calc_g_cost(flecs::entity lhs, flecs::entity rhs)
    {
      s32 hash = hash_combine(lhs.id(), rhs.id());
      auto it = m_g_cost_cache.find(hash);
      if (it != m_g_cost_cache.cend())
      {
        return it->second;
      }

      const grid_cell_component& lhs_cell = lhs.get<grid_cell_component>();
      const grid_cell_component& rhs_cell = rhs.get<grid_cell_component>();

      s32 g_cost = lhs_cell.traveral_cost + rhs_cell.traveral_cost + calc_distance(lhs_cell, rhs_cell);
      m_g_cost_cache[hash] = g_cost;

      return g_cost;
    }

    s32 calc_h_cost(flecs::entity lhs)
    {
      const grid_cell_component& lhs_cell = lhs.get<grid_cell_component>();
      const grid_cell_component& target_cell = m_target.get<grid_cell_component>();

      return calc_distance(lhs_cell, target_cell);
    }

    s32 hash_combine(size_t lhs, size_t rhs)
    {
      rhs += 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
      lhs ^= rhs;

      return lhs;
    }
  
private:
    std::unordered_map<s32, s32> m_g_cost_cache;
    flecs::entity m_target;
  };

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
      e.set<ecs::tile_component>({ tile_type });

      switch (tile_type)
      {
      case ppp::tile_type::grass:
        e.add<ecs::grass_component>();
        break;
      case ppp::tile_type::path:
        e.add<ecs::path_component>();
        break;
      case ppp::tile_type::water:
        e.add<ecs::water_component>();
        break;
      case ppp::tile_type::begin:
        e.add<ecs::begin_component>();
        break;
      case ppp::tile_type::end:
        e.add<ecs::end_component>();
        break;
      }

      return e;
    }

    //-------------------------------------------------------------------------
    std::vector<flecs::entity> build_from_json(sierra_layer* layer, const json& level_json, const level_builder_config2& config)
    {
      std::vector<flecs::entity> entities;
      entities.reserve(level_json["matrix"].size());

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







    //-------------------------------------------------------------------------
    sierra_main_layer::sierra_main_layer(sierra_engine_context* ctx)
        :sierra_layer(ctx, "main"_sid, 0, false)
      , _current_time(0.0f)
    {}

    //-------------------------------------------------------------------------
    void sierra_main_layer::on_enable()
    {
        create_camera();

        load_level();
        init_systems();
        init_enemy_spawn_points();

        find_nav_paths();

        spawn_enemies();
    }

    void sierra_main_layer::load_level()
    {
      json demo_level = load_text_json("local:content/json/demo_level.json");

      level_builder_config2 cfg;
      cfg.tile_size = demo_level["tile-size"];
      cfg.tile_spacing = demo_level["tile-spacing"];
      cfg.xoffset = -cfg.tile_size * 2;
      cfg.zoffset = -cfg.tile_size * 2;

      s32 width = demo_level["matrix"].front().size();
      s32 height = demo_level["matrix"].size();

      flecs::entity e_grid = create_entity("grid");
      e_grid.emplace<grid>(context()->scene_manager.active_scene()->world(), demo_level["matrix"], cfg, glm::vec2{});
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
    void sierra_main_layer::create_enemy(const glm::vec3& pos, const std::vector<flecs::entity>* path)
    {
      static int enemy_counter = 0;
      std::string enemy_tag = "enemy_" + std::to_string(enemy_counter++);
      auto e_enemy = create_entity(enemy_tag.c_str());

      e_enemy.set<ecs::transform_component>({
          pos,                          /*.position */
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
      e_enemy.set<ecs::enemy_component>({ _enemy_radius * 0.5f, 50.0f });
      e_enemy.set<ecs::enemy_state>({ 100, path });
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

    void sierra_main_layer::spawn_enemies()
    {
      flecs::world& world = context()->scene_manager.active_scene()->world();
      world.query_builder()
        .with<ecs::begin_component>()
        .each([this](flecs::entity entity)
          {
            ecs::transform_component transform_comp = entity.get<ecs::transform_component>();
            transform_comp.position.y += 50;
            create_enemy(transform_comp.position, &m_start_to_path.at(entity.id()));
          });
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
    }

    void sierra_main_layer::init_enemy_spawn_points()
    {
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
    }

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

    void sierra_main_layer::find_nav_paths()
    {
      // calculate the linked path list for every node
      flecs::world& world = context()->scene_manager.active_scene()->world();
      world.query_builder()
        .with<ecs::begin_component>()
        .each([this](flecs::entity beginEntity)
          {
            astart_path_resolver resolver{};
            
            const ecs::begin_component& begin_component = beginEntity.get<ecs::begin_component>();
            std::vector<flecs::entity> path_to_end = resolver.resolve(beginEntity, begin_component.target);
            m_start_to_path[beginEntity.id()] = std::move(path_to_end);


            //std::vector<flecs::entity> path_to_end;

            //std::vector<flecs::entity> open_nodes;

            //// check if current node is the destination node

            //// loop over the neighbours of the current node and calculate the costs

            //// g cost: cost it takes to go from the current node to the neighbour
            //// h cost: cost it takes to go from the neighbours node to the end node

            //auto find_next_cell = [&](flecs::entity cell)
            //  {
            //    const grid_cell_component& tile = cell.get<grid_cell_component>();
            //    for (flecs::entity neighbour : tile.neighbours)
            //    {
            //      if (std::find(path_to_end.cbegin(), path_to_end.cend(), neighbour) != path_to_end.cend())
            //      {
            //        continue;
            //      }

            //      const grid_cell_component& neightbour_tile = neighbour.get<grid_cell_component>();
            //      if (neighbour.has<ecs::path_component>())
            //      {
            //        return neighbour;
            //      }
            //      if (neighbour.has<ecs::end_component>())
            //      {
            //        return neighbour;
            //      }
            //    }

            //    return flecs::entity::null();
            //  };

            //const grid_cell_component& begin_cell = beginEntity.get<grid_cell_component>();
            //const grid_cell_component& target_cell = begin_component.target.get<grid_cell_component>();

            //flecs::entity current_cell = beginEntity;
            //while (current_cell.is_valid() && !current_cell.has<ecs::end_component>())
            //{
            //  path_to_end.push_back(current_cell);
            //  current_cell = find_next_cell(current_cell);
            //}
            //path_to_end.push_back(current_cell);

            for (flecs::entity path : path_to_end)
            {
              const grid_cell_component& cell = path.get<grid_cell_component>();

              log::info("path: ({}, {})", cell.get_world_location().x, cell.get_world_location().y);
            }

            //m_start_to_path[beginEntity.id()] = std::move(path_to_end);
          });
    }

    void sierra_main_layer::on_tick(f32 dt)
    {
      _current_time += dt;
    }
}