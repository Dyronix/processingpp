#pragma once

#include "string/string_id.h"

#include "render/render_shader.h"
#include "render/render_framebuffer.h"
#include "render/render_batch_render_strategy.h"
#include "render/render_instance_render_strategy.h"

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

        class irender_pass
        {
        public:
            irender_pass(string::string_id pass_tag)
                :m_pass_tag(pass_tag)
                ,m_enable(true)
            {}

            virtual ~irender_pass() = default;

        public:
            void                                enable() { m_enable = true; }
            void                                disable() { m_enable = false; }

        public:
            virtual void                        begin_frame(const render_context& context) = 0;
            virtual void                        render(const render_context& context) = 0;
            virtual void                        end_frame(const render_context& context) = 0;

        public:
            virtual bool                        should_render(const render_context& context) const { return m_enable; }

            virtual bool                        has_framebuffer_tag(string::string_id shader_tag) const { return false; }
            virtual bool                        has_shader(string::string_id shader_tag) const { return false; }

        public:
            string::string_id                   pass_tag() const { return m_pass_tag; }

        protected:
            virtual string::string_id           framebuffer_tag() const { return string::string_id::create_invalid(); }
            virtual string::string_id           shader_tag() const { return string::string_id::create_invalid(); }

        private:
            string::string_id                   m_pass_tag;
            bool                                m_enable;
        };

        class framebuffer_render_pass : public irender_pass
        {
        public:
            framebuffer_render_pass(string::string_id pass_tag, string::string_id framebuffer_tag, s32 framebuffer_flags);
            ~framebuffer_render_pass() override;

        public:
            bool                                has_framebuffer_tag(string::string_id framebuffer_tag) const override;

        protected:
            string::string_id                   framebuffer_tag() const override { return m_framebuffer_tag; }
            u32                                 framebuffer_flags() const { return m_framebuffer_flags; }

            const resources::iframebuffer*      framebuffer() const;

        private:
            string::string_id                   m_framebuffer_tag;
            const s32                           m_framebuffer_flags;
        };

        class geometry_render_pass : public framebuffer_render_pass
        {
        public:
            enum class draw_mode
            {
                AUTO,
                INSTANCED,
                BATCHED
            };

            geometry_render_pass(string::string_id pass_tag, string::string_id shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags, draw_mode draw_mode);
            ~geometry_render_pass() override;

        public:
            bool                                should_render(const render_context& context) const override;

            bool                                has_shader(string::string_id shader_tag) const override;

        public:
            virtual batch_draw_strategy         make_batch_render_strategy() const { return std::make_unique<default_batch_render_strategy>(); }
            virtual inst_draw_strategy          make_inst_render_strategy() const { return std::make_unique<default_instance_render_strategy>(); }

        protected:
            string::string_id                   shader_tag() const override { return m_shader_tag; }
            const resources::shader_program     shader_program() const;
            const resources::imaterial*         material() const;

            ibatch_render_strategy*             batch_render_strategy();
            bool                                batch_rendering_enabled() const;

            iinstance_render_strategy*          instance_render_strategy();
            bool                                instance_rendering_enabled() const;

            draw_mode                           drawing_mode() const { return m_draw_mode; }

        private:
            string::string_id                   m_shader_tag;

            batch_draw_strategy                 m_batch_draw_strategy;
            inst_draw_strategy                  m_inst_draw_strategy;

            draw_mode                           m_draw_mode;
        };
    }
}