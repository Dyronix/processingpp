#pragma once

#include "a-star-grid.h"

#include "vector.h"
#include "conversions.h"

namespace ppp
{
    class grid_drawer
    {
    public:
        static void draw(const world_grid& grid)
        {
            float x = grid.origin().x;
            float y = grid.origin().y;

            //fill(0);
            //inner_stroke(255);
            //inner_stroke_weight(3.0f);

            //for (int h = 0; h < grid.height(); ++h)
            //{
            //    for (int w = 0; w < grid.width(); ++w)
            //    {
            //        rect(x, y, grid.cell_size(), grid.cell_size());

            //        x += grid.cell_size();
            //    }

            //    x = grid.origin().x;
            //    y += grid.cell_size();
            //}

            fill(0, 255, 0, 255);
            no_stroke();

            for (int h = 0; h < grid.height(); ++h)
            {
                for (int w = 0; w < grid.width(); ++w)
                {
                    vec2 world_pos = grid.cell_world_position_center(w, h);
                    int index = w + (h * grid.width());
                    //circle(world_pos.x, world_pos.y, 5.0f);
                    text(to_str(index), world_pos.x, world_pos.y);
                }
            }
        }
    };
}
