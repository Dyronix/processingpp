#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class iframebuffer;

        // Clear bits is an mask holding which buffer of a framebuffer to clear
        enum clear_bits
        {
            NONE                  = BIT(0),
            CLEAR_COLOR_BUFFER    = BIT(1),
            CLEAR_DEPTH_BUFFER    = BIT(2),
        };

        struct clear_state_desc
        {
            clear_state_desc()
                : rgba(1.0f, 1.0f, 1.0f, 1.0f)
                , depth(1.0f)
                , stencil(0x00)
                , flags()
            {}

            glm::vec4   rgba;
            f32         depth;
            u8          stencil;
            u32         flags;
        };

        clear_state_desc make_rtv_clear_state(const glm::vec4& rgba = { 0.0f, 0.0f, 0.0f, 0.0f });
        clear_state_desc make_depth_clear_state(f32 depth = 1.0f);

        class clear_pass : public framebuffer_render_pass
        {
        public:
            clear_pass(const clear_state_desc& clear_state, string::string_id framebuffer_tag, s32 framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH);
            ~clear_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;
        private:
            clear_state_desc m_clear_state;
        };
    }
}