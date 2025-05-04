#pragma once

#include <vector>

namespace ppp
{
    struct grid_cell
    {
        int id = 0;

        int x = 0;
        int y = 0;
    };

    class grid
    {
    public:
        grid(int width, int height)
            :m_width(width)
            ,m_height(height)
        {
            m_cells.resize(width * height);
            for (int i = 0; i < width * height; ++i)
            {
                m_cells[i].id = i;
            }
        }

        const std::vector<grid_cell>& cells() const { return m_cells; }

        int width() const { return m_width; }
        int height() const { return m_height; }

    private:
        std::vector<grid_cell> m_cells;

        int m_width{ 0 };
        int m_height{ 0 };
    };

    class world_grid
    {
    public:
        world_grid(const vec2& origin, float cell_size, int width, int height)
            :m_grid(width, height)
            ,m_grid_cell_size(cell_size)
            ,m_grid_origin(origin)
        {}

        vec2 origin() const { return m_grid_origin; }

        vec2 cell_world_position(int x, int y) const
        {
            // Out of bounds
            if (   x > m_grid.width() 
                || y > m_grid.height() 
                || x < 0 
                || y < 0)
            {
                return m_invalid;
            }

            return { (m_grid_origin.x - (m_grid_cell_size * 0.5f)) + (static_cast<float>(x) * m_grid_cell_size),(m_grid_origin.y - (m_grid_cell_size * 0.5f)) + (static_cast<float>(y) * m_grid_cell_size) };
        }
        vec2 cell_world_position_center(int x, int y) const
        {
            vec2 world_pos = cell_world_position(x, y);
            if (world_pos == m_invalid)
            {
                return m_invalid;
            }

            return { world_pos.x + m_grid_cell_size * 0.5f, world_pos.y + m_grid_cell_size * 0.5f };
        }

        float cell_size() const { return m_grid_cell_size; }

        int width() const { return m_grid.width(); }
        int height() const { return m_grid.height(); }

    private:
        static vec2 m_invalid;

        grid    m_grid;
        vec2    m_grid_origin;
        float   m_grid_cell_size;
    };

    vec2 world_grid::m_invalid = {-1, -1};
}
