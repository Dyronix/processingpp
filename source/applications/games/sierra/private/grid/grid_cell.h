#pragma once

#include "flecs.h"
#include "vector.h"
#include "util/types.h"
#include "glm/glm.hpp"

#include <string>

namespace ppp
{
  class grid;
  struct grid_cell_component
  {
    vec2i pos;
    grid* grid;
    f32 traveral_cost;
    std::vector<flecs::entity> neighbours;

  public:
    glm::vec2 get_world_location() const;
    std::string to_string() const;

  };
}