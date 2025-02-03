#pragma once

#include "render/render_framebuffer.h"

#include "util/types.h"

#include <memory>

namespace ppp
{
    namespace framebuffer_pool
    {
        struct framebuffer_description
        {
            std::string_view tag;

            bool require_depth;

            //-------------------------------------------------------------------------
            bool operator==(const framebuffer_description& other) const
            {
                return tag == other.tag && require_depth == other.require_depth;
            }

            //-------------------------------------------------------------------------
            bool operator!=(const framebuffer_description& other) const
            {
                return !(*this == other);
            }
        };

        bool                                initialize(s32 width, s32 height);
        void                                terminate();

        void                                release(const render::framebuffer* framebuffer);
        void                                release(const framebuffer_description& desc);

        const render::framebuffer*          bind(const framebuffer_description& desc, render::framebuffer_bound_target target = render::framebuffer_bound_target::READ_WRITE);
        const render::framebuffer*          unbind(const framebuffer_description& desc);

        const render::framebuffer*          get(const framebuffer_description& desc);
        const render::default_framebuffer*  get_system();
    }
}

namespace std
{
    template<>
    struct hash<ppp::framebuffer_pool::framebuffer_description>
    {
        u64 operator()(const ppp::framebuffer_pool::framebuffer_description& s) const
        {
            return std::hash<std::string_view>{}(s.tag);
        }
    };
}