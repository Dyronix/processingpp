#include "render/render_framebuffer.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "memory/memory_unique_ptr_util.h"

#include "util/log.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        constexpr s32 _min_frame_buffer_width = 32;
        constexpr s32 _min_frame_buffer_height = 32;
        
        //-------------------------------------------------------------------------
        // FRAMEBUFFER
        class framebuffer::impl
        {
        public:
            //-------------------------------------------------------------------------
            impl(s32 width, s32 height, bool with_depth)
                :m_width(width)
                ,m_height(height)
                ,m_framebuffer_id(0)
                ,m_color_rbo(0)
                ,m_depth_rbo(0)
            {
                opengl::api::instance().generate_framebuffers(1, &m_framebuffer_id);
                opengl::api::instance().bind_framebuffer(GL_FRAMEBUFFER, m_framebuffer_id);

                // Color attachment
                opengl::api::instance().generate_textures(1, &m_color_rbo);
                opengl::api::instance().bind_texture(GL_TEXTURE_2D, m_color_rbo);

                opengl::api::instance().texture_image_2D(GL_TEXTURE_2D, 0, GL_RGBA, std::max(width, _min_frame_buffer_width), std::max(height, _min_frame_buffer_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                opengl::api::instance().framebuffer_texture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_rbo, 0);

                // Depth attachment ( if enabled )
                if (with_depth)
                {
                    opengl::api::instance().generate_renderbuffers(1, &m_depth_rbo);
                    opengl::api::instance().bind_renderbuffer(GL_RENDERBUFFER, m_depth_rbo);
                    opengl::api::instance().renderbuffer_storage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, std::max(width, _min_frame_buffer_width), std::max(height, _min_frame_buffer_height));
                    opengl::api::instance().framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rbo);
                }

                if (opengl::api::instance().check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    log::error("FramebufferStatus != FRAMEBUFFER_COMPLETE");
                    return;
                }

                opengl::api::instance().bind_framebuffer(GL_FRAMEBUFFER, 0);
            }

            //-------------------------------------------------------------------------
            void bind(framebuffer_bound_target target)
            {
                GLenum fb_bound_target = 0;

                switch (target)
                {
                case framebuffer_bound_target::READ_WRITE: fb_bound_target = GL_FRAMEBUFFER; break;
                case framebuffer_bound_target::READ: fb_bound_target = GL_READ_FRAMEBUFFER; break;
                case framebuffer_bound_target::WRITE: fb_bound_target = GL_DRAW_FRAMEBUFFER; break;
                }

                opengl::api::instance().bind_framebuffer(fb_bound_target, m_framebuffer_id);
            }

            //-------------------------------------------------------------------------
            void unbind()
            {
                opengl::api::instance().bind_framebuffer(GL_FRAMEBUFFER, 0);
            }

            //-------------------------------------------------------------------------
            bool release()
            {
                if (m_color_rbo != 0)
                {
                    opengl::api::instance().delete_textures(1, &m_color_rbo);
                    m_color_rbo = 0;
                }

                if (m_depth_rbo != 0)
                {
                    opengl::api::instance().delete_renderbuffers(1, &m_depth_rbo);
                    m_depth_rbo = 0;
                }

                if (m_framebuffer_id != 0)
                {
                    opengl::api::instance().delete_framebuffers(1, &m_framebuffer_id);
                    m_framebuffer_id = 0;
                }

                return true;
            }

            s32 m_width;
            s32 m_height;

            u32 m_framebuffer_id;

            u32 m_color_rbo;
            u32 m_depth_rbo;
        };

        //-------------------------------------------------------------------------
        framebuffer::framebuffer(s32 width, s32 height, bool with_depth)
            :m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(width, height, with_depth))
        {

        }

        //-------------------------------------------------------------------------
        framebuffer::~framebuffer()
        {
            m_pimpl->unbind();
            m_pimpl->release();
        }

        //-------------------------------------------------------------------------
        void framebuffer::bind(framebuffer_bound_target target) const
        {
            m_pimpl->bind(target);
        }

        //-------------------------------------------------------------------------
        void framebuffer::unbind() const
        {
            m_pimpl->unbind();
        }

        //-------------------------------------------------------------------------
        s32 framebuffer::width() const
        {
            return m_pimpl->m_width;
        }

        //-------------------------------------------------------------------------
        s32 framebuffer::height() const
        {
            return m_pimpl->m_height;
        }

        //-------------------------------------------------------------------------
        bool framebuffer::has_depth() const
        {
            return m_pimpl->m_depth_rbo != 0;
        }

        //-------------------------------------------------------------------------
        // DEFAULT FRAMEBUFFER
        class default_framebuffer::impl
        {
        public:
            //-------------------------------------------------------------------------
            impl(s32 width, s32 height)
                :m_width(width)
                ,m_height(height)
                ,m_framebuffer_id(0)
            {
            }

            //-------------------------------------------------------------------------
            void bind(framebuffer_bound_target target)
            {
                GLenum fb_bound_target = 0;

                switch (target)
                {
                case framebuffer_bound_target::READ_WRITE: fb_bound_target = GL_FRAMEBUFFER; break;
                case framebuffer_bound_target::READ: fb_bound_target = GL_READ_FRAMEBUFFER; break;
                case framebuffer_bound_target::WRITE: fb_bound_target = GL_DRAW_FRAMEBUFFER; break;
                }

                opengl::api::instance().bind_framebuffer(fb_bound_target, m_framebuffer_id);
            }

            s32 m_width;
            s32 m_height;

            u32 m_framebuffer_id;
        };

        //-------------------------------------------------------------------------
        default_framebuffer::default_framebuffer(s32 width, s32 height)
            :m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(width, height))
        {

        }
        //-------------------------------------------------------------------------
        default_framebuffer::~default_framebuffer() = default;

        //-------------------------------------------------------------------------
        void default_framebuffer::bind(framebuffer_bound_target target) const
        {
            m_pimpl->bind(target);
        }

        //-------------------------------------------------------------------------
        s32 default_framebuffer::width() const
        {
            return m_pimpl->m_width;
        }

        //-------------------------------------------------------------------------
        s32 default_framebuffer::height() const
        {
            return m_pimpl->m_height;
        }
    } 
}