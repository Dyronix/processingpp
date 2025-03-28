#pragma once

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        class ibatch_render_strategy;
        class batch_data_table;

        class batch_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

            static f32 wireframe_linewidth();
            static s32 wireframe_linecolor();

        public:
            static void render(const ibatch_render_strategy* strategy, batch_data_table* batches);
        };
    }
}