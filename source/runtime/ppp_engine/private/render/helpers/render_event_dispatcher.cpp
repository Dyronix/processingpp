#include "render/helpers/render_event_dispatcher.h"

#include "memory/memory_types.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            static pool_vector<std::function<void()>>& draw_begin_subs()
            {
                static pool_vector<std::function<void()>> s_draw_begin_subs;

                return s_draw_begin_subs;
            }

            //-------------------------------------------------------------------------
            static pool_vector<std::function<void()>>& draw_end_subs()
            {
                static pool_vector<std::function<void()>> s_draw_end_subs;

                return s_draw_end_subs;
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