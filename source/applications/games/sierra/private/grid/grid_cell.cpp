#include "grid/grid_cell.h"
#include "grid/grid.h"

#include "environment.h"

namespace ppp
{
  glm::vec2 grid_cell_component::get_world_location() const
  {
    auto world_location = grid->get_grid_cell_at_world_location(pos);
    if (world_location)
    {
      return pos;
    }

    print("Cell index not found [X, Y][%d, %d]", pos.x, pos.y);
    return vec2{ 0.0f, 0.0f };
  }
  std::string grid_cell_component::to_string() const
  {
    std::string debug_text;

    debug_text += "[";
    debug_text += std::to_string(pos.x);
    debug_text += ", ";
    debug_text += std::to_string(pos.y);
    debug_text += "]";

    return debug_text;
  }

}