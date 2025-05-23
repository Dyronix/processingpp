#include "render/render_instance_renderer.h"
#include "render/render_item.h"
#include "render/render_instance_render_strategy.h"
#include "render/render_instance_data_table.h"

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

        // Instance Renderer
        //-------------------------------------------------------------------------
        void instance_renderer::set_wireframe_linewidth(f32 width)
        {
            internal::_wireframe_linewidth = width;
        }
        //-------------------------------------------------------------------------
        void instance_renderer::set_wireframe_linecolor(s32 color)
        {
            internal::_wireframe_linecolor = color;
        }
        //-------------------------------------------------------------------------
        f32 instance_renderer::wireframe_linewidth()
        {
            return internal::_wireframe_linewidth;
        }
        //-------------------------------------------------------------------------
        s32 instance_renderer::wireframe_linecolor()
        {
            return internal::_wireframe_linecolor;
        }

        //-------------------------------------------------------------------------
        void instance_renderer::render(const iinstance_render_strategy* strategy, instance_data_table* instances)
        {
            if (instances->empty())
            {
                // No drawing data, early out
                return;
            }

            for (auto& [topology, draw_data] : *instances)
            {
                // No drawing data, early out
                if (!draw_data.has_drawing_data())
                {
                    return;
                }

                strategy->render_instance(topology, draw_data);
            }
        }
    }
}