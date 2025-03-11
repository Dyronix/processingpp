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

        class iframebuffer
        {
        public:
            virtual ~iframebuffer() = default;

            virtual void bind(framebuffer_bound_target target = framebuffer_bound_target::READ_WRITE) const = 0;

            virtual s32 width() const = 0;
            virtual s32 height() const = 0;
        };

        class framebuffer : public iframebuffer
        {
        public:
            constexpr static s32 min_framebuffer_width() { return 32; }
            constexpr static s32 min_framebuffer_height() { return 32; }

        public:
            framebuffer(const framebuffer_descriptor& desc);
            ~framebuffer() override;

            void bind(framebuffer_bound_target target = framebuffer_bound_target::READ_WRITE) const override;
            void unbind() const;

            s32 width() const override;
            s32 height() const override;

            bool has_depth_texture() const;
            bool has_depth() const;
            bool has_color_attachment() const;

        private:
            class impl;
            global_unique_ptr<impl> m_pimpl;
        };

        class default_framebuffer : public iframebuffer
        {
        public:
            default_framebuffer(s32 width, s32 height);
            ~default_framebuffer() override;

            void bind(framebuffer_bound_target target = framebuffer_bound_target::READ_WRITE) const override;

            s32 width() const override;
            s32 height() const override;

        private:
            class impl;
            global_unique_ptr<impl> m_pimpl;
        };
    }
}