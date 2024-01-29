#pragma once

#include "types.h"

#include <functional>

namespace ppp
{
    struct AppParams
    {
        using setup_func = std::function<void()>;
        using draw_func = std::function<void()>;

        setup_func app_setup_func = nullptr;
        draw_func app_draw_func = nullptr;

        s32 window_width = 1280;
        s32 window_height = 720;
    };

    extern AppParams app_entry();
}