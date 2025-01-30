#include "render/render_framebuffer.h"

#include "render/opengl/render_gl_error.h"

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
                GL_CALL(glGenFramebuffers(1, &m_framebuffer_id));
                GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id));

                // Color attachment
                GL_CALL(glGenTextures(1, &m_color_rbo));
                GL_CALL(glBindTexture(GL_TEXTURE_2D, m_color_rbo));

                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, std::max(width, _min_frame_buffer_width), std::max(height, _min_frame_buffer_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
                GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_rbo, 0));

                // Depth attachment ( if enabled )
                if (with_depth)
                {
                    GL_CALL(glGenRenderbuffers(1, &m_depth_rbo));
                    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rbo));
                    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, std::max(width, _min_frame_buffer_width), std::max(height, _min_frame_buffer_height)));
                    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rbo));
                }

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    log::error("FramebufferStatus != FRAMEBUFFER_COMPLETE");
                    return;
                }

                GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            }

            //-------------------------------------------------------------------------
            void bind(framebuffer_bound_target target)
            {
                GLenum fb_bound_target;

                switch (target)
                {
                case framebuffer_bound_target::READ_WRITE: fb_bound_target = GL_FRAMEBUFFER; break;
                case framebuffer_bound_target::READ: fb_bound_target = GL_READ_FRAMEBUFFER; break;
                case framebuffer_bound_target::WRITE: fb_bound_target = GL_DRAW_FRAMEBUFFER; break;
                }

                GL_CALL(glBindFramebuffer(fb_bound_target, m_framebuffer_id));
            }

            //-------------------------------------------------------------------------
            void unbind()
            {
                GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            }

            //-------------------------------------------------------------------------
            bool release()
            {
                if (m_color_rbo != 0)
                {
                    glDeleteTextures(1, &m_color_rbo);
                    m_color_rbo = 0;
                }

                if (m_depth_rbo != 0)
                {
                    glDeleteRenderbuffers(1, &m_depth_rbo);
                    m_depth_rbo = 0;
                }

                if (m_framebuffer_id != 0)
                {
                    glDeleteFramebuffers(1, &m_framebuffer_id);
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
            :m_pimpl(std::make_unique<impl>(width, height, with_depth))
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
                GLenum fb_bound_target;

                switch (target)
                {
                case framebuffer_bound_target::READ_WRITE: fb_bound_target = GL_FRAMEBUFFER; break;
                case framebuffer_bound_target::READ: fb_bound_target = GL_READ_FRAMEBUFFER; break;
                case framebuffer_bound_target::WRITE: fb_bound_target = GL_DRAW_FRAMEBUFFER; break;
                }

                GL_CALL(glBindFramebuffer(fb_bound_target, m_framebuffer_id));
            }

            s32 m_width;
            s32 m_height;

            u32 m_framebuffer_id;
        };

        //-------------------------------------------------------------------------
        default_framebuffer::default_framebuffer(s32 width, s32 height)
            :m_pimpl(std::make_unique<impl>(width, height))
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