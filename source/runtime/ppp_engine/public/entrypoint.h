#pragma once

#include <functional>

namespace ppp
{
    struct AppParams
    {
        int window_width = 1280;
        int window_height = 720;
    };

    extern AppParams entry();

    extern void setup();
    extern void draw();
}