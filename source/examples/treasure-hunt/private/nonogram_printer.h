#pragma once

#include "nonogram_grid.h"

#include <iostream>

namespace ppp
{
    // NONOGRAM PRINTER
    // Handles printing the solution.
    class nonogram_printer
    {
    public:
        void print(const nonogram_grid& solution)
        {
            for (const auto& row : solution)
            {
                for (bool cell : row)
                {
                    std::cout << (cell ? '#' : '.');
                }
                std::cout << "\n";
            }
        }
    };
}