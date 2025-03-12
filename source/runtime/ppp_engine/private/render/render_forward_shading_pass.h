#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class forward_shading_pass : public render_pass
        {
        public:
            ~forward_shading_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

            void setup(const render_context& context) override;
            void cleanup(const render_context& context) override;

        private:
            const iframebuffer* m_framebuffer;
        };
    }
}