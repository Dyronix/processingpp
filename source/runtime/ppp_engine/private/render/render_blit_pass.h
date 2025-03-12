#pragma once

#include "render/render_pass.h"

#include "string/string_id.h"

namespace ppp
{
    namespace render
    {
        class blit_pass : public render_pass
        {
        public:
            blit_pass(string::string_id src, s32 src_flags, string::string_id dst = string::string_id::create_invalid(), s32 dst_flags = -1);
            ~blit_pass() override;

            void begin_frame(const render_context& context) override;
            void render(const render_context& context) override;
            void end_frame(const render_context& context) override;

            void setup(const render_context& context) override;
            void cleanup(const render_context& context) override;

        private:
            string::string_id m_src;
            s32 m_src_flags;
            string::string_id m_dst;
            s32 m_dst_flags;
        };
    }
}