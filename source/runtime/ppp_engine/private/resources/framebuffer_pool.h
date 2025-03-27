#pragma once

#include "render/render_framebuffer.h"
#include "render/render_framebuffer_flags.h"

#include "util/types.h"
#include "string/string_id.h"

#include <memory>

namespace ppp
{
    namespace framebuffer_pool
    {
        namespace tags
        {
            string::string_id composite();
            string::string_id predepth();
            string::string_id wireframe();
            string::string_id unlit();
            string::string_id shadow_map();
            string::string_id forward_shading();
            string::string_id ui();
            string::string_id blit();
        }

        bool                                initialize(s32 width, s32 height);
        void                                terminate();

        void                                release(const render::iframebuffer* framebuffer);

        const render::iframebuffer*         bind(string::string_id tag, render::framebuffer_bound_target target = render::framebuffer_bound_target::READ_WRITE);
        const render::iframebuffer*         unbind(string::string_id tag);

        const render::iframebuffer*         get(string::string_id tag, s32 flags);
        const render::iframebuffer*         get_system();
    }
}