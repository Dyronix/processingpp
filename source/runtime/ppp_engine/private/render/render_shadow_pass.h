#pragma once

#include "render/render_pass.h"

namespace ppp
{
    namespace render
    {
        class shadow_pass : public render_pass
        {
        public:
            ~shadow_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

            bool should_render() const override;
        };
    }
}