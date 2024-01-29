#pragma once

#include "types.h"

#include <functional>

namespace ppp
{
    namespace device
    {
        bool initialize(s32 w, s32 h);
        void terminate();

        void window_width(s32* w);
        void window_height(s32* h);

        void set_framebuffer_resize_callback(std::function<void(s32, s32, s32, s32)> callback);

        void poll_events();
        void process_input();
        void present();

        bool should_close();
    }
}