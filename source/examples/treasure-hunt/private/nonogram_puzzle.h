#pragma once

#include <vector>

namespace ppp
{
    // NONOGRAM PUZZLE
    // Holds the parsed puzzle data.
    struct nonogram_puzzle
    {
        int grid_size;
        std::vector<std::vector<std::vector<int>>> row_clues;
        std::vector<std::vector<std::vector<int>>> col_clues;
    };
}