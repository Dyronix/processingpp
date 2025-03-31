#include "render/render_pipeline.h"
#include "render/render_pass.h"
#include "render/render_features.h"
#include "render/opengl/render_gl_error.h"

#include <glad/glad.h>

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
#if _DEBUG
                    if (has_debugging_capabilities())
                    {
                        auto pass_tag_sid = pass->pass_tag();
                        auto pass_tag_sv = string::restore_sid(pass_tag_sid);

                        GL_CALL(glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, pass_tag_sv.data()));
                    }
#endif
                    pass->begin_frame(context);
                    pass->render(context);
                    pass->end_frame(context);
#if _DEBUG
                    if (has_debugging_capabilities())
                    {
                        GL_CALL(glPopDebugGroup());
                    }
#endif
                }
            }
        }
    }
}
