#pragma once

namespace ppp
{
    namespace render
    {
        struct render_context;

        class render_pass
        {
        public:
            virtual ~render_pass() = default;

            virtual void begin_frame(const render_context& context) = 0;
            virtual void render(const render_context& context) = 0;
            virtual void end_frame(const render_context& context) = 0;

            virtual void setup(const render_context& context) = 0;
            virtual void cleanup(const render_context& context) = 0;
        };
    }
}