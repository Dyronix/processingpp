#pragma once

#include "render/render_pass.h"
#include "render/render_shader_tags.h"

namespace ppp
{
    namespace render
    {
        class iframebuffer;

        class font_pass : public geometry_render_pass
        {
        public:
            font_pass(const tag<unlit::font> shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH);
            ~font_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

        public:
            bool should_render(const render_context& context) const override;
        };
    }
}