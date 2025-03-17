#include "render/render_pipeline.h"
#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        render_pipeline::~render_pipeline() = default;

        //-------------------------------------------------------------------------
        void render_pipeline::add_pass(std::unique_ptr<render_pass> pass)
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