#include "render/render_pipeline.h"
#include "render/render_pass.h"
#include "render/render_features.h"
#include "render/opengl/render_gl_error.h"

#include "geometry/geometry.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        render_pipeline::~render_pipeline() = default;

        //-------------------------------------------------------------------------
        bool render_pipeline::has_pass_with_framebuffer_tag(string::string_id framebuffer_tag) const
        {
            auto it = std::find_if(std::begin(m_passes), std::end(m_passes),
                [framebuffer_tag](const graphics_unique_ptr<irender_pass>& pass)
                {
                    return pass->has_framebuffer_tag(framebuffer_tag);
                });

            return it != std::cend(m_passes);
        }
        //-------------------------------------------------------------------------
        bool render_pipeline::has_pass_with_shader_tag(string::string_id shader_tag) const
        {
            auto it = std::find_if(std::begin(m_passes), std::end(m_passes),
                [shader_tag](const graphics_unique_ptr<irender_pass>& pass)
                {
                    return pass->has_shader(shader_tag);
                });

            return it != std::cend(m_passes);
        }

        //-------------------------------------------------------------------------
        void render_pipeline::add_pass(graphics_unique_ptr<irender_pass> pass)
        {
            m_passes.push_back(std::move(pass));
        }

        //-------------------------------------------------------------------------
        void render_pipeline::insert_pass(insertion_point point, graphics_unique_ptr<irender_pass> pass)
        {
            auto it = m_insertion_points.find(point);
            if (it == std::cend(m_insertion_points))
            {
                return;
            }

            auto insertion_it = m_passes.begin();
            std::advance(insertion_it, it->second);

            m_passes.insert(insertion_it, std::move(pass));
        }

        //-------------------------------------------------------------------------
        void render_pipeline::add_insertion_point(insertion_point point)
        {
            m_insertion_points[point] = m_passes.size();
        }

        //-------------------------------------------------------------------------
        void render_pipeline::execute(const render_context& context)
        {
            for (auto& pass : m_passes)
            {
                if (pass->should_render(context))
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
