#include "events.h"
#include "structure.h"
#include "device/device.h"
#include "util/log.h"

#include <GLFW/glfw3.h>

namespace ppp
{
    namespace keyboard
    {
        namespace internal
        {
            int convert_key_to_glfw(KeyCode code)
            {
                /* The unknown key */
                switch (code)
                {
                case KeyCode::KEY_UNKNOWN: return GLFW_KEY_UNKNOWN;

                /* Printable keys */
                case KeyCode::KEY_SPACE: return GLFW_KEY_SPACE;
                case KeyCode::KEY_APOSTROPHE: return GLFW_KEY_APOSTROPHE;           /* ' */
                case KeyCode::KEY_COMMA: return GLFW_KEY_COMMA;                     /* , */
                case KeyCode::KEY_MINUS: return GLFW_KEY_MINUS;                     /* - */
                case KeyCode::KEY_PERIOD: return GLFW_KEY_PERIOD;                   /* . */
                case KeyCode::KEY_SLASH: return GLFW_KEY_SLASH;                     /* / */
                case KeyCode::KEY_0: return GLFW_KEY_1;
                case KeyCode::KEY_1: return GLFW_KEY_2;
                case KeyCode::KEY_2: return GLFW_KEY_3;
                case KeyCode::KEY_3: return GLFW_KEY_4;
                case KeyCode::KEY_4: return GLFW_KEY_5;
                case KeyCode::KEY_5: return GLFW_KEY_6;
                case KeyCode::KEY_6: return GLFW_KEY_7;
                case KeyCode::KEY_7: return GLFW_KEY_8;
                case KeyCode::KEY_8: return GLFW_KEY_9;
                case KeyCode::KEY_9: return GLFW_KEY_0;
                case KeyCode::KEY_SEMICOLON: return GLFW_KEY_SEMICOLON;             /* ; */
                case KeyCode::KEY_EQUAL: return GLFW_KEY_EQUAL;                     /* = */
                case KeyCode::KEY_A: return GLFW_KEY_A;
                case KeyCode::KEY_B: return GLFW_KEY_B;
                case KeyCode::KEY_C: return GLFW_KEY_C;
                case KeyCode::KEY_D: return GLFW_KEY_D;
                case KeyCode::KEY_E: return GLFW_KEY_E;
                case KeyCode::KEY_F: return GLFW_KEY_F;
                case KeyCode::KEY_G: return GLFW_KEY_G;
                case KeyCode::KEY_H: return GLFW_KEY_H;
                case KeyCode::KEY_I: return GLFW_KEY_I;
                case KeyCode::KEY_J: return GLFW_KEY_J;
                case KeyCode::KEY_K: return GLFW_KEY_K;
                case KeyCode::KEY_L: return GLFW_KEY_L;
                case KeyCode::KEY_M: return GLFW_KEY_M;
                case KeyCode::KEY_N: return GLFW_KEY_N;
                case KeyCode::KEY_O: return GLFW_KEY_O;
                case KeyCode::KEY_P: return GLFW_KEY_P;
                case KeyCode::KEY_Q: return GLFW_KEY_Q;
                case KeyCode::KEY_R: return GLFW_KEY_R;
                case KeyCode::KEY_S: return GLFW_KEY_S;
                case KeyCode::KEY_T: return GLFW_KEY_T;
                case KeyCode::KEY_U: return GLFW_KEY_U;
                case KeyCode::KEY_V: return GLFW_KEY_V;
                case KeyCode::KEY_W: return GLFW_KEY_W;
                case KeyCode::KEY_X: return GLFW_KEY_X;
                case KeyCode::KEY_Y: return GLFW_KEY_Y;
                case KeyCode::KEY_Z: return GLFW_KEY_Z;
                case KeyCode::KEY_LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;       /* [ */
                case KeyCode::KEY_BACKSLASH: return GLFW_KEY_BACKSLASH;             /* \ */
                case KeyCode::KEY_RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;     /* ] */
                case KeyCode::KEY_GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;       /* ` */

                /* Function keys */
                case KeyCode::KEY_ESCAPE: return GLFW_KEY_ESCAPE;
                case KeyCode::KEY_ENTER: return GLFW_KEY_ENTER;
                case KeyCode::KEY_TAB: return GLFW_KEY_TAB;
                case KeyCode::KEY_BACKSPACE: return GLFW_KEY_BACKSPACE;
                case KeyCode::KEY_INSERT: return GLFW_KEY_INSERT;
                case KeyCode::KEY_DELETE: return GLFW_KEY_DELETE;
                case KeyCode::KEY_RIGHT: return GLFW_KEY_RIGHT;
                case KeyCode::KEY_LEFT: return GLFW_KEY_LEFT;
                case KeyCode::KEY_DOWN: return GLFW_KEY_DOWN;
                case KeyCode::KEY_UP: return GLFW_KEY_UP;
                case KeyCode::KEY_PAGE_UP: return GLFW_KEY_PAGE_UP;
                case KeyCode::KEY_PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
                case KeyCode::KEY_HOME: return GLFW_KEY_HOME;
                case KeyCode::KEY_END: return GLFW_KEY_END;
                case KeyCode::KEY_CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
                case KeyCode::KEY_SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
                case KeyCode::KEY_PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
                case KeyCode::KEY_PAUSE: return GLFW_KEY_PAUSE;
                case KeyCode::KEY_F1: return GLFW_KEY_F1;
                case KeyCode::KEY_F2: return GLFW_KEY_F2;
                case KeyCode::KEY_F3: return GLFW_KEY_F3;
                case KeyCode::KEY_F4: return GLFW_KEY_F4;
                case KeyCode::KEY_F5: return GLFW_KEY_F5;
                case KeyCode::KEY_F6: return GLFW_KEY_F6;
                case KeyCode::KEY_F7: return GLFW_KEY_F7;
                case KeyCode::KEY_F8: return GLFW_KEY_F8;
                case KeyCode::KEY_F9: return GLFW_KEY_F9;
                case KeyCode::KEY_F10: return GLFW_KEY_F10;
                case KeyCode::KEY_F11: return GLFW_KEY_F11;
                case KeyCode::KEY_F12: return GLFW_KEY_F12;
                case KeyCode::KEY_F13: return GLFW_KEY_F13;
                case KeyCode::KEY_F14: return GLFW_KEY_F14;
                case KeyCode::KEY_F15: return GLFW_KEY_F15;
                case KeyCode::KEY_F16: return GLFW_KEY_F16;
                case KeyCode::KEY_F17: return GLFW_KEY_F17;
                case KeyCode::KEY_F18: return GLFW_KEY_F18;
                case KeyCode::KEY_F19: return GLFW_KEY_F19;
                case KeyCode::KEY_F20: return GLFW_KEY_F20;
                case KeyCode::KEY_F21: return GLFW_KEY_F21;
                case KeyCode::KEY_F22: return GLFW_KEY_F22;
                case KeyCode::KEY_F23: return GLFW_KEY_F23;
                case KeyCode::KEY_F24: return GLFW_KEY_F24;
                case KeyCode::KEY_KP_0: return GLFW_KEY_KP_0;
                case KeyCode::KEY_KP_1: return GLFW_KEY_KP_1;
                case KeyCode::KEY_KP_2: return GLFW_KEY_KP_2;
                case KeyCode::KEY_KP_3: return GLFW_KEY_KP_3;
                case KeyCode::KEY_KP_4: return GLFW_KEY_KP_4;
                case KeyCode::KEY_KP_5: return GLFW_KEY_KP_5;
                case KeyCode::KEY_KP_6: return GLFW_KEY_KP_6;
                case KeyCode::KEY_KP_7: return GLFW_KEY_KP_7;
                case KeyCode::KEY_KP_8: return GLFW_KEY_KP_8;
                case KeyCode::KEY_KP_9: return GLFW_KEY_KP_9;
                case KeyCode::KEY_KP_DECIMAL: return GLFW_KEY_KP_DECIMAL;
                case KeyCode::KEY_KP_DIVIDE: return GLFW_KEY_KP_DIVIDE;
                case KeyCode::KEY_KP_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
                case KeyCode::KEY_KP_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
                case KeyCode::KEY_KP_ADD: return GLFW_KEY_KP_ADD;
                case KeyCode::KEY_KP_ENTER: return GLFW_KEY_KP_ENTER;
                case KeyCode::KEY_KP_EQUAL: return GLFW_KEY_KP_EQUAL;
                case KeyCode::KEY_LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
                case KeyCode::KEY_LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
                case KeyCode::KEY_LEFT_ALT: return GLFW_KEY_LEFT_ALT;
                case KeyCode::KEY_RIGHT_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
                case KeyCode::KEY_RIGHT_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
                case KeyCode::KEY_RIGHT_ALT: return GLFW_KEY_RIGHT_ALT;
                case KeyCode::KEY_MENU: return GLFW_KEY_MENU;
                }

                log::warn("Unknown KeyCode");
                return GLFW_KEY_UNKNOWN;
            }

            KeyCode convert_key_to_ppp(int code)
            {
                /* The unknown key */
                switch (code)
                {
                case GLFW_KEY_UNKNOWN: return KeyCode::KEY_UNKNOWN;

                /* Printable keys */
                case GLFW_KEY_SPACE: return KeyCode::KEY_SPACE;
                case GLFW_KEY_APOSTROPHE: return KeyCode::KEY_APOSTROPHE;           /* ' */
                case GLFW_KEY_COMMA: return KeyCode::KEY_COMMA;                     /* , */
                case GLFW_KEY_MINUS: return KeyCode::KEY_MINUS;                     /* - */
                case GLFW_KEY_PERIOD: return KeyCode::KEY_PERIOD;                   /* . */
                case GLFW_KEY_SLASH: return KeyCode::KEY_SLASH;                     /* / */
                case GLFW_KEY_1: return KeyCode::KEY_0;
                case GLFW_KEY_2: return KeyCode::KEY_1;
                case GLFW_KEY_3: return KeyCode::KEY_2;
                case GLFW_KEY_4: return KeyCode::KEY_3;
                case GLFW_KEY_5: return KeyCode::KEY_4;
                case GLFW_KEY_6: return KeyCode::KEY_5;
                case GLFW_KEY_7: return KeyCode::KEY_6;
                case GLFW_KEY_8: return KeyCode::KEY_7;
                case GLFW_KEY_9: return KeyCode::KEY_8;
                case GLFW_KEY_0: return KeyCode::KEY_9;
                case GLFW_KEY_SEMICOLON: return KeyCode::KEY_SEMICOLON;             /* ; */
                case GLFW_KEY_EQUAL: return KeyCode::KEY_EQUAL;                     /* = */
                case GLFW_KEY_A: return KeyCode::KEY_A;
                case GLFW_KEY_B: return KeyCode::KEY_B;
                case GLFW_KEY_C: return KeyCode::KEY_C;
                case GLFW_KEY_D: return KeyCode::KEY_D;
                case GLFW_KEY_E: return KeyCode::KEY_E;
                case GLFW_KEY_F: return KeyCode::KEY_F;
                case GLFW_KEY_G: return KeyCode::KEY_G;
                case GLFW_KEY_H: return KeyCode::KEY_H;
                case GLFW_KEY_I: return KeyCode::KEY_I;
                case GLFW_KEY_J: return KeyCode::KEY_J;
                case GLFW_KEY_K: return KeyCode::KEY_K;
                case GLFW_KEY_L: return KeyCode::KEY_L;
                case GLFW_KEY_M: return KeyCode::KEY_M;
                case GLFW_KEY_N: return KeyCode::KEY_N;
                case GLFW_KEY_O: return KeyCode::KEY_O;
                case GLFW_KEY_P: return KeyCode::KEY_P;
                case GLFW_KEY_Q: return KeyCode::KEY_Q;
                case GLFW_KEY_R: return KeyCode::KEY_R;
                case GLFW_KEY_S: return KeyCode::KEY_S;
                case GLFW_KEY_T: return KeyCode::KEY_T;
                case GLFW_KEY_U: return KeyCode::KEY_U;
                case GLFW_KEY_V: return KeyCode::KEY_V;
                case GLFW_KEY_W: return KeyCode::KEY_W;
                case GLFW_KEY_X: return KeyCode::KEY_X;
                case GLFW_KEY_Y: return KeyCode::KEY_Y;
                case GLFW_KEY_Z: return KeyCode::KEY_Z;
                case GLFW_KEY_LEFT_BRACKET: return KeyCode::KEY_LEFT_BRACKET;       /* [ */
                case GLFW_KEY_BACKSLASH: return KeyCode::KEY_BACKSLASH;             /* \ */
                case GLFW_KEY_RIGHT_BRACKET: return KeyCode::KEY_RIGHT_BRACKET;     /* ] */
                case GLFW_KEY_GRAVE_ACCENT: return KeyCode::KEY_GRAVE_ACCENT;       /* ` */

                /* Function keys */
                case GLFW_KEY_ESCAPE: return KeyCode::KEY_ESCAPE;
                case GLFW_KEY_ENTER: return KeyCode::KEY_ENTER;
                case GLFW_KEY_TAB: return KeyCode::KEY_TAB;
                case GLFW_KEY_BACKSPACE: return KeyCode::KEY_BACKSPACE;
                case GLFW_KEY_INSERT: return KeyCode::KEY_INSERT;
                case GLFW_KEY_DELETE: return KeyCode::KEY_DELETE;
                case GLFW_KEY_RIGHT: return KeyCode::KEY_RIGHT;
                case GLFW_KEY_LEFT: return KeyCode::KEY_LEFT;
                case GLFW_KEY_DOWN: return KeyCode::KEY_DOWN;
                case GLFW_KEY_UP: return KeyCode::KEY_UP;
                case GLFW_KEY_PAGE_UP: return KeyCode::KEY_PAGE_UP;
                case GLFW_KEY_PAGE_DOWN: return KeyCode::KEY_PAGE_DOWN;
                case GLFW_KEY_HOME: return KeyCode::KEY_HOME;
                case GLFW_KEY_END: return KeyCode::KEY_END;
                case GLFW_KEY_CAPS_LOCK: return KeyCode::KEY_CAPS_LOCK;
                case GLFW_KEY_SCROLL_LOCK: return KeyCode::KEY_SCROLL_LOCK;
                case GLFW_KEY_PRINT_SCREEN: return KeyCode::KEY_PRINT_SCREEN;
                case GLFW_KEY_PAUSE: return KeyCode::KEY_PAUSE;
                case GLFW_KEY_F1: return KeyCode::KEY_F1;
                case GLFW_KEY_F2: return KeyCode::KEY_F2;
                case GLFW_KEY_F3: return KeyCode::KEY_F3;
                case GLFW_KEY_F4: return KeyCode::KEY_F4;
                case GLFW_KEY_F5: return KeyCode::KEY_F5;
                case GLFW_KEY_F6: return KeyCode::KEY_F6;
                case GLFW_KEY_F7: return KeyCode::KEY_F7;
                case GLFW_KEY_F8: return KeyCode::KEY_F8;
                case GLFW_KEY_F9: return KeyCode::KEY_F9;
                case GLFW_KEY_F10: return KeyCode::KEY_F10;
                case GLFW_KEY_F11: return KeyCode::KEY_F11;
                case GLFW_KEY_F12: return KeyCode::KEY_F12;
                case GLFW_KEY_F13: return KeyCode::KEY_F13;
                case GLFW_KEY_F14: return KeyCode::KEY_F14;
                case GLFW_KEY_F15: return KeyCode::KEY_F15;
                case GLFW_KEY_F16: return KeyCode::KEY_F16;
                case GLFW_KEY_F17: return KeyCode::KEY_F17;
                case GLFW_KEY_F18: return KeyCode::KEY_F18;
                case GLFW_KEY_F19: return KeyCode::KEY_F19;
                case GLFW_KEY_F20: return KeyCode::KEY_F20;
                case GLFW_KEY_F21: return KeyCode::KEY_F21;
                case GLFW_KEY_F22: return KeyCode::KEY_F22;
                case GLFW_KEY_F23: return KeyCode::KEY_F23;
                case GLFW_KEY_F24: return KeyCode::KEY_F24;
                case GLFW_KEY_KP_0: return KeyCode::KEY_KP_0;
                case GLFW_KEY_KP_1: return KeyCode::KEY_KP_1;
                case GLFW_KEY_KP_2: return KeyCode::KEY_KP_2;
                case GLFW_KEY_KP_3: return KeyCode::KEY_KP_3;
                case GLFW_KEY_KP_4: return KeyCode::KEY_KP_4;
                case GLFW_KEY_KP_5: return KeyCode::KEY_KP_5;
                case GLFW_KEY_KP_6: return KeyCode::KEY_KP_6;
                case GLFW_KEY_KP_7: return KeyCode::KEY_KP_7;
                case GLFW_KEY_KP_8: return KeyCode::KEY_KP_8;
                case GLFW_KEY_KP_9: return KeyCode::KEY_KP_9;
                case GLFW_KEY_KP_DECIMAL: return KeyCode::KEY_KP_DECIMAL;
                case GLFW_KEY_KP_DIVIDE: return KeyCode::KEY_KP_DIVIDE;
                case GLFW_KEY_KP_MULTIPLY: return KeyCode::KEY_KP_MULTIPLY;
                case GLFW_KEY_KP_SUBTRACT: return KeyCode::KEY_KP_SUBTRACT;
                case GLFW_KEY_KP_ADD: return KeyCode::KEY_KP_ADD;
                case GLFW_KEY_KP_ENTER: return KeyCode::KEY_KP_ENTER;
                case GLFW_KEY_KP_EQUAL: return KeyCode::KEY_KP_EQUAL;
                case GLFW_KEY_LEFT_SHIFT: return KeyCode::KEY_LEFT_SHIFT;
                case GLFW_KEY_LEFT_CONTROL: return KeyCode::KEY_LEFT_CONTROL;
                case GLFW_KEY_LEFT_ALT: return KeyCode::KEY_LEFT_ALT;
                case GLFW_KEY_RIGHT_SHIFT: return KeyCode::KEY_RIGHT_SHIFT;
                case GLFW_KEY_RIGHT_CONTROL: return KeyCode::KEY_RIGHT_CONTROL;
                case GLFW_KEY_RIGHT_ALT: return KeyCode::KEY_RIGHT_ALT;
                case GLFW_KEY_MENU: return KeyCode::KEY_MENU;
                }

                log::warn("Unknown KeyCode");
                return KeyCode::KEY_UNKNOWN;
            }
        }

        bool is_key_pressed(KeyCode code)
        {
            return device::input::keyboard::is_key_pressed(internal::convert_key_to_glfw(code));
        }

        bool is_key_released(KeyCode code)
        {
            return device::input::keyboard::is_key_released(internal::convert_key_to_glfw(code));
        }

        bool is_key_down(KeyCode code)
        {
            return device::input::keyboard::is_key_down(internal::convert_key_to_glfw(code));
        }

        bool is_any_key_pressed()
        {
            return device::input::keyboard::is_any_key_pressed();
        }

        bool is_any_key_released()
        {
            return device::input::keyboard::is_any_key_released();
        }

        bool is_any_key_down()
        {
            return device::input::keyboard::is_any_key_down();
        }

        KeyCode key()
        {
            return internal::convert_key_to_ppp(device::input::keyboard::key());
        }

        void add_key_pressed_callback(const std::function<void(KeyCode)>& callback)
        {
            device::input::keyboard::add_key_pressed_callback(
                [callback](int key, int scancode, int mods)
            {
                callback(internal::convert_key_to_ppp(key));
            });
        }
        
        void add_key_released_callback(const std::function<void(KeyCode)>& callback)
        {
            device::input::keyboard::add_key_released_callback(
                [callback](int key, int scancode, int mods)
            {
                callback(internal::convert_key_to_ppp(key));
            });
        }
        
        void add_key_down_callback(const std::function<void(KeyCode)>& callback)
        {
            device::input::keyboard::add_key_down_callback(
                [callback](int key, int scancode, int mods)
            {
                callback(internal::convert_key_to_ppp(key));
            });
        }

        void set_quit_application_keycode(KeyCode code)
        {
            keyboard::add_key_pressed_callback(
                [code](keyboard::KeyCode key)
            {
                if (key == code)
                {
                    structure::quit();
                }
            });
        }
    }

    namespace mouse
    {
        namespace internal
        {
            int convert_key_to_glfw(mouse_code code)
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

            mouse_code convert_key_to_ppp(int code)
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
            return device::input::mouse::moved_x();
        }
        
        float moved_y()
        {
            return device::input::mouse::moved_y();
        }
        
        float mouse_x()
        {
            return device::input::mouse::mouse_x();
        }
        
        float mouse_y()
        {
            return device::input::mouse::mouse_y();
        }
        
        float prev_mouse_x()
        {
            return device::input::mouse::prev_mouse_x();
        }
        
        float prev_mouse_y()
        {
            return device::input::mouse::prev_mouse_y();
        }
        
        mouse_code mouse_button()
        {
            return internal::convert_key_to_ppp(device::input::mouse::mouse_button());
        }

        float scroll_offset_x()
        {
            return device::input::mouse::scroll_offset_x();
        }

        float scroll_offset_y()
        {
            return device::input::mouse::scroll_offset_y();
        }
        
        bool is_left_button_pressed()
        {
            return device::input::mouse::is_left_button_pressed();
        }
        
        bool is_right_button_pressed()
        {
            return device::input::mouse::is_right_button_pressed();
        }
        
        bool is_middle_button_pressed()
        {
            return device::input::mouse::is_middle_button_pressed();
        }

        bool is_any_mouse_button_pressed()
        {
            return device::input::mouse::is_any_mouse_button_pressed();
        }
        
        void add_mouse_moved_callback(const std::function<void(float, float)>& callback)
        {
            device::input::mouse::add_mouse_moved_callback(callback);
        }
        
        void add_mouse_dragged_callback(const std::function<void(float, float)>& callback)
        {
            device::input::mouse::add_mouse_dragged_callback(callback);
        }
        
        void add_mouse_pressed_callback(const std::function<void(mouse_code)>& callback)
        {
            device::input::mouse::add_mouse_pressed_callback(
                [callback](int button, int mods)
            {
                callback(internal::convert_key_to_ppp(button));
            });
        }
        
        void add_mouse_released_callback(const std::function<void(mouse_code)>& callback)
        {
            device::input::mouse::add_mouse_released_callback(
                [callback](int button, int mods)
            {
                callback(internal::convert_key_to_ppp(button));
            });
        }
        
        void add_mouse_horizontal_wheel_callback(const std::function<void(float)>& callback)
        {
            device::input::mouse::add_mouse_horizontal_wheel_callback(callback);
        }
        
        void add_mouse_verticel_wheel_callback(const std::function<void(float)>& callback)
        {
            device::input::mouse::add_mouse_vertical_wheel_callback(callback);
        }
        
        void request_pointer_lock()
        {
            device::input::mouse::request_pointer_lock();
        }

        void request_pointer_hide()
        {
            device::input::mouse::request_pointer_hide();
        }
        
        void request_pointer_unlock()
        {
            device::input::mouse::request_pointer_unlock();
        }

        void request_pointer_show()
        {
            device::input::mouse::request_pointer_show();
        }
    }
}