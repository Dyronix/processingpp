#pragma once

#include <functional>

namespace ppp
{
    namespace structure
    {
        void on_draw_begin(std::function<void()> draw_begin);
        void on_draw_end(std::function<void()> draw_end);

        void redraw();

        void loop();
        void no_loop();

        bool is_looping();

        void quit();
    }
}