#pragma once 

#include "util/types.h"

#include "events.h"

namespace ppp
{
    enum class input_event_type : u32
    {
        KEY_PRESS,
        KEY_DOWN,
        KEY_RELEASE,
        MOUSE_MOTION,
        MOUSE_PRESS,
        MOUSE_RELEASE,
        MOUSE_DOWN,
        MOUSE_V_WHEEL,
        MOUSE_H_WHEEL,
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

    struct mouse_h_wheel_event
    {
        f32 wheel_h;
        s32 x, y;
    };

    struct mouse_v_wheel_event
    {
        f32 wheel_v;
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
            mouse_h_wheel_event mouse_h_wheel;
            mouse_v_wheel_event mouse_v_wheel;
        };
    };
}