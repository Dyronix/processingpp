#include "render/render_pass_blit.h"

#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        blit_pass::blit_pass(string::string_id src, s32 flags, string::string_id dst)
            :irender_pass("blit"_sid)
            ,m_src(src)
            ,m_dst(dst)
            ,m_flags(flags)
        {}
        //-------------------------------------------------------------------------
        blit_pass::~blit_pass() = default;

        //-------------------------------------------------------------------------
        void blit_pass::begin_frame(const render_context& context)
        {
            // No per-frame setup is required for blitting.
        }

        //-------------------------------------------------------------------------
        void blit_pass::render(const render_context& context)
        {
            // Retrieve the target framebuffer (your off-screen/main framebuffer)
            // and the system framebuffer (e.g., the default window framebuffer).
            auto target_framebuffer = framebuffer_pool::get(m_src, m_flags);
            auto system_framebuffer = m_dst.is_none() || m_flags == -1
                ? framebuffer_pool::get_system() 
                : framebuffer_pool::get(m_dst, m_flags);

            // Bind the target framebuffer for reading.
            target_framebuffer->bind(framebuffer_bound_target::READ);

            // Bind the system framebuffer for writing.
            system_framebuffer->bind(framebuffer_bound_target::WRITE);

            u32 copy_flags = 0;
            if (m_flags | framebuffer_flags::COLOR)
            {
                copy_flags |= GL_COLOR_BUFFER_BIT;
            }
            if (m_flags | framebuffer_flags::DEPTH || m_flags | framebuffer_flags::SAMPLED_DEPTH)
            {
                copy_flags |= GL_DEPTH_BUFFER_BIT;
            }

            // Perform the blit operation.
            opengl::api::instance().blit_framebuffer(
                0,
                0,
                target_framebuffer->width(),
                target_framebuffer->height(),
                0,
                0,
                system_framebuffer->width(),
                system_framebuffer->height(),
                copy_flags,
                GL_NEAREST
            );

            // Bind the system framebuffer as the active target for further operations.
            system_framebuffer->bind();
        }

        //-------------------------------------------------------------------------
        void blit_pass::end_frame(const render_context& context)
        {
            // No specific end-of-frame operations needed.
        }
    }
}
