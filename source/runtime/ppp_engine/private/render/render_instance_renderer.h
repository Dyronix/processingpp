#pragma once

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        class iinstance_render_strategy;
        class instance_data_table;

        class instance_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

            static f32 wireframe_linewidth();
            static s32 wireframe_linecolor();

            static void render(const iinstance_render_strategy* strategy, instance_data_table* instances);
        };
    }
}