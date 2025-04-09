#include "resources/framebuffer_pool.h"

#include "util/log.h"

#include <unordered_map>

namespace ppp
{
    namespace framebuffer_pool
    {
        namespace tags
        {
            //-------------------------------------------------------------------------
            string::string_id composite()
            {
                static string::string_id s_composite_fb = string::store_sid("composite_framebuffer");
                return s_composite_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id predepth()
            {
                static string::string_id s_predepth_fb = string::store_sid("predepth_framebuffer");
                return s_predepth_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id wireframe()
            {
                static string::string_id s_wireframe_fb = string::store_sid("wireframe_framebuffer");
                return s_wireframe_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id unlit()
            {
                static string::string_id s_unlit_fb = string::store_sid("unlit_framebuffer");
                return s_unlit_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id shadow_map()
            {
                static string::string_id s_shadow_map_fb = string::store_sid("shadow_map_framebuffer");
                return s_shadow_map_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id forward_shading()
            {
                static string::string_id s_forward_shading_fb = string::store_sid("forward_shading_framebuffer");
                return s_forward_shading_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id ui()
            {
                static string::string_id s_ui_fb = string::store_sid("ui_framebuffer");
                return s_ui_fb;
            }
            //-------------------------------------------------------------------------
            string::string_id blit()
            {
                static string::string_id s_blit_fb = string::store_sid("blit_framebuffer");
                return s_blit_fb;
            }
        }

        using framebuffers_arr              = std::vector<std::unique_ptr<render::framebuffer>>;
        using active_framebuffers_hash_map  = std::unordered_map<string::string_id, render::framebuffer*>;
        using default_framebuffer_ptr       = std::unique_ptr<render::default_framebuffer>;

        struct context
        {
            framebuffers_arr                framebuffers;
            active_framebuffers_hash_map    framebuffers_in_use;

            default_framebuffer_ptr         default_framebuffer;

        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize(s32 width, s32 height)
        {
            g_ctx.default_framebuffer = std::make_unique<render::default_framebuffer>(width, height);

            render::framebuffer_descriptor fbo_desc = 
            {
                width, height,
                {
                    {render::attachment_type::COLOR, render::attachment_format::RGBA8, false},      
                    {render::attachment_type::DEPTH, render::attachment_format::DEPTH24, false}     // Depth sampled = false
                }
            };

            render::framebuffer_descriptor shadow_fbo_desc = 
            {
                1024, 1024,
                {
                    {render::attachment_type::DEPTH, render::attachment_format::DEPTH24, true}      // Depth sampled = true
                }
            };

            render::framebuffer_descriptor blit_desc =
            {
                width, height,
                {
                    {render::attachment_type::COLOR, render::attachment_format::RGBA8},
                }
            };

            g_ctx.framebuffers.reserve(4);
            g_ctx.framebuffers.emplace_back(std::make_unique<render::framebuffer>(fbo_desc));
            g_ctx.framebuffers.emplace_back(std::make_unique<render::framebuffer>(fbo_desc));
            g_ctx.framebuffers.emplace_back(std::make_unique<render::framebuffer>(shadow_fbo_desc));
            g_ctx.framebuffers.emplace_back(std::make_unique<render::framebuffer>(blit_desc));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            g_ctx.framebuffers.clear();
        }

        //-------------------------------------------------------------------------
        void release(const render::iframebuffer* framebuffer)
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
        const render::iframebuffer* bind(string::string_id tag, render::framebuffer_bound_target target)
        {
            assert(g_ctx.framebuffers_in_use.find(tag) != std::cend(g_ctx.framebuffers_in_use));

            auto fb = g_ctx.framebuffers_in_use.find(tag);
            fb->second->bind(target);
            return fb->second;
        }

        //-------------------------------------------------------------------------
        const render::iframebuffer* unbind(string::string_id tag)
        {
            assert(g_ctx.framebuffers_in_use.find(tag) != std::cend(g_ctx.framebuffers_in_use));

            auto fb = g_ctx.framebuffers_in_use.find(tag);
            fb->second->unbind();
            return fb->second;
        }

        const render::iframebuffer* get(string::string_id tag, s32 flags)
        {
            // 1) Check if there's already a framebuffer in use for this tag 
            auto it = g_ctx.framebuffers_in_use.find(tag);
            if (it != g_ctx.framebuffers_in_use.end())
            {
                auto fb = it->second;
                // Check if fb meets the required flag criteria
                if ((!(flags & framebuffer_flags::DEPTH) || fb->has_depth()) &&
                    (!(flags & framebuffer_flags::SAMPLED_DEPTH) || (fb->has_depth() && fb->has_depth_texture())) &&
                    (!(flags & framebuffer_flags::COLOR) || fb->has_color_attachment()))
                {
                    return fb;
                }
            }

            // 2) Look for a free framebuffer that meets the flag criteria.
            for (auto& fb : g_ctx.framebuffers)
            {
                // Skip if this framebuffer is already in use.
                bool in_use = std::any_of(g_ctx.framebuffers_in_use.begin(), g_ctx.framebuffers_in_use.end(),
                    [fb_ptr = fb.get()](const auto& pair)
                    {
                        return pair.second == fb_ptr;
                    });
                
                if (in_use)
                {
                    continue;
                }

                bool meets_criteria = true;

                // Check DEPTH: if either DEPTH or SAMPLED_DEPTH is requested, require depth.
                if (flags & (framebuffer_flags::DEPTH | framebuffer_flags::SAMPLED_DEPTH))
                {
                    meets_criteria &= fb->has_depth();
                }
                else
                {
                    meets_criteria &= !fb->has_depth();
                }

                // Check SAMPLED_DEPTH: if requested, require depth texture.
                if (flags & framebuffer_flags::SAMPLED_DEPTH)
                {
                    meets_criteria &= fb->has_depth_texture();
                }
                else
                {
                    meets_criteria &= !fb->has_depth_texture();
                }

                // Check COLOR: only allow if requested.
                if (flags & framebuffer_flags::COLOR)
                {
                    meets_criteria &= fb->has_color_attachment();
                }
                else
                {
                    meets_criteria &= !fb->has_color_attachment();
                }

                if (meets_criteria)
                {
                    g_ctx.framebuffers_in_use[tag] = fb.get();
                    return fb.get();
                }
            }

            log::error("No framebuffers available for tag: {}", string::restore_sid(tag));
            return nullptr;
        }


        //-------------------------------------------------------------------------
        const render::iframebuffer* get_system()
        {
            return g_ctx.default_framebuffer.get();
        }
    }
}