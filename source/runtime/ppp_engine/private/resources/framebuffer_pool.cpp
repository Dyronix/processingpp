#include "resources/framebuffer_pool.h"

#include "util/log.h"

#include "memory/memory_placement_new.h"
#include "memory/memory_types.h"
#include "memory/memory_unique_ptr_util.h"

#include <unordered_map>

namespace ppp
{
    namespace framebuffer_pool
    {
        using framebuffers_arr              = graphics_vector<graphics_unique_ptr<render::framebuffer>>;
        using active_framebuffers_hash_map  = graphics_hash_map<string::string_id, render::framebuffer*>;
        using default_framebuffer_ptr       = graphics_unique_ptr<render::default_framebuffer>;

        struct context
        {
            framebuffers_arr                framebuffers;
            active_framebuffers_hash_map    framebuffers_in_use;

            default_framebuffer_ptr         default_framebuffer;

        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize(s32 width, s32 height)
        {
            g_ctx.default_framebuffer = memory::make_unique<render::default_framebuffer, memory::persistent_graphics_tagged_allocator<render::default_framebuffer>>(width, height);

            constexpr bool with_depth = true;
            constexpr bool without_depth = false;

            render::framebuffer_descriptor fbo_desc = 
            {
                width, height,
                {
                    {render::attachment_type::COLOR, render::attachment_format::RGBA8, false},
                    {render::attachment_type::DEPTH, render::attachment_format::DEPTH24, false}
                }
            };

            render::framebuffer_descriptor shadow_fbo_desc = 
            {
                1024, 1024,
                {
                    {render::attachment_type::DEPTH, render::attachment_format::DEPTH24, true} // Depth sampled = true
                }
            };

            g_ctx.framebuffers.reserve(4);
            g_ctx.framebuffers.emplace_back(memory::make_unique<render::framebuffer, memory::persistent_graphics_tagged_allocator<render::framebuffer>>(fbo_desc));
            g_ctx.framebuffers.emplace_back(memory::make_unique<render::framebuffer, memory::persistent_graphics_tagged_allocator<render::framebuffer>>(fbo_desc));
            g_ctx.framebuffers.emplace_back(memory::make_unique<render::framebuffer, memory::persistent_graphics_tagged_allocator<render::framebuffer>>(shadow_fbo_desc));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            g_ctx.framebuffers.clear();
        }

        //-------------------------------------------------------------------------
        void release(const render::framebuffer* framebuffer)
        {
            auto it = std::find_if(std::cbegin(g_ctx.framebuffers_in_use), std::cend(g_ctx.framebuffers_in_use),
                [framebuffer](const auto& pair)
                {
                    return pair.second == framebuffer;
                });

            if (it != std::cend(g_ctx.framebuffers_in_use))
            {
                // Make sure the frame buffer is unbound when we release it from the list
                framebuffer->unbind();

                g_ctx.framebuffers_in_use.erase(it);
            }
        }

        //-------------------------------------------------------------------------
        void release(const framebuffer_description& desc)
        {
            auto it = g_ctx.framebuffers_in_use.find(desc.tag);
            if (it != std::cend(g_ctx.framebuffers_in_use))
            {
                // Make sure the frame buffer is unbound when we release it from the list
                it->second->unbind();

                g_ctx.framebuffers_in_use.erase(it);
            }
        }

        //-------------------------------------------------------------------------
        const render::framebuffer* bind(const framebuffer_description& desc, render::framebuffer_bound_target target)
        {
            auto fb = get(desc);
            fb->bind(target);
            return fb;
        }

        //-------------------------------------------------------------------------
        const render::framebuffer* unbind(const framebuffer_description& desc)
        {
            auto fb = get(desc);
            fb->unbind();
            return fb;
        }

        //-------------------------------------------------------------------------
        const render::framebuffer* get(const framebuffer_description& desc)
        {
            // 1) Check if we already have a framebuffer with the same (tag, withDepth) in use.
            for (auto& fb : g_ctx.framebuffers)
            {
                auto it = g_ctx.framebuffers_in_use.find(desc.tag);
                if (it != std::cend(g_ctx.framebuffers_in_use) 
                    && fb->has_depth() == desc.require_depth
                    && fb->has_depth_texture() == desc.require_sampled_depth)
                {
                    return it->second;
                }
            }

            // 2) Otherwise find a free one with the same depth requirement
            for (auto& fb : g_ctx.framebuffers) 
            {
                auto it = g_ctx.framebuffers_in_use.find(desc.tag);
                if (it == std::cend(g_ctx.framebuffers_in_use) 
                    && fb->has_depth() == desc.require_depth
                    && fb->has_depth_texture() == desc.require_sampled_depth)
                {
                    g_ctx.framebuffers_in_use[desc.tag] = fb.get();
                    return fb.get();
                }
            }

            log::error("No framebuffers available for tag: {}", string::restore_sid(desc.tag));
            return nullptr;
        }

        //-------------------------------------------------------------------------
        const render::default_framebuffer* get_system()
        {
            return g_ctx.default_framebuffer.get();
        }
    }
}