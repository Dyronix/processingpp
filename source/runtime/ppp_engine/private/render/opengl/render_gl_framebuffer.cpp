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
            impl(const framebuffer_descriptor& desc)
                :m_width(desc.width)
                ,m_height(desc.height)
                ,m_framebuffer_id(0)
                ,m_depth_attachment(0)
                ,m_depth_is_texture(false)
            {
                opengl::api::instance().generate_framebuffers(1, &m_framebuffer_id);
                opengl::api::instance().bind_framebuffer(GL_FRAMEBUFFER, m_framebuffer_id);

                for (const auto& attachment : desc.attachments)
                {
                    if (attachment.type == attachment_type::COLOR)
                    {
                        u32 color_texture;
                        opengl::api::instance().generate_textures(1, &color_texture);
                        opengl::api::instance().bind_texture(GL_TEXTURE_2D, color_texture);

                        opengl::api::instance().texture_image_2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                        opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                        GLenum attachment_point = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(m_color_attachments.size());
                        opengl::api::instance().framebuffer_texture2D(GL_FRAMEBUFFER, attachment_point, GL_TEXTURE_2D, color_texture, 0);

                        m_color_attachments.push_back(color_texture);
                    }
                    else if (attachment.type == attachment_type::DEPTH)
                    {
                        if (attachment.sampled) // Depth texture
                        {
                            u32 depth_texture;
                            opengl::api::instance().generate_textures(1, &depth_texture);
                            opengl::api::instance().bind_texture(GL_TEXTURE_2D, depth_texture);

                            opengl::api::instance().texture_image_2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                            opengl::api::instance().framebuffer_texture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
                            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                            f32 border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                            opengl::api::instance().set_texture_float_array_parameter(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border_color[0]);

                            opengl::api::instance().framebuffer_texture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_attachment, 0);

                            m_depth_attachment = depth_texture;
                            m_depth_is_texture = true;
                        }
                        else // Depth renderbuffer
                        {
                            u32 depth_rbo;
                            opengl::api::instance().generate_renderbuffers(1, &depth_rbo);
                            opengl::api::instance().bind_renderbuffer(GL_RENDERBUFFER, depth_rbo);
                            opengl::api::instance().renderbuffer_storage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
                            opengl::api::instance().framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
                            opengl::api::instance().framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_attachment);

                            m_depth_attachment = depth_rbo;
                            m_depth_is_texture = false;
                        }
                    }
                }

                if (opengl::api::instance().check_framebuffer_status(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    log::error("FramebufferStatus != FRAMEBUFFER_COMPLETE");
                    return;
                }

                opengl::api::instance().bind_framebuffer(GL_FRAMEBUFFER, 0);
            }
            //-------------------------------------------------------------------------
            ~impl()
            {
                release();
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
                for (auto tex : m_color_attachments)
                {
                    opengl::api::instance().delete_textures(1, &tex);
                }

                m_color_attachments.clear();

                if (m_depth_attachment)
                {
                    if (m_depth_is_texture)
                    {
                        opengl::api::instance().delete_textures(1, &m_depth_attachment);
                    }
                    else
                    {
                        opengl::api::instance().delete_renderbuffers(1, &m_depth_attachment);
                    }

                    m_depth_attachment = 0;
                }

                if (m_framebuffer_id)
                {
                    opengl::api::instance().delete_framebuffers(1, &m_framebuffer_id);

                    m_framebuffer_id = 0;
                }

                return true;
            }

            s32 m_width;
            s32 m_height;

            u32 m_framebuffer_id;

            graphics_vector<u32> m_color_attachments;
            u32 m_depth_attachment = 0;
            
            bool m_depth_is_texture = false;
        };

        //-------------------------------------------------------------------------
        framebuffer::framebuffer(const framebuffer_descriptor& desc)
            :m_pimpl(memory::make_unique<impl, memory::persistent_global_tagged_allocator<impl>>(desc))
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
        bool framebuffer::has_depth_texture() const 
        { 
            return m_pimpl->m_depth_is_texture; 
        }
        //-------------------------------------------------------------------------
        bool framebuffer::has_depth() const 
        {
            return m_pimpl->m_depth_attachment != 0; 
        }
        //-------------------------------------------------------------------------
        bool framebuffer::has_color_attachment() const 
        {
            return !m_pimpl->m_color_attachments.empty(); 
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