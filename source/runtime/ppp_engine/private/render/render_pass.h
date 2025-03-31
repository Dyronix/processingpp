#pragma once

#include "string/string_id.h"

#include "render/render_shader.h"
#include "render/render_framebuffer.h"
#include "render/render_framebuffer_flags.h"
#include "render/render_batch_render_strategy.h"
#include "render/render_instance_render_strategy.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"

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
        using batch_draw_strategy = std::unique_ptr<ibatch_render_strategy>;
        using inst_draw_strategy = std::unique_ptr<iinstance_render_strategy>;

        struct render_context;

        class render_pass
        {
        public:
            render_pass(const string::string_id pass_tag, const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags);

            virtual ~render_pass() = default;

        public:
            virtual void                      begin_frame(const render_context& context) = 0;
            virtual void                      render(const render_context& context) = 0;
            virtual void                      end_frame(const render_context& context) = 0;

            virtual bool                      should_render() const { return true; }

            virtual batch_draw_strategy       make_batch_render_strategy() const { return std::make_unique<default_batch_render_strategy>(); }
            virtual inst_draw_strategy        make_inst_render_strategy() const { return std::make_unique<default_instance_render_strategy>(); }

        public:
            string::string_id                 pass_tag() const { return m_pass_tag.is_none() ? m_shader_tag : m_pass_tag; }
            string::string_id                 shader_tag() const { return m_shader_tag; }

        protected:
            const resources::iframebuffer*    framebuffer() const;
            const resources::shader_program   shader_program() const;
            const resources::imaterial*       material() const;

            ibatch_render_strategy*           batch_render_strategy();
            iinstance_render_strategy*        instance_render_strategy();

        private:
            const string::string_id           m_pass_tag;
            const string::string_id           m_shader_tag;
            const string::string_id           m_framebuffer_tag;
            const s32                         m_framebuffer_flags;

            batch_draw_strategy               m_batch_draw_strategy;
            inst_draw_strategy                m_inst_draw_strategy;
        };
    }
}