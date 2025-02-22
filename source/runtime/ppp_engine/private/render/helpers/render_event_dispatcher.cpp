#include "render/helpers/render_event_dispatcher.h"

#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"

namespace ppp
{
    namespace render
    {
        using draw_begin_subscriptions = global_vector<std::function<void()>>;
        using draw_end_subscriptions = global_vector<std::function<void()>>;

        struct context_event_dispatcher
        {
            draw_begin_subscriptions draw_begin_subs;
            draw_end_subscriptions draw_end_subs;
        } g_event_dispatcher_ctx;

        //-------------------------------------------------------------------------
        void broadcast_on_draw_begin()
        {
            if (g_event_dispatcher_ctx.draw_begin_subs.empty() == false)
            {
                for (auto& fn : g_event_dispatcher_ctx.draw_begin_subs)
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void broadcast_on_draw_end()
        {
            if (g_event_dispatcher_ctx.draw_end_subs.empty() == false)
            {
                for (auto& fn : g_event_dispatcher_ctx.draw_end_subs)
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void register_on_draw_begin(std::function<void()> draw_begin)
        {
            g_event_dispatcher_ctx.draw_begin_subs.push_back(draw_begin);
        }

        //-------------------------------------------------------------------------
        void register_on_draw_end(std::function<void()> draw_end)
        {
            g_event_dispatcher_ctx.draw_end_subs.push_back(draw_end);
        }
    }
}