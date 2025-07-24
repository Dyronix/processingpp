#include "a-star-grid-cell.h"
#include "a-star-grid.h"

#include "conversions.h"
#include "environment.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    grid_cell::grid_cell(const igrid* source, int x, int y)
        :m_grid_x(x)
        ,m_grid_y(y)
        ,m_grid(source)
    {
        
    }

    //-------------------------------------------------------------------------
    void grid_cell::assign_neighbours(std::vector<grid_cell*>& cells)
    {
        m_neighbours = cells;
    }
    //-------------------------------------------------------------------------
    void grid_cell::clear_neighbours()
    {
        m_neighbours.clear();
    }

    //-------------------------------------------------------------------------
    vec2 grid_cell::get_world_location() const
    {
        auto world_location = m_grid->get_cell_center_world_location(m_grid_x, m_grid_y);
        if (world_location)
        {
            return *world_location;
        }

        print("Cell index not found [X, Y][%d, %d]", m_grid_x, m_grid_y);
        return vec2{ 0.0f, 0.0f };
    }
    //-------------------------------------------------------------------------
    vec2 grid_cell::get_center_world_location() const
    {
        auto world_location = m_grid->get_cell_center_world_location(m_grid_x, m_grid_y);
        if (world_location)
        {
            return *world_location;
        }

        print("Cell index not found [X, Y][%d, %d]", m_grid_x, m_grid_y);
        return vec2{ 0.0f, 0.0f };
    }

    //-------------------------------------------------------------------------
    std::string grid_cell::to_string() const
    {
        std::string debug_text;

        debug_text += "[";
        debug_text += to_str(m_grid_x);
        debug_text += ", ";
        debug_text += to_str(m_grid_y);
        debug_text += "]";

        return debug_text;
    }
}