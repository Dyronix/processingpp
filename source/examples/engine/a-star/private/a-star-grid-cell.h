#pragma once

#include "vector.h"

#include <vector>
#include <memory>
#include <string>

namespace ppp
{
    class igrid;

    class grid_cell
    {
    public:
        grid_cell(const igrid* source, int x, int y);

        virtual ~grid_cell() = default;

        grid_cell(const grid_cell& other) = default;
        grid_cell(grid_cell&& other) noexcept = default;
        
        grid_cell& operator=(const grid_cell& other) = default;
        grid_cell& operator=(grid_cell&& other) noexcept = default;

    public:
        void assign_neighbours(std::vector<grid_cell*>& cells);
        void clear_neighbours();

    public:
        vec2 get_world_location() const;
        vec2 get_center_world_location() const;

    public:
        virtual std::string to_string() const;

    private:
        int m_grid_x;
        int m_grid_y;

        const igrid* m_grid;

        std::vector<grid_cell*> m_neighbours;
    };

    //--------------------------------------------------------------------------
    template<typename TGridCell>
    std::unique_ptr<TGridCell> create_new_grid_cell(const igrid* grid, int x, int y)
    {
        return std::make_unique<TGridCell>(grid, x, y);
    }
}