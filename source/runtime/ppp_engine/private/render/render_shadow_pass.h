#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class shadow_pass : public render_pass
        {
        public:
            shadow_pass(const string::string_id shader_tag, const string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::SAMPLED_DEPTH);
            ~shadow_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

            bool should_render() const override;
        };
    }
}