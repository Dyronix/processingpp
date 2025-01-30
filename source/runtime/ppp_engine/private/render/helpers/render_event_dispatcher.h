#pragma once

#include <functional>

namespace ppp
{
    namespace render
    {
        void broadcast_on_draw_begin();
        void broadcast_on_draw_end();

        void register_on_draw_begin(std::function<void()> draw_begin);
        void register_on_draw_end(std::function<void()> draw_end);
    }
}