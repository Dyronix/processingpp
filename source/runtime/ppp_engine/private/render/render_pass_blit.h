#pragma once

#include "render/render_pass.h"

#include "string/string_id.h"

namespace ppp
{
    namespace render
    {
        class blit_pass : public irender_pass
        {
        public:
            blit_pass(string::string_id src, s32 flags, string::string_id dst = string::string_id::create_invalid());
            ~blit_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

        private:
            string::string_id m_src;
            string::string_id m_dst;
            s32 m_flags;
        };
    }
}