#pragma once

#include "nonogram_puzzle.h"
#include "nonogram_grid.h"

#include <vector>

namespace ppp
{
    // NONOGRAM SOLVER
    // Contains the solving logic (here using a dummy diamond pattern).
    class nonogram_solver
    {
    public:
        virtual nonogram_grid solve(const nonogram_puzzle& puzzle)
        {
            nonogram_grid solution(puzzle.grid_size, std::vector<bool>(puzzle.grid_size, false));

            int center = puzzle.grid_size / 2;

            for (int i = 0; i < puzzle.grid_size; i++)
            {
                for (int j = 0; j < puzzle.grid_size; j++)
                {
                    int dist = std::abs(i - center) + std::abs(j - center);
                    if (dist <= center - 1)
                    {
                        solution[i][j] = true;
                    }
                }
            }

            return solution;
        }

        virtual ~nonogram_solver() = default;
    };
}