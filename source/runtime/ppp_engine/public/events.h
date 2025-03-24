#pragma once

#include <functional>

namespace ppp
{
    enum class key_code
    {
        /* The unknown key */
        KEY_UNKNOWN,

        /* Printable keys */
        KEY_SPACE,
        KEY_APOSTROPHE,         /* ' */
        KEY_COMMA,              /* , */
        KEY_MINUS,              /* - */
        KEY_PERIOD,             /* . */
        KEY_SLASH,              /* / */
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_SEMICOLON,          /* ; */
        KEY_EQUAL,              /* = */
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_LEFT_BRACKET,       /* [ */
        KEY_BACKSLASH,          /* \ */
        KEY_RIGHT_BRACKET,      /* ] */
        KEY_GRAVE_ACCENT,       /* ` */

        /* Function keys */
        KEY_ESCAPE,
        KEY_ENTER,
        KEY_TAB,
        KEY_BACKSPACE,
        KEY_INSERT,
        KEY_DELETE,
        KEY_RIGHT,
        KEY_LEFT,
        KEY_DOWN,
        KEY_UP,
        KEY_PAGE_UP,
        KEY_PAGE_DOWN,
        KEY_HOME,
        KEY_END,
        KEY_CAPS_LOCK,
        KEY_SCROLL_LOCK,
        KEY_NUM_LOCK,
        KEY_PRINT_SCREEN,
        KEY_PAUSE,
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15,
        KEY_F16,
        KEY_F17,
        KEY_F18,
        KEY_F19,
        KEY_F20,
        KEY_F21,
        KEY_F22,
        KEY_F23,
        KEY_F24,
        KEY_KP_0,
        KEY_KP_1,
        KEY_KP_2,
        KEY_KP_3,
        KEY_KP_4,
        KEY_KP_5,
        KEY_KP_6,
        KEY_KP_7,
        KEY_KP_8,
        KEY_KP_9,
        KEY_KP_DECIMAL,
        KEY_KP_DIVIDE,
        KEY_KP_MULTIPLY,
        KEY_KP_SUBTRACT,
        KEY_KP_ADD,
        KEY_KP_ENTER,
        KEY_KP_EQUAL,
        KEY_LEFT_SHIFT,
        KEY_LEFT_CONTROL,
        KEY_LEFT_ALT,
        KEY_RIGHT_SHIFT,
        KEY_RIGHT_CONTROL,
        KEY_RIGHT_ALT,
        KEY_MENU
    };

    bool is_key_pressed(key_code code);
    bool is_key_released(key_code code);
    bool is_key_down(key_code code);

    bool is_any_key_pressed();
    bool is_any_key_released();
    bool is_any_key_down();

    key_code key();

    void add_key_pressed_callback(const std::function<void(key_code)>& callback);
    void add_key_released_callback(const std::function<void(key_code)>& callback);
    void add_key_down_callback(const std::function<void(key_code)>& callback);

    void set_quit_application_keycode(key_code code);

    enum class mouse_code
    {
        BUTTON_LEFT,
        BUTTON_MIDDLE,
        BUTTON_RIGHT
    };

    float moved_x();
    float moved_y();

    float mouse_x();
    float mouse_y();

    float prev_mouse_x();
    float prev_mouse_y();

    mouse_code mouse_button();

    float scroll_offset_x();
    float scroll_offset_y();

    bool is_left_button_pressed();
    bool is_right_button_pressed();
    bool is_middle_button_pressed();

    bool is_any_mouse_button_pressed();

    void add_mouse_moved_callback(const std::function<void(float, float)>& callback);
    void add_mouse_dragged_callback(const std::function<void(float, float)>& callback);
    void add_mouse_pressed_callback(const std::function<void(mouse_code)>& callback);
    void add_mouse_released_callback(const std::function<void(mouse_code)>& callback);
    void add_mouse_horizontal_wheel_callback(const std::function<void(float)>& callback);
    void add_mouse_verticel_wheel_callback(const std::function<void(float)>& callback);

    void request_pointer_lock();
    void request_pointer_hide();
    void request_pointer_unlock();
    void request_pointer_show();
}