#include "events.h"
#include "structure.h"
#include "device/device.h"
#include "util/log.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace internal
    {
        int convert_key_to_glfw(key_code code)
        {
            /* The unknown key */
            switch (code)
            {
            case key_code::KEY_UNKNOWN: return GLFW_KEY_UNKNOWN;

                /* Printable keys */
            case key_code::KEY_SPACE: return GLFW_KEY_SPACE;
            case key_code::KEY_APOSTROPHE: return GLFW_KEY_APOSTROPHE;           /* ' */
            case key_code::KEY_COMMA: return GLFW_KEY_COMMA;                     /* , */
            case key_code::KEY_MINUS: return GLFW_KEY_MINUS;                     /* - */
            case key_code::KEY_PERIOD: return GLFW_KEY_PERIOD;                   /* . */
            case key_code::KEY_SLASH: return GLFW_KEY_SLASH;                     /* / */
            case key_code::KEY_0: return GLFW_KEY_0;
            case key_code::KEY_1: return GLFW_KEY_1;
            case key_code::KEY_2: return GLFW_KEY_2;
            case key_code::KEY_3: return GLFW_KEY_3;
            case key_code::KEY_4: return GLFW_KEY_4;
            case key_code::KEY_5: return GLFW_KEY_5;
            case key_code::KEY_6: return GLFW_KEY_6;
            case key_code::KEY_7: return GLFW_KEY_7;
            case key_code::KEY_8: return GLFW_KEY_8;
            case key_code::KEY_9: return GLFW_KEY_9;
            case key_code::KEY_SEMICOLON: return GLFW_KEY_SEMICOLON;             /* ; */
            case key_code::KEY_EQUAL: return GLFW_KEY_EQUAL;                     /* = */
            case key_code::KEY_A: return GLFW_KEY_A;
            case key_code::KEY_B: return GLFW_KEY_B;
            case key_code::KEY_C: return GLFW_KEY_C;
            case key_code::KEY_D: return GLFW_KEY_D;
            case key_code::KEY_E: return GLFW_KEY_E;
            case key_code::KEY_F: return GLFW_KEY_F;
            case key_code::KEY_G: return GLFW_KEY_G;
            case key_code::KEY_H: return GLFW_KEY_H;
            case key_code::KEY_I: return GLFW_KEY_I;
            case key_code::KEY_J: return GLFW_KEY_J;
            case key_code::KEY_K: return GLFW_KEY_K;
            case key_code::KEY_L: return GLFW_KEY_L;
            case key_code::KEY_M: return GLFW_KEY_M;
            case key_code::KEY_N: return GLFW_KEY_N;
            case key_code::KEY_O: return GLFW_KEY_O;
            case key_code::KEY_P: return GLFW_KEY_P;
            case key_code::KEY_Q: return GLFW_KEY_Q;
            case key_code::KEY_R: return GLFW_KEY_R;
            case key_code::KEY_S: return GLFW_KEY_S;
            case key_code::KEY_T: return GLFW_KEY_T;
            case key_code::KEY_U: return GLFW_KEY_U;
            case key_code::KEY_V: return GLFW_KEY_V;
            case key_code::KEY_W: return GLFW_KEY_W;
            case key_code::KEY_X: return GLFW_KEY_X;
            case key_code::KEY_Y: return GLFW_KEY_Y;
            case key_code::KEY_Z: return GLFW_KEY_Z;
            case key_code::KEY_LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;       /* [ */
            case key_code::KEY_BACKSLASH: return GLFW_KEY_BACKSLASH;             /* \ */
            case key_code::KEY_RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;     /* ] */
            case key_code::KEY_GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;       /* ` */

                /* Function keys */
            case key_code::KEY_ESCAPE: return GLFW_KEY_ESCAPE;
            case key_code::KEY_ENTER: return GLFW_KEY_ENTER;
            case key_code::KEY_TAB: return GLFW_KEY_TAB;
            case key_code::KEY_BACKSPACE: return GLFW_KEY_BACKSPACE;
            case key_code::KEY_INSERT: return GLFW_KEY_INSERT;
            case key_code::KEY_DELETE: return GLFW_KEY_DELETE;
            case key_code::KEY_RIGHT: return GLFW_KEY_RIGHT;
            case key_code::KEY_LEFT: return GLFW_KEY_LEFT;
            case key_code::KEY_DOWN: return GLFW_KEY_DOWN;
            case key_code::KEY_UP: return GLFW_KEY_UP;
            case key_code::KEY_PAGE_UP: return GLFW_KEY_PAGE_UP;
            case key_code::KEY_PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
            case key_code::KEY_HOME: return GLFW_KEY_HOME;
            case key_code::KEY_END: return GLFW_KEY_END;
            case key_code::KEY_CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
            case key_code::KEY_SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
            case key_code::KEY_PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
            case key_code::KEY_PAUSE: return GLFW_KEY_PAUSE;
            case key_code::KEY_F1: return GLFW_KEY_F1;
            case key_code::KEY_F2: return GLFW_KEY_F2;
            case key_code::KEY_F3: return GLFW_KEY_F3;
            case key_code::KEY_F4: return GLFW_KEY_F4;
            case key_code::KEY_F5: return GLFW_KEY_F5;
            case key_code::KEY_F6: return GLFW_KEY_F6;
            case key_code::KEY_F7: return GLFW_KEY_F7;
            case key_code::KEY_F8: return GLFW_KEY_F8;
            case key_code::KEY_F9: return GLFW_KEY_F9;
            case key_code::KEY_F10: return GLFW_KEY_F10;
            case key_code::KEY_F11: return GLFW_KEY_F11;
            case key_code::KEY_F12: return GLFW_KEY_F12;
            case key_code::KEY_F13: return GLFW_KEY_F13;
            case key_code::KEY_F14: return GLFW_KEY_F14;
            case key_code::KEY_F15: return GLFW_KEY_F15;
            case key_code::KEY_F16: return GLFW_KEY_F16;
            case key_code::KEY_F17: return GLFW_KEY_F17;
            case key_code::KEY_F18: return GLFW_KEY_F18;
            case key_code::KEY_F19: return GLFW_KEY_F19;
            case key_code::KEY_F20: return GLFW_KEY_F20;
            case key_code::KEY_F21: return GLFW_KEY_F21;
            case key_code::KEY_F22: return GLFW_KEY_F22;
            case key_code::KEY_F23: return GLFW_KEY_F23;
            case key_code::KEY_F24: return GLFW_KEY_F24;
            case key_code::KEY_KP_0: return GLFW_KEY_KP_0;
            case key_code::KEY_KP_1: return GLFW_KEY_KP_1;
            case key_code::KEY_KP_2: return GLFW_KEY_KP_2;
            case key_code::KEY_KP_3: return GLFW_KEY_KP_3;
            case key_code::KEY_KP_4: return GLFW_KEY_KP_4;
            case key_code::KEY_KP_5: return GLFW_KEY_KP_5;
            case key_code::KEY_KP_6: return GLFW_KEY_KP_6;
            case key_code::KEY_KP_7: return GLFW_KEY_KP_7;
            case key_code::KEY_KP_8: return GLFW_KEY_KP_8;
            case key_code::KEY_KP_9: return GLFW_KEY_KP_9;
            case key_code::KEY_KP_DECIMAL: return GLFW_KEY_KP_DECIMAL;
            case key_code::KEY_KP_DIVIDE: return GLFW_KEY_KP_DIVIDE;
            case key_code::KEY_KP_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
            case key_code::KEY_KP_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
            case key_code::KEY_KP_ADD: return GLFW_KEY_KP_ADD;
            case key_code::KEY_KP_ENTER: return GLFW_KEY_KP_ENTER;
            case key_code::KEY_KP_EQUAL: return GLFW_KEY_KP_EQUAL;
            case key_code::KEY_LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
            case key_code::KEY_LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
            case key_code::KEY_LEFT_ALT: return GLFW_KEY_LEFT_ALT;
            case key_code::KEY_RIGHT_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
            case key_code::KEY_RIGHT_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
            case key_code::KEY_RIGHT_ALT: return GLFW_KEY_RIGHT_ALT;
            case key_code::KEY_MENU: return GLFW_KEY_MENU;
            }

            log::warn("Unknown key_code");
            return GLFW_KEY_UNKNOWN;
        }

        key_code convert_key_to_ppp(int code)
        {
            /* The unknown key */
            switch (code)
            {
            case GLFW_KEY_UNKNOWN: return key_code::KEY_UNKNOWN;

                /* Printable keys */
            case GLFW_KEY_SPACE: return key_code::KEY_SPACE;
            case GLFW_KEY_APOSTROPHE: return key_code::KEY_APOSTROPHE;           /* ' */
            case GLFW_KEY_COMMA: return key_code::KEY_COMMA;                     /* , */
            case GLFW_KEY_MINUS: return key_code::KEY_MINUS;                     /* - */
            case GLFW_KEY_PERIOD: return key_code::KEY_PERIOD;                   /* . */
            case GLFW_KEY_SLASH: return key_code::KEY_SLASH;                     /* / */
            case GLFW_KEY_0: return key_code::KEY_0;
            case GLFW_KEY_1: return key_code::KEY_1;
            case GLFW_KEY_2: return key_code::KEY_2;
            case GLFW_KEY_3: return key_code::KEY_3;
            case GLFW_KEY_4: return key_code::KEY_4;
            case GLFW_KEY_5: return key_code::KEY_5;
            case GLFW_KEY_6: return key_code::KEY_6;
            case GLFW_KEY_7: return key_code::KEY_7;
            case GLFW_KEY_8: return key_code::KEY_8;
            case GLFW_KEY_9: return key_code::KEY_9;
            case GLFW_KEY_SEMICOLON: return key_code::KEY_SEMICOLON;             /* ; */
            case GLFW_KEY_EQUAL: return key_code::KEY_EQUAL;                     /* = */
            case GLFW_KEY_A: return key_code::KEY_A;
            case GLFW_KEY_B: return key_code::KEY_B;
            case GLFW_KEY_C: return key_code::KEY_C;
            case GLFW_KEY_D: return key_code::KEY_D;
            case GLFW_KEY_E: return key_code::KEY_E;
            case GLFW_KEY_F: return key_code::KEY_F;
            case GLFW_KEY_G: return key_code::KEY_G;
            case GLFW_KEY_H: return key_code::KEY_H;
            case GLFW_KEY_I: return key_code::KEY_I;
            case GLFW_KEY_J: return key_code::KEY_J;
            case GLFW_KEY_K: return key_code::KEY_K;
            case GLFW_KEY_L: return key_code::KEY_L;
            case GLFW_KEY_M: return key_code::KEY_M;
            case GLFW_KEY_N: return key_code::KEY_N;
            case GLFW_KEY_O: return key_code::KEY_O;
            case GLFW_KEY_P: return key_code::KEY_P;
            case GLFW_KEY_Q: return key_code::KEY_Q;
            case GLFW_KEY_R: return key_code::KEY_R;
            case GLFW_KEY_S: return key_code::KEY_S;
            case GLFW_KEY_T: return key_code::KEY_T;
            case GLFW_KEY_U: return key_code::KEY_U;
            case GLFW_KEY_V: return key_code::KEY_V;
            case GLFW_KEY_W: return key_code::KEY_W;
            case GLFW_KEY_X: return key_code::KEY_X;
            case GLFW_KEY_Y: return key_code::KEY_Y;
            case GLFW_KEY_Z: return key_code::KEY_Z;
            case GLFW_KEY_LEFT_BRACKET: return key_code::KEY_LEFT_BRACKET;       /* [ */
            case GLFW_KEY_BACKSLASH: return key_code::KEY_BACKSLASH;             /* \ */
            case GLFW_KEY_RIGHT_BRACKET: return key_code::KEY_RIGHT_BRACKET;     /* ] */
            case GLFW_KEY_GRAVE_ACCENT: return key_code::KEY_GRAVE_ACCENT;       /* ` */

                /* Function keys */
            case GLFW_KEY_ESCAPE: return key_code::KEY_ESCAPE;
            case GLFW_KEY_ENTER: return key_code::KEY_ENTER;
            case GLFW_KEY_TAB: return key_code::KEY_TAB;
            case GLFW_KEY_BACKSPACE: return key_code::KEY_BACKSPACE;
            case GLFW_KEY_INSERT: return key_code::KEY_INSERT;
            case GLFW_KEY_DELETE: return key_code::KEY_DELETE;
            case GLFW_KEY_RIGHT: return key_code::KEY_RIGHT;
            case GLFW_KEY_LEFT: return key_code::KEY_LEFT;
            case GLFW_KEY_DOWN: return key_code::KEY_DOWN;
            case GLFW_KEY_UP: return key_code::KEY_UP;
            case GLFW_KEY_PAGE_UP: return key_code::KEY_PAGE_UP;
            case GLFW_KEY_PAGE_DOWN: return key_code::KEY_PAGE_DOWN;
            case GLFW_KEY_HOME: return key_code::KEY_HOME;
            case GLFW_KEY_END: return key_code::KEY_END;
            case GLFW_KEY_CAPS_LOCK: return key_code::KEY_CAPS_LOCK;
            case GLFW_KEY_SCROLL_LOCK: return key_code::KEY_SCROLL_LOCK;
            case GLFW_KEY_PRINT_SCREEN: return key_code::KEY_PRINT_SCREEN;
            case GLFW_KEY_PAUSE: return key_code::KEY_PAUSE;
            case GLFW_KEY_F1: return key_code::KEY_F1;
            case GLFW_KEY_F2: return key_code::KEY_F2;
            case GLFW_KEY_F3: return key_code::KEY_F3;
            case GLFW_KEY_F4: return key_code::KEY_F4;
            case GLFW_KEY_F5: return key_code::KEY_F5;
            case GLFW_KEY_F6: return key_code::KEY_F6;
            case GLFW_KEY_F7: return key_code::KEY_F7;
            case GLFW_KEY_F8: return key_code::KEY_F8;
            case GLFW_KEY_F9: return key_code::KEY_F9;
            case GLFW_KEY_F10: return key_code::KEY_F10;
            case GLFW_KEY_F11: return key_code::KEY_F11;
            case GLFW_KEY_F12: return key_code::KEY_F12;
            case GLFW_KEY_F13: return key_code::KEY_F13;
            case GLFW_KEY_F14: return key_code::KEY_F14;
            case GLFW_KEY_F15: return key_code::KEY_F15;
            case GLFW_KEY_F16: return key_code::KEY_F16;
            case GLFW_KEY_F17: return key_code::KEY_F17;
            case GLFW_KEY_F18: return key_code::KEY_F18;
            case GLFW_KEY_F19: return key_code::KEY_F19;
            case GLFW_KEY_F20: return key_code::KEY_F20;
            case GLFW_KEY_F21: return key_code::KEY_F21;
            case GLFW_KEY_F22: return key_code::KEY_F22;
            case GLFW_KEY_F23: return key_code::KEY_F23;
            case GLFW_KEY_F24: return key_code::KEY_F24;
            case GLFW_KEY_KP_0: return key_code::KEY_KP_0;
            case GLFW_KEY_KP_1: return key_code::KEY_KP_1;
            case GLFW_KEY_KP_2: return key_code::KEY_KP_2;
            case GLFW_KEY_KP_3: return key_code::KEY_KP_3;
            case GLFW_KEY_KP_4: return key_code::KEY_KP_4;
            case GLFW_KEY_KP_5: return key_code::KEY_KP_5;
            case GLFW_KEY_KP_6: return key_code::KEY_KP_6;
            case GLFW_KEY_KP_7: return key_code::KEY_KP_7;
            case GLFW_KEY_KP_8: return key_code::KEY_KP_8;
            case GLFW_KEY_KP_9: return key_code::KEY_KP_9;
            case GLFW_KEY_KP_DECIMAL: return key_code::KEY_KP_DECIMAL;
            case GLFW_KEY_KP_DIVIDE: return key_code::KEY_KP_DIVIDE;
            case GLFW_KEY_KP_MULTIPLY: return key_code::KEY_KP_MULTIPLY;
            case GLFW_KEY_KP_SUBTRACT: return key_code::KEY_KP_SUBTRACT;
            case GLFW_KEY_KP_ADD: return key_code::KEY_KP_ADD;
            case GLFW_KEY_KP_ENTER: return key_code::KEY_KP_ENTER;
            case GLFW_KEY_KP_EQUAL: return key_code::KEY_KP_EQUAL;
            case GLFW_KEY_LEFT_SHIFT: return key_code::KEY_LEFT_SHIFT;
            case GLFW_KEY_LEFT_CONTROL: return key_code::KEY_LEFT_CONTROL;
            case GLFW_KEY_LEFT_ALT: return key_code::KEY_LEFT_ALT;
            case GLFW_KEY_RIGHT_SHIFT: return key_code::KEY_RIGHT_SHIFT;
            case GLFW_KEY_RIGHT_CONTROL: return key_code::KEY_RIGHT_CONTROL;
            case GLFW_KEY_RIGHT_ALT: return key_code::KEY_RIGHT_ALT;
            case GLFW_KEY_MENU: return key_code::KEY_MENU;
            }

            log::warn("Unknown key_code");
            return key_code::KEY_UNKNOWN;
        }
    }

    bool is_key_pressed(key_code code)
    {
        return device::input::is_key_pressed(internal::convert_key_to_glfw(code));
    }

    bool is_key_released(key_code code)
    {
        return device::input::is_key_released(internal::convert_key_to_glfw(code));
    }

    bool is_key_down(key_code code)
    {
        return device::input::is_key_down(internal::convert_key_to_glfw(code));
    }

    bool is_any_key_pressed()
    {
        return device::input::is_any_key_pressed();
    }

    bool is_any_key_released()
    {
        return device::input::is_any_key_released();
    }

    bool is_any_key_down()
    {
        return device::input::is_any_key_down();
    }

    key_code key()
    {
        return internal::convert_key_to_ppp(device::input::key());
    }

    void add_key_pressed_callback(const std::function<void(key_code)>& callback)
    {
        device::input::add_key_pressed_callback(
            [callback](int key, int scancode, int mods)
        {
            callback(internal::convert_key_to_ppp(key));
        });
    }

    void add_key_released_callback(const std::function<void(key_code)>& callback)
    {
        device::input::add_key_released_callback(
            [callback](int key, int scancode, int mods)
        {
            callback(internal::convert_key_to_ppp(key));
        });
    }

    void add_key_down_callback(const std::function<void(key_code)>& callback)
    {
        device::input::add_key_down_callback(
            [callback](int key, int scancode, int mods)
        {
            callback(internal::convert_key_to_ppp(key));
        });
    }

    void set_quit_application_keycode(key_code code)
    {
        add_key_pressed_callback(
            [code](key_code key)
        {
            if (key == code)
            {
                quit();
            }
        });
    }
    namespace internal
    {
        int convert_mouse_code_to_glfw(mouse_code code)
        {
            switch (code)
            {
            case mouse_code::BUTTON_LEFT: return GLFW_MOUSE_BUTTON_LEFT;
            case mouse_code::BUTTON_RIGHT: return GLFW_MOUSE_BUTTON_RIGHT;
            case mouse_code::BUTTON_MIDDLE: return GLFW_MOUSE_BUTTON_MIDDLE;
            }

            log::warn("Unknown mouse_code, returning Left Click");
            return GLFW_MOUSE_BUTTON_LEFT;
        }

        mouse_code convert_mouse_code_to_ppp(int code)
        {
            switch (code)
            {
            case GLFW_MOUSE_BUTTON_LEFT: return mouse_code::BUTTON_LEFT;
            case GLFW_MOUSE_BUTTON_RIGHT: return mouse_code::BUTTON_RIGHT;
            case GLFW_MOUSE_BUTTON_MIDDLE: return mouse_code::BUTTON_MIDDLE;
            }

            log::warn("Unknown mouse_code, returning Left Click");
            return mouse_code::BUTTON_LEFT;
        }
    }

    float moved_x()
    {
        return device::input::moved_x();
    }

    float moved_y()
    {
        return device::input::moved_y();
    }

    float mouse_x()
    {
        return device::input::mouse_x();
    }

    float mouse_y()
    {
        return device::input::mouse_y();
    }

    float prev_mouse_x()
    {
        return device::input::prev_mouse_x();
    }

    float prev_mouse_y()
    {
        return device::input::prev_mouse_y();
    }

    mouse_code mouse_button()
    {
        return internal::convert_mouse_code_to_ppp(device::input::mouse_button());
    }

    float scroll_offset_x()
    {
        return device::input::scroll_offset_x();
    }

    float scroll_offset_y()
    {
        return device::input::scroll_offset_y();
    }

    bool is_left_button_pressed()
    {
        return device::input::is_left_button_pressed();
    }

    bool is_right_button_pressed()
    {
        return device::input::is_right_button_pressed();
    }

    bool is_middle_button_pressed()
    {
        return device::input::is_middle_button_pressed();
    }

    bool is_any_mouse_button_pressed()
    {
        return device::input::is_any_mouse_button_pressed();
    }

    void add_mouse_moved_callback(const std::function<void(float, float)>& callback)
    {
        device::input::add_mouse_moved_callback(callback);
    }

    void add_mouse_dragged_callback(const std::function<void(float, float)>& callback)
    {
        device::input::add_mouse_dragged_callback(callback);
    }

    void add_mouse_pressed_callback(const std::function<void(mouse_code)>& callback)
    {
        device::input::add_mouse_pressed_callback(
            [callback](int button, int mods)
        {
            callback(internal::convert_mouse_code_to_ppp(button));
        });
    }

    void add_mouse_released_callback(const std::function<void(mouse_code)>& callback)
    {
        device::input::add_mouse_released_callback(
            [callback](int button, int mods)
        {
            callback(internal::convert_mouse_code_to_ppp(button));
        });
    }

    void add_mouse_horizontal_wheel_callback(const std::function<void(float)>& callback)
    {
        device::input::add_mouse_horizontal_wheel_callback(callback);
    }

    void add_mouse_verticel_wheel_callback(const std::function<void(float)>& callback)
    {
        device::input::add_mouse_vertical_wheel_callback(callback);
    }

    void request_pointer_lock()
    {
        device::input::request_pointer_lock();
    }

    void request_pointer_hide()
    {
        device::input::request_pointer_hide();
    }

    void request_pointer_unlock()
    {
        device::input::request_pointer_unlock();
    }

    void request_pointer_show()
    {
        device::input::request_pointer_show();
    }
}