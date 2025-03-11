#include "render/render_blit_pass.h"
#include "render/render_context.h"

#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        blit_pass::blit_pass(string::string_id src, string::string_id dst)
            :m_source(src)
            ,m_dst(dst)
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
            auto target_framebuffer = framebuffer_pool::get({ m_source, true, false });
            auto system_framebuffer = m_dst.is_none() ? framebuffer_pool::get_system() : framebuffer_pool::get({ m_source, true, false });;

            // Bind the target framebuffer for reading.
            target_framebuffer->bind(framebuffer_bound_target::READ);

            // Bind the system framebuffer for writing.
            system_framebuffer->bind(framebuffer_bound_target::WRITE);

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
                GL_COLOR_BUFFER_BIT,
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

        //-------------------------------------------------------------------------
        void blit_pass::setup(const render_context& context)
        {
            // One-time setup (if necessary) can be performed here.
        }

        //-------------------------------------------------------------------------
        void blit_pass::cleanup(const render_context& context)
        {
            // Clean up any resources if needed.
        }
    }
}
