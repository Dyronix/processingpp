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
        void render_pipeline::begin(const render_context& context)
        {
            for (auto& pass : m_passes)
            {
                pass->begin_frame(context);
            }
        }

        //-------------------------------------------------------------------------
        void render_pipeline::render(const render_context& context)
        {
            for (auto& pass : m_passes)
            {
                pass->render(context);
            }
        }

        //-------------------------------------------------------------------------
        void render_pipeline::end(const render_context& context)
        {
            for (auto& pass : m_passes)
            {
                pass->end_frame(context);
            }
        }
    }
}