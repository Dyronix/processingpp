#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class iframebuffer;

        class unlit_pass : public render_pass
        {
        public:
            unlit_pass(const string::string_id shader_tag);
            ~unlit_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;
        };
    }
}