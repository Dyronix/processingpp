#pragma once

#include "a-star-grid.h"

namespace ppp
{
    class grid_drawer
    {
    public:
        static vec2 s_grid_origin;

        static float s_grid_cell_size;
        static float s_grid_cell_padding;

        static void set_origin(const vec2& origin)
        {
            s_grid_origin = origin;
        }

        static void set_cell_size(float cell_size)
        {
            s_grid_cell_size = cell_size;
        }

        static void set_cell_padding(float padding)
        {
            s_grid_cell_padding = padding;
        }

        static void draw(const grid& grid)
        {
            float x = s_grid_origin.x;
            float y = s_grid_origin.y;

            fill(0);
            stroke(255);
            stroke_weight(3.0f);

            for (int c = 0; c < grid.columns(); ++c)
            {
                for (int r = 0; r < grid.rows(); ++r)
                {
                    rect(x, y, s_grid_cell_size, s_grid_cell_size);

                    x += s_grid_cell_size + s_grid_cell_padding;
                }

                x = s_grid_origin.x;
                y += s_grid_cell_size + s_grid_cell_padding;
            }

            fill(255, 0, 0, 255);
            no_stroke();

            circle(s_grid_origin.x, s_grid_origin.y, 5.0f);
        }
    };
}