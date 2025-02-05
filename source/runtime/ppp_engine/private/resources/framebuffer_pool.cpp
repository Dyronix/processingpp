#include "resources/framebuffer_pool.h"

#include "util/log.h"

#include "memory/memory_types.h"
#include "memory/unique_ptr.h"

#include <unordered_map>

namespace ppp
{
    namespace framebuffer_pool
    {
        ppp::unique_ptr<render::default_framebuffer> _default_framebuffer;

        //-------------------------------------------------------------------------
        graphics_vector<ppp::unique_ptr<render::framebuffer>>& framebuffers()
        {
            static graphics_vector<ppp::unique_ptr<render::framebuffer>> s_framebuffers;

            return s_framebuffers;
        }
        //-------------------------------------------------------------------------
        graphics_hash_map<std::string_view, render::framebuffer*>& active_framebuffers()
        {
            static graphics_hash_map<std::string_view, render::framebuffer*> s_in_use;

            return s_in_use;
        }        

        //-------------------------------------------------------------------------
        bool initialize(s32 width, s32 height)
        {
            _default_framebuffer = ppp::make_unique<render::default_framebuffer>(width, height);

            constexpr bool with_depth = true;
            constexpr bool without_depth = false;

            framebuffers().reserve(4);
            framebuffers().emplace_back(ppp::make_unique<render::framebuffer>(width, height, with_depth));
            framebuffers().emplace_back(ppp::make_unique<render::framebuffer>(width, height, with_depth));
            framebuffers().emplace_back(ppp::make_unique<render::framebuffer>(width, height, without_depth));
            framebuffers().emplace_back(ppp::make_unique<render::framebuffer>(width, height, without_depth));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            framebuffers().clear();
        }

        //-------------------------------------------------------------------------
        void release(const render::framebuffer* framebuffer)
        {
            auto it = std::find_if(std::cbegin(active_framebuffers()), std::cend(active_framebuffers()),
                [framebuffer](const auto& pair)
                {
                    return pair.second == framebuffer;
                });

            if (it != std::cend(active_framebuffers()))
            {
                // Make sure the frame buffer is unbound when we release it from the list
                framebuffer->unbind();

                active_framebuffers().erase(it);
            }
        }

        //-------------------------------------------------------------------------
        void release(const framebuffer_description& desc)
        {
            auto it = active_framebuffers().find(desc.tag);
            if (it != std::cend(active_framebuffers()))
            {
                // Make sure the frame buffer is unbound when we release it from the list
                it->second->unbind();

                active_framebuffers().erase(it);
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
            for (auto& fb : framebuffers())
            {
                auto it = active_framebuffers().find(desc.tag);
                if (it != std::cend(active_framebuffers()) && fb->has_depth() == desc.require_depth)
                {
                    return it->second;
                }
            }

            // 2) Otherwise find a free one with the same depth requirement
            for (auto& fb : framebuffers()) 
            {
                auto it = active_framebuffers().find(desc.tag);
                if (it == std::cend(active_framebuffers()) && fb->has_depth() == desc.require_depth)
                {
                    active_framebuffers()[desc.tag] = fb.get();
                    return fb.get();
                }
            }

            log::error("No framebuffers available for tag: {}", desc.tag);
            return nullptr;
        }

        //-------------------------------------------------------------------------
        const render::default_framebuffer* get_system()
        {
            return _default_framebuffer.get();
        }
    }
}