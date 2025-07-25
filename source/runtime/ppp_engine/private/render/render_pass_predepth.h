#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class iframebuffer;

        class predepth_pass : public geometry_render_pass
        {
        public:
            predepth_pass(string::string_id shader_tag, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH, draw_mode draw_mode = draw_mode::AUTO);
            ~predepth_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

            s32 count_batched_draw_calls(const render_context& context) const override;
            s32 count_instanced_draw_calls(const render_context& context) const override;
        };
    }
}