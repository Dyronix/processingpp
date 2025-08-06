#include "astar_path_resolver.h"

#include "environment.h"
#include "ecs/components/ecs_components.h"
#include "util/log.h"

namespace ppp
{
  std::vector<flecs::entity> astart_path_resolver::resolve(flecs::entity start, flecs::entity end)
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
    open_nodes.push_back(std::make_unique<path_node>(path_node{ start, nullptr, 0 }));
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
        open_nodes.push_back(std::make_unique<path_node>(path_node{ neighbour, current_path_node, fcost }));
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

    return path_to_end;
  }

  // Simple distance calculation that first calculates how far we have to go diagonally
  // Then adds the straight line movement distance
  s32 astart_path_resolver::calc_distance(const grid_cell_component& lhs, const grid_cell_component& rhs)
  {
    const s32 diag_move_cost = 14;
    const s32 straight_move_cost = 10;

    s32 x_distance = std::abs(lhs.get_world_location().x - rhs.get_world_location().x);
    s32 y_distance = std::abs(lhs.get_world_location().y - rhs.get_world_location().y);

    s32 remaining = std::abs(x_distance - y_distance);

    return diag_move_cost * std::min(x_distance, y_distance) + straight_move_cost * remaining;
  }

  s32 astart_path_resolver::calc_g_cost(flecs::entity lhs, flecs::entity rhs)
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

  s32 astart_path_resolver::calc_h_cost(flecs::entity lhs)
  {
    const grid_cell_component& lhs_cell = lhs.get<grid_cell_component>();
    const grid_cell_component& target_cell = m_target.get<grid_cell_component>();

    return calc_distance(lhs_cell, target_cell);
  }

  s32 astart_path_resolver::hash_combine(size_t lhs, size_t rhs)
  {
    rhs += 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    lhs ^= rhs;

    return lhs;
  }

}