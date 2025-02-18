#pragma once

#include "util/types.h"

#include "memory/unique_ptr.h"

namespace ppp
{
    namespace render
    {
        enum class framebuffer_bound_target
        {
            READ_WRITE,
            READ,
            WRITE
        };

        class framebuffer
        {
        public:
            framebuffer(s32 width, s32 height, bool with_depth = false);
            ~framebuffer();

            void bind(framebuffer_bound_target target = framebuffer_bound_target::READ_WRITE) const;
            void unbind() const;

            s32 width() const;
            s32 height() const;

            bool has_depth() const;           

        private:
            class impl;
            global_unique_ptr<impl> m_pimpl;
        };

        class default_framebuffer
        {
        public:
            default_framebuffer(s32 width, s32 height);
            ~default_framebuffer();

            void bind(framebuffer_bound_target target = framebuffer_bound_target::READ_WRITE) const;

            s32 width() const;
            s32 height() const;

        private:
            class impl;
            global_unique_ptr<impl> m_pimpl;
        };
    }
}