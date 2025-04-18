#pragma once

#include <functional>

namespace ppp
{
    void on_draw_begin(std::function<void()> draw_begin);
    void on_draw_end(std::function<void()> draw_end);

    void redraw();

    void loop();
    void no_loop();

    bool is_headless();
    bool is_looping();

    void headless();
    void quit();
}