#pragma once

namespace ppp
{
    namespace structure
    {
        void push();
        void pop();

        void redraw();

        void loop();
        void no_loop();

        bool is_looping();

        void quit();
    }
}