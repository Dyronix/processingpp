#pragma once

#include "string/string_id.h"

#include "render/render_shader.h"
#include "render/render_framebuffer.h"
#include "render/render_framebuffer_flags.h"

namespace ppp
{
    namespace resources
    {
        class imaterial;

        using shader_program = std::shared_ptr<render::shaders::shader_program>;
        using iframebuffer = render::iframebuffer;
    }

    namespace render
    {
        struct render_context;

        class render_pass
        {
        public:
            render_pass(const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags);

            virtual ~render_pass() = default;

        public:
            virtual void                begin_frame(const render_context& context) = 0;
            virtual void                render(const render_context& context) = 0;
            virtual void                end_frame(const render_context& context) = 0;

            virtual bool                should_render() const { return true; }

        protected:
            const resources::iframebuffer*    framebuffer() const;
            const resources::shader_program   shader_program() const;
            const resources::imaterial*       material() const;

        private:
            const string::string_id     m_shader_tag;
            const string::string_id     m_framebuffer_tag;
            const s32                   m_framebuffer_flags;
        };
    }
}