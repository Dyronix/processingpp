#include "render/render_pipeline.h"
#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        void render_pipeline::add_pass(graphics_unique_ptr<render_pass> pass)
        {
            m_passes.push_back(std::move(pass));
        }

        //-------------------------------------------------------------------------
        void render_pipeline::execute(const render_context& context)
        {
            for (auto& pass : m_passes)
            {
                if (pass->should_render())
                {
                    pass->begin_frame(context);
                    pass->render(context);
                    pass->end_frame(context);
                }
            }
        }
    }
}