#pragma once 

#include "util/types.h"

#include "events.h"

namespace ppp
{
    enum class input_event_type : u32
    {
        KEY_DOWN,
        KEY_UP,
        MOUSE_MOTION,
        MOUSE_BUTTON_DOWN,
        MOUSE_BUTTON_UP,
        MOUSE_WHEEL,
    };

    struct keyboard_event
    {
        key_code code;
    };

    struct mouse_motion_event
    {
        s32 x, y;
    };

    struct mouse_button_event
    {
        mouse_code button;
        s32 x, y;
    };

    struct mouse_wheel_event
    {
        f32 wheel_x, wheel_y;
        s32 x, y;
    };

    struct input_event
    {
        input_event_type type;
        union
        {
            keyboard_event keyboard;
            mouse_motion_event mouse_motion;
            mouse_button_event mouse_button;
            mouse_wheel_event  mouse_wheel;
        };
    };
}