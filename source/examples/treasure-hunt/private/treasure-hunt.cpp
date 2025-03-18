#include "engine.h"

#include "nonogram_parser.h"
#include "nonogram_puzzle.h"
#include "nonogram_solver.h"
#include "nonogram_printer.h"
#include "nonogram_grid.h"

#include <sstream>

namespace ppp
{
    constexpr int window_width = 1280;
    constexpr int window_height = 720;
    constexpr int canvas_width = 600;
    constexpr int canvas_height = 600;

    app_params entry(int argc, char** argv)
    {
        environment::print("Current working directory: %s", environment::cwd().data());
        app_params app_parameters;
        app_parameters.window_width = 1280;
        app_parameters.window_height = 720;
        return app_parameters;
    }

    void setup()
    {
        auto treasure_path = "local:/content/TREASU_1.BIN";
        auto treasure_data = file::load_binary_file(treasure_path);

        nonogram_parser  parser;
        nonogram_puzzle  puzzle;

        try
        {
            puzzle = parser.parse(treasure_data);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error while parsing: " << ex.what() << "\n";
            return;
        }

        nonogram_solver  solver;
        nonogram_grid    solution = solver.solve(puzzle);

        nonogram_printer printer;
        
        std::cout << "\nNonogram Solution:\n";
        printer.print(solution);
    }

    void draw()
    {
    }

} // namespace ppp
