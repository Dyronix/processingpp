#include "render/helpers/render_event_dispatcher.h"

#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            using draw_begin_subscriptions = global_vector<std::function<void()>>;
            using draw_end_subscriptions = global_vector<std::function<void()>>;

            //-------------------------------------------------------------------------
            static draw_begin_subscriptions& draw_begin_subs()
            {
                static auto s_draw_begin_subs = memory::tagged_placement_new<draw_begin_subscriptions>();

                return *s_draw_begin_subs;
            }

            //-------------------------------------------------------------------------
            static global_vector<std::function<void()>>& draw_end_subs()
            {
                static auto s_draw_end_subs = memory::tagged_placement_new<draw_end_subscriptions>();

                return *s_draw_end_subs;
            }
        }

        //-------------------------------------------------------------------------
        void broadcast_on_draw_begin()
        {
            if (internal::draw_begin_subs().empty() == false)
            {
                for (auto& fn : internal::draw_begin_subs())
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void broadcast_on_draw_end()
        {
            if (internal::draw_end_subs().empty() == false)
            {
                for (auto& fn : internal::draw_end_subs())
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void register_on_draw_begin(std::function<void()> draw_begin)
        {
            internal::draw_begin_subs().push_back(draw_begin);
        }

        //-------------------------------------------------------------------------
        void register_on_draw_end(std::function<void()> draw_end)
        {
            internal::draw_end_subs().push_back(draw_end);
        }
    }
}