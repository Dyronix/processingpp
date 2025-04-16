#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class shadow_pass : public geometry_render_pass
        {
        public:
            shadow_pass(string::string_id shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::SAMPLED_DEPTH, draw_mode draw_mode = draw_mode::AUTO);
            ~shadow_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

            bool should_render(const render_context& context) const override;
        };
    }
}