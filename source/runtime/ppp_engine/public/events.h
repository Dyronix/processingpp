/**
 * @file events.h
 * @brief Input handling of the sketch
 */
#pragma once

#include <functional>

namespace ppp
{
    /**
     * @brief Keyboard key codes enumeration.
     */
    enum class key_code
    {
        KEY_UNKNOWN,

        KEY_SPACE,
        KEY_APOSTROPHE,         
        KEY_COMMA,              
        KEY_MINUS,              
        KEY_PERIOD,             
        KEY_SLASH,              
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
        KEY_SEMICOLON,          
        KEY_EQUAL,              
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
        KEY_LEFT_BRACKET,       
        KEY_BACKSLASH,          
        KEY_RIGHT_BRACKET,      
        KEY_GRAVE_ACCENT,       

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

    /**
     * @brief Checks if a key was pressed during the current frame.
     * @param code The key code to check.
     * @return True if the key was pressed.
     */
    bool is_key_pressed(key_code code);

    /**
     * @brief Checks if a key was released during the current frame.
     * @param code The key code to check.
     * @return True if the key was released.
     */
    bool is_key_released(key_code code);

    /**
     * @brief Checks if a key is currently held down.
     * @param code The key code to check.
     * @return True if the key is down.
     */
    bool is_key_down(key_code code);

    /**
     * @brief Checks if any key was pressed during the current frame.
     * @return True if any key was pressed.
     */
    bool is_any_key_pressed();

    /**
     * @brief Checks if any key was released during the current frame.
     * @return True if any key was released.
     */
    bool is_any_key_released();

    /**
     * @brief Checks if any key is currently held down.
     * @return True if any key is down.
     */
    bool is_any_key_down();

    /**
     * @brief Gets the last key code event.
     * @return The last key code.
     */
    key_code key();

    /**
     * @brief Registers a callback for key pressed events.
     * @param callback Function receiving the pressed key code.
     */
    void add_key_pressed_callback(const std::function<void(key_code)>& callback);

    /**
     * @brief Registers a callback for key released events.
     * @param callback Function receiving the released key code.
     */
    void add_key_released_callback(const std::function<void(key_code)>& callback);

    /**
     * @brief Registers a callback for key down events.
     * @param callback Function receiving the key code while key is held.
     */
    void add_key_down_callback(const std::function<void(key_code)>& callback);

    /**
     * @brief Sets a specific key to trigger application quit when pressed.
     * @param code The key code that will quit the app on press.
     */
    void set_quit_application_keycode(key_code code);

    /**
     * @brief Mouse button codes enumeration.
     */
    enum class mouse_code
    {
        BUTTON_LEFT,
        BUTTON_MIDDLE,
        BUTTON_RIGHT
    };

    /**
     * @brief Gets the mouse movement delta in the X axis since last frame.
     * @return Delta X in pixels.
     */
    float moved_x();

    /**
     * @brief Gets the mouse movement delta in the Y axis since last frame.
     * @return Delta Y in pixels.
     */
    float moved_y();

    /**
     * @brief Gets the current mouse X position.
     * @return X coordinate in pixels.
     */
    float mouse_x();

    /**
     * @brief Gets the current mouse Y position.
     * @return Y coordinate in pixels.
     */
    float mouse_y();

    /**
     * @brief Gets the previous mouse X position from last frame.
     * @return Previous X coordinate in pixels.
     */
    float prev_mouse_x();

    /**
     * @brief Gets the previous mouse Y position from last frame.
     * @return Previous Y coordinate in pixels.
     */
    float prev_mouse_y();

    /**
     * @brief Gets the current mouse button code.
     * @return Mouse button code.
     */
    mouse_code mouse_button();

    /**
     * @brief Gets horizontal scroll offset since last frame.
     * @return Scroll offset in X.
     */
    float scroll_offset_x();

    /**
     * @brief Gets vertical scroll offset since last frame.
     * @return Scroll offset in Y.
     */
    float scroll_offset_y();

    /**
     * @brief Checks if the left mouse button is pressed.
     * @return True if left button is pressed.
     */
    bool is_left_button_pressed();

    /**
     * @brief Checks if the right mouse button is pressed.
     * @return True if right button is pressed.
     */
    bool is_right_button_pressed();

    /**
     * @brief Checks if the middle mouse button is pressed.
     * @return True if middle button is pressed.
     */
    bool is_middle_button_pressed();

    /**
     * @brief Checks if any mouse button is pressed.
     * @return True if any mouse button is pressed.
     */
    bool is_any_mouse_button_pressed();

    /**
     * @brief Registers a callback for mouse movement events.
     * @param callback Function receiving X and Y deltas.
     */
    void add_mouse_moved_callback(const std::function<void(float, float)>& callback);

    /**
     * @brief Registers a callback for mouse drag events.
     * @param callback Function receiving X and Y deltas.
     */
    void add_mouse_dragged_callback(const std::function<void(float, float)>& callback);

    /**
     * @brief Registers a callback for mouse button press events.
     * @param callback Function receiving the pressed mouse code.
     */
    void add_mouse_pressed_callback(const std::function<void(mouse_code)>& callback);

    /**
     * @brief Registers a callback for mouse button release events.
     * @param callback Function receiving the released mouse code.
     */
    void add_mouse_released_callback(const std::function<void(mouse_code)>& callback);

    /**
     * @brief Registers a callback for horizontal scroll wheel events.
     * @param callback Function receiving scroll offset in X.
     */
    void add_mouse_horizontal_wheel_callback(const std::function<void(float)>& callback);

    /**
     * @brief Registers a callback for vertical scroll wheel events.
     * @param callback Function receiving scroll offset in Y.
     */
    void add_mouse_verticel_wheel_callback(const std::function<void(float)>& callback);

    /**
     * @brief Locks the mouse pointer to the window.
     */
    void request_pointer_lock();

    /**
     * @brief Hides the mouse pointer.
     */
    void request_pointer_hide();

    /**
     * @brief Unlocks the mouse pointer from the window.
     */
    void request_pointer_unlock();

    /**
     * @brief Shows the mouse pointer.
     */
    void request_pointer_show();
}