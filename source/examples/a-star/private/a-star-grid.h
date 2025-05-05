#pragma once

#include "vector.h"

#include <vector>

namespace ppp
{
    struct grid_cell
    {
        int id = 0;
    };

    template<typename TGridCell>
    class grid
    {
    public:
        grid(int width, int height)
            :m_width(width)
            ,m_height(height)
        {
            m_cells.resize(width * height);
        }

        void set_cell(int index, const TGridCell& cell)
        {
            m_cells[index] = cell;
        }

        void set_cell(int x, int y, const TGridCell& cell)
        {
            set_cell(x + (y * m_width));
        }

        const TGridCell& cell(int index) const
        {
            return m_cells[index];
        }

        const TGridCell& cell(int x, int y) const
        {
            return cell(x + (y * m_width));
        }

        const std::vector<TGridCell>& cells() const
        {
            return m_cells;
        }

        int width() const { return m_width; }
        int height() const { return m_height; }

    private:
        std::vector<TGridCell> m_cells;

        int m_width{ 0 };
        int m_height{ 0 };
    };

    class iworld_grid
    {
    public:
        virtual vec2i xy(const vec2& world_position) const = 0;

        virtual vec2 origin() const = 0;

        virtual vec2 cell_world_position(int x, int y) const = 0;
        virtual vec2 cell_world_position_center(int x, int y) const = 0;

        virtual float cell_size() const = 0;

        virtual int width() const = 0;
        virtual int height() const = 0;
    };

    template<typename TGridCell = grid_cell>
    class world_grid : public iworld_grid
    {
    public:
        world_grid(const vec2& origin, float cell_size, int width, int height)
            :m_grid(width, height)
            ,m_grid_cell_size(cell_size)
            ,m_grid_origin(origin)
        {}

        void set_cell(const vec2& world_position, const TGridCell& cell)
        {
            vec2i grid_cell_coord = xy(world_position);
            set_cell(grid_cell_coord.x, grid_cell_coord.y, cell);
        }

        void set_cell(int x, int y, const TGridCell& cell)
        {
            m_grid.set_cell(x, y, cell);
        }

        const TGridCell& cell(const vec2& world_position)
        {
            vec2i grid_cell_coord = xy(world_position);
            return cell(grid_cell_coord.x, grid_cell_coord.y);
        }

        const TGridCell& cell(int x, int y)
        {
            return m_grid.cell(x, y);
        }

        vec2i xy(const vec2& world_position) const override
        {
            return
            {
                std::floor((world_position - m_grid_origin).x / m_grid_cell_size),
                std::floor((world_position - m_grid_origin).y / m_grid_cell_size)
            };
        }

        vec2 origin() const override { return m_grid_origin; }

        vec2 cell_world_position(int x, int y) const override
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
        vec2 cell_world_position_center(int x, int y) const override
        {
            vec2 world_pos = cell_world_position(x, y);
            if (world_pos == m_invalid)
            {
                return m_invalid;
            }

            return { world_pos.x + m_grid_cell_size * 0.5f, world_pos.y + m_grid_cell_size * 0.5f };
        }

        float cell_size() const override { return m_grid_cell_size; }

        int width() const override { return m_grid.width(); }
        int height() const override { return m_grid.height(); }

    private:
        static vec2 m_invalid;

        grid<TGridCell> m_grid;
        vec2 m_grid_origin;
        float m_grid_cell_size;
    };

    template<typename TGridCell>
    vec2 world_grid<TGridCell>::m_invalid = {-1, -1};
}
