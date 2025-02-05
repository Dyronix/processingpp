#pragma once

#include "util/types.h"

#include <functional>

struct GLFWwindow;

namespace ppp
{
    namespace device
    {
        namespace input
        {
            namespace keyboard
            {
                using key_callback = std::function<void(s32, s32, s32)>;
                using key_pressed_callback = std::function<void(s32, s32, s32)>;
                using key_released_callback = std::function<void(s32, s32, s32)>;
                using key_down_callback = std::function<void(s32, s32, s32)>;

                bool is_key_pressed(GLFWwindow* window, s32 code);
                bool is_key_released(GLFWwindow* window, s32 code);
                bool is_key_down(GLFWwindow* window, s32 code);

                void add_key_pressed_callback(GLFWwindow* window, const key_pressed_callback& callback);
                void add_key_released_callback(GLFWwindow* window, const key_released_callback& callback);
                void add_key_down_callback(GLFWwindow* window, const key_down_callback& callback);
            }

            namespace mouse
            {
                using mouse_pos_callback = std::function<void(f32, f32)>;
                using mouse_button_callback = std::function<void(s32, s32)>;
                using mouse_button_pressed_callback = std::function<void(s32, s32)>;
                using mouse_button_released_callback = std::function<void(s32, s32)>;

                using mouse_button_scroll_callback = std::function<void(f32)>;
                using mouse_button_scroll_x_callback = std::function<void(f32)>;
                using mouse_button_scroll_y_callback = std::function<void(f32)>;

                f32 mouse_x(GLFWwindow* window);
                f32 mouse_y(GLFWwindow* window);

                bool is_mouse_button_pressed(GLFWwindow* window, s32 code);
                bool is_mouse_button_released(GLFWwindow* window, s32 code);

                void lock_cursor(GLFWwindow* window);
                void hide_cursor(GLFWwindow* window);
                void unlock_cursor(GLFWwindow* window);
                void show_cursor(GLFWwindow* window);

                void add_mouse_pos_callback(GLFWwindow* window, const mouse_pos_callback& callback);

                void add_mouse_button_pressed_callback(GLFWwindow* window, const mouse_button_pressed_callback& callback);
                void add_mouse_button_released_callback(GLFWwindow* window, const mouse_button_released_callback& callback);

                void add_mouse_scroll_x_callback(GLFWwindow* window, const mouse_button_scroll_x_callback& callback);
                void add_mouse_scroll_y_callback(GLFWwindow* window, const mouse_button_scroll_y_callback& callback);
            }
        }
    }
}