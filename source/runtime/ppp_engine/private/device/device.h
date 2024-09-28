#pragma once

#include "util/types.h"

#include <functional>

namespace ppp
{
    namespace device
    {
        namespace input
        {
            namespace keyboard
            {
                bool is_key_pressed(s32 code);
                bool is_key_released(s32 code);
                bool is_key_down(s32 code);

                bool is_any_key_pressed();
                bool is_any_key_released();
                bool is_any_key_down();

                s32 key();

                void add_key_pressed_callback(const std::function<void(s32, s32, s32)>& callback);
                void add_key_released_callback(const std::function<void(s32, s32, s32)>& callback);
                void add_key_down_callback(const std::function<void(s32, s32, s32)>& callback);
            }

            namespace mouse
            {
                s32 moved_x();
                s32 moved_y();

                s32 mouse_x();
                s32 mouse_y();

                s32 prev_mouse_x();
                s32 prev_mouse_y();

                s32 mouse_button();

                bool is_left_button_pressed();
                bool is_right_button_pressed();
                bool is_middle_button_pressed();

                bool is_any_mouse_button_pressed();
                bool is_any_mouse_button_released();

                void add_mouse_moved_callback(const std::function<void(s32, s32)>& callback);
                void add_mouse_dragged_callback(const std::function<void(s32, s32)>& callback);
                void add_mouse_pressed_callback(const std::function<void(s32, s32)>& callback);
                void add_mouse_released_callback(const std::function<void(s32, s32)>& callback);
                void add_mouse_horizontal_wheel_callback(const std::function<void(f32)>& callback);
                void add_mouse_verticel_wheel_callback(const std::function<void(f32)>& callback);

                void request_pointer_lock();
                void request_pointer_hide();
                void request_pointer_unlock();
                void request_pointer_show();
            }
        }

        bool initialize(s32 w, s32 h);
        void terminate();

        void window_width(s32* w);
        void window_height(s32* h);

        void loop();
        void no_loop();
        void redraw();
        void present();
        void poll_events();
        void request_quit();
        void target_frame_rate(u32 rate);

        bool is_looping();
        bool can_draw();
        bool should_close();

        u32 current_frame_index();
        u32 desired_frame_index();

        u32 target_frame_rate();
    }
}