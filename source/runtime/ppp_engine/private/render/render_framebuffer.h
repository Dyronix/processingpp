#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

namespace ppp
{
    namespace render
    {
        enum class attachment_type
        {
            COLOR,
            DEPTH,
            STENCIL,
            DEPTH_STENCIL
        };

        enum class attachment_format
        {
            RGBA8,
            RGB8,
            DEPTH24,      // 24-bit depth buffer
            DEPTH32F,     // 32-bit floating-point depth buffer
            DEPTH_STENCIL // Combined depth & stencil
        };

        struct framebuffer_attachment
        {
            attachment_type type;
            attachment_format format;

            bool sampled; // Determines whether it should be a texture (true) or a renderbuffer (false)
        };

        struct framebuffer_descriptor
        {
            s32 width;
            s32 height;

            temp_vector<framebuffer_attachment> attachments;
        };

        enum class framebuffer_bound_target
        {
            READ_WRITE,
            READ,
            WRITE
        };

        class framebuffer
        {
        public:
            constexpr static s32 min_framebuffer_width() { return 32; }
            constexpr static s32 min_framebuffer_height() { return 32; }

        public:
            framebuffer(const framebuffer_descriptor& desc);
            ~framebuffer();

            void bind(framebuffer_bound_target target = framebuffer_bound_target::READ_WRITE) const;
            void unbind() const;

            s32 width() const;
            s32 height() const;

            bool has_depth_texture() const;
            bool has_depth() const;
            bool has_color_attachment() const;

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