#include "render/render_batch_renderer.h"
#include "render/render_item.h"
#include "render/render_batch_render_strategy.h"
#include "render/render_batch_data_table.h"

#include "resources/material_pool.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static f32 _wireframe_linewidth = 3.0f;
            static s32 _wireframe_linecolor = 0x000000FF;
        }

        // Batch Renderer
        //-------------------------------------------------------------------------
        void batch_renderer::set_wireframe_linewidth(f32 width)
        {
            internal::_wireframe_linewidth = width;
        }
        //-------------------------------------------------------------------------
        void batch_renderer::set_wireframe_linecolor(s32 color)
        {
            internal::_wireframe_linecolor = color;
        }

        //-------------------------------------------------------------------------
        f32 batch_renderer::wireframe_linewidth()
        {
            return internal::_wireframe_linewidth;
        }
        //-------------------------------------------------------------------------
        s32 batch_renderer::wireframe_linecolor()
        {
            return internal::_wireframe_linecolor;
        }

        //-------------------------------------------------------------------------
        void batch_renderer::render(const ibatch_render_strategy* strategy, batch_data_table* batches)
        {
            if (batches->empty())
            {
                // No drawing data, early out
                return;
            }

            for (auto& [topology, draw_data] : *batches)
            {
                // No drawing data, early out
                if (!draw_data.has_drawing_data())
                {
                    return;
                }

                strategy->render_batch(topology, draw_data);
            }
        }
    }
}
