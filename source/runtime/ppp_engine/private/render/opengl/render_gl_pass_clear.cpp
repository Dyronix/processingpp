#include "render/render_pass_clear.h"
#include "render/render_context.h"
#include "render/render_scissor.h"
#include "render/render_framebuffer.h"

#include "render/opengl/render_gl_api.h"

#include "resources/framebuffer_pool.h"

#include "util/log.h"
#include "util/brush.h"

#include <algorithm>
#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        clear_state_desc make_rtv_clear_state(const glm::vec4& rgba)
        {
            clear_state_desc desc{};
            desc.flags |= clear_bits::CLEAR_COLOR_BUFFER;
            desc.rgba = rgba;

            return desc;
        }
        //-------------------------------------------------------------------------
        clear_state_desc make_depth_clear_state(f32 depth)
        {
            clear_state_desc desc{};
            desc.flags |= clear_bits::CLEAR_DEPTH_BUFFER;
            desc.depth = depth;
            return desc;
        }

        //-------------------------------------------------------------------------
        clear_pass::clear_pass(const clear_state_desc& clear_state, const string::string_id framebuffer_tag, s32 framebuffer_flags)
            :framebuffer_render_pass("clear"_sid, framebuffer_tag, framebuffer_flags)
            ,m_clear_state(clear_state)
        {}
        //-------------------------------------------------------------------------
        clear_pass::~clear_pass() = default;

        //-------------------------------------------------------------------------
        void clear_pass::begin_frame(const render_context& context)
        {
            assert(context && "Invalid render context");

            // Bind pass framebuffer
            framebuffer()->bind();

            // Configure OpenGL state.
            opengl::api::instance().viewport(0, 0, framebuffer()->width(), framebuffer()->height());

            u32 clear_buffer_bit = 0;
            if (m_clear_state.flags & CLEAR_COLOR_BUFFER)
            {
                opengl::api::instance().clear_color(
                    m_clear_state.rgba.r,
                    m_clear_state.rgba.g,
                    m_clear_state.rgba.b,
                    m_clear_state.rgba.a);
                opengl::api::instance().clear(GL_COLOR_BUFFER_BIT);

                // Set the background clear color from the brush.
                opengl::api::instance().clear_color(
                    brush::background().r,
                    brush::background().g,
                    brush::background().b,
                    brush::background().a);

                clear_buffer_bit |= GL_COLOR_BUFFER_BIT;
            }

            if (m_clear_state.flags & CLEAR_DEPTH_BUFFER)
            {
                opengl::api::instance().clear_depth(m_clear_state.depth);

                clear_buffer_bit |= GL_DEPTH_BUFFER_BIT;
            }

            // Configure scissor test if enabled.
            if (context.scissor->enable)
            {
                opengl::api::instance().enable(GL_SCISSOR_TEST);
                opengl::api::instance().scissor(
                    std::clamp(context.scissor->x, 0, framebuffer()->width()),
                    std::clamp(context.scissor->y, 0, framebuffer()->height()),
                    std::clamp(context.scissor->width, framebuffer::min_framebuffer_width(), framebuffer()->width()),
                    std::clamp(context.scissor->height, framebuffer::min_framebuffer_height(), framebuffer()->height())
                );
            }
            else
            {
                opengl::api::instance().disable(GL_SCISSOR_TEST);
            }

            // Clear buffers after scissor state is set.
            opengl::api::instance().clear(clear_buffer_bit);
        }

        //-------------------------------------------------------------------------
        void clear_pass::render(const render_context& context)
        {
            // Nothing to implement
        }

        //-------------------------------------------------------------------------
        void clear_pass::end_frame(const render_context& context)
        {
            // Unbind pass framebuffer
            auto framebuffer = framebuffer_pool::get(framebuffer_pool::tags::forward_shading(), framebuffer_flags::COLOR | framebuffer_flags::DEPTH);

            framebuffer->unbind();
        }
    }
}
