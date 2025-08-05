#pragma once

#include "flecs.h"
#include "util/types.h"
#include "grid/grid_cell.h"

#include <vector>
#include <memory>
#include <unordered_map>

namespace ppp
{
  class astart_path_resolver
  {
    struct path_node
    {
      flecs::entity node;
      const path_node* parent;
      s32 fcost;
    };

  public:
    std::vector<flecs::entity> resolve(flecs::entity start, flecs::entity end);

  private:

    // Simple distance calculation that first calculates how far we have to go diagonally
    // Then adds the straight line movement distance
    s32 calc_distance(const grid_cell_component& lhs, const grid_cell_component& rhs);

    s32 calc_g_cost(flecs::entity lhs, flecs::entity rhs);

    s32 calc_h_cost(flecs::entity lhs);

    s32 hash_combine(size_t lhs, size_t rhs);

  private:
    std::unordered_map<s32, s32> m_g_cost_cache;
    flecs::entity m_target;
  };

}