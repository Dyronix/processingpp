#pragma once

#include <vector>

namespace ppp
{
    struct grid_cell
    {
        int id;
    };

    class grid
    {
    public:
        grid(int rows, int columns)
            :m_rows(rows)
            ,m_columns(columns)
        {
            m_cells.resize(rows * columns);
            for (int i = 0; i < rows * columns; ++i)
            {
                m_cells[i].id = i;
            }
        }

        int rows() const { return m_rows; }
        int columns() const { return m_columns; }

    private:
        std::vector<grid_cell> m_cells;

        int m_rows{ 0 };
        int m_columns{ 0 };
    };
}