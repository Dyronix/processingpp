#pragma once

#include "util/types.h"

#include <functional>

namespace ppp
{
    namespace device
    {
        namespace input
        {
            void initialize();

            using key_callback = std::function<void(s32, s32, s32)>;
            using key_pressed_callback = std::function<void(s32, s32, s32)>;
            using key_released_callback = std::function<void(s32, s32, s32)>;
            using key_down_callback = std::function<void(s32, s32, s32)>;

            bool is_key_pressed(s32 code);
            bool is_key_released(s32 code);
            bool is_key_down(s32 code);

            void add_key_pressed_callback(const key_pressed_callback& callback);
            void add_key_released_callback(const key_released_callback& callback);
            void add_key_down_callback(const key_down_callback& callback);

            using mouse_pos_callback = std::function<void(f32, f32)>;
            using mouse_button_callback = std::function<void(s32, s32)>;
            using mouse_button_pressed_callback = std::function<void(s32, s32)>;
            using mouse_button_released_callback = std::function<void(s32, s32)>;

            using mouse_button_scroll_callback = std::function<void(f32)>;
            using mouse_button_scroll_x_callback = std::function<void(f32)>;
            using mouse_button_scroll_y_callback = std::function<void(f32)>;

            f32 mouse_x();
            f32 mouse_y();

            bool is_mouse_button_pressed(s32 code);
            bool is_mouse_button_released(s32 code);

            void lock_cursor();
            void hide_cursor();
            void unlock_cursor();
            void show_cursor();

            void add_mouse_pos_callback(const mouse_pos_callback& callback);

            void add_mouse_button_pressed_callback(const mouse_button_pressed_callback& callback);
            void add_mouse_button_released_callback(const mouse_button_released_callback& callback);

            void add_mouse_scroll_x_callback(const mouse_button_scroll_x_callback& callback);
            void add_mouse_scroll_y_callback(const mouse_button_scroll_y_callback& callback);
        }
    }
}