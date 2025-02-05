#include "render/helpers/render_event_dispatcher.h"

#include "memory/memory_types.h"

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            global_vector<std::function<void()>> _draw_begin_subs;
            global_vector<std::function<void()>> _draw_end_subs;
        }

        //-------------------------------------------------------------------------
        void broadcast_on_draw_begin()
        {
            if (internal::_draw_begin_subs.empty() == false)
            {
                for (auto& fn : internal::_draw_begin_subs)
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void broadcast_on_draw_end()
        {
            if (internal::_draw_end_subs.empty() == false)
            {
                for (auto& fn : internal::_draw_end_subs)
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void register_on_draw_begin(std::function<void()> draw_begin)
        {
            internal::_draw_begin_subs.push_back(draw_begin);
        }

        //-------------------------------------------------------------------------
        void register_on_draw_end(std::function<void()> draw_end)
        {
            internal::_draw_end_subs.push_back(draw_end);
        }
    }
}