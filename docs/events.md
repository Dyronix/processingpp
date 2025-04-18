# events.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Input handling of the sketch</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `key_code`

**Description:**
```
Keyboard key codes enumeration.
```

**Syntax:**
```cpp
enum class key_code {
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
  KEY_MENU,
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### enum class `mouse_code`

**Description:**
```
Mouse button codes enumeration.
```

**Syntax:**
```cpp
enum class mouse_code {
  BUTTON_LEFT,
  BUTTON_MIDDLE,
  BUTTON_RIGHT,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### is_key_pressed

**Description:**
```
Checks if a key was pressed during the current frame.
```

**Parameters:**

- `code`: The key code to check.

**Returns:**
```
True if the key was pressed.
```

**Syntax:**
```cpp
bool is_key_pressed(key_code code);
```

<hr style="border-top:1px solid #eee" />

#### is_key_released

**Description:**
```
Checks if a key was released during the current frame.
```

**Parameters:**

- `code`: The key code to check.

**Returns:**
```
True if the key was released.
```

**Syntax:**
```cpp
bool is_key_released(key_code code);
```

<hr style="border-top:1px solid #eee" />

#### is_key_down

**Description:**
```
Checks if a key is currently held down.
```

**Parameters:**

- `code`: The key code to check.

**Returns:**
```
True if the key is down.
```

**Syntax:**
```cpp
bool is_key_down(key_code code);
```

<hr style="border-top:1px solid #eee" />

#### is_any_key_pressed

**Description:**
```
Checks if any key was pressed during the current frame.
```

**Returns:**
```
True if any key was pressed.
```

**Syntax:**
```cpp
bool is_any_key_pressed();
```

<hr style="border-top:1px solid #eee" />

#### is_any_key_released

**Description:**
```
Checks if any key was released during the current frame.
```

**Returns:**
```
True if any key was released.
```

**Syntax:**
```cpp
bool is_any_key_released();
```

<hr style="border-top:1px solid #eee" />

#### is_any_key_down

**Description:**
```
Checks if any key is currently held down.
```

**Returns:**
```
True if any key is down.
```

**Syntax:**
```cpp
bool is_any_key_down();
```

<hr style="border-top:1px solid #eee" />

#### key

**Description:**
```
Gets the last key code event.
```

**Returns:**
```
The last key code.
```

**Syntax:**
```cpp
key_code key();
```

<hr style="border-top:1px solid #eee" />

#### add_key_pressed_callback

**Description:**
```
Registers a callback for key pressed events.
```

**Parameters:**

- `callback`: Function receiving the pressed key code. 

**Syntax:**
```cpp
void add_key_pressed_callback(const std::function<void(key_code)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_key_released_callback

**Description:**
```
Registers a callback for key released events.
```

**Parameters:**

- `callback`: Function receiving the released key code. 

**Syntax:**
```cpp
void add_key_released_callback(const std::function<void(key_code)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_key_down_callback

**Description:**
```
Registers a callback for key down events.
```

**Parameters:**

- `callback`: Function receiving the key code while key is held. 

**Syntax:**
```cpp
void add_key_down_callback(const std::function<void(key_code)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### set_quit_application_keycode

**Description:**
```
Sets a specific key to trigger application quit when pressed.
```

**Parameters:**

- `code`: The key code that will quit the app on press. 

**Syntax:**
```cpp
void set_quit_application_keycode(key_code code);
```

<hr style="border-top:1px solid #eee" />

#### moved_x

**Description:**
```
Gets the mouse movement delta in the X axis since last frame.
```

**Returns:**
```
Delta X in pixels.
```

**Syntax:**
```cpp
float moved_x();
```

<hr style="border-top:1px solid #eee" />

#### moved_y

**Description:**
```
Gets the mouse movement delta in the Y axis since last frame.
```

**Returns:**
```
Delta Y in pixels.
```

**Syntax:**
```cpp
float moved_y();
```

<hr style="border-top:1px solid #eee" />

#### mouse_x

**Description:**
```
Gets the current mouse X position.
```

**Returns:**
```
X coordinate in pixels.
```

**Syntax:**
```cpp
float mouse_x();
```

<hr style="border-top:1px solid #eee" />

#### mouse_y

**Description:**
```
Gets the current mouse Y position.
```

**Returns:**
```
Y coordinate in pixels.
```

**Syntax:**
```cpp
float mouse_y();
```

<hr style="border-top:1px solid #eee" />

#### prev_mouse_x

**Description:**
```
Gets the previous mouse X position from last frame.
```

**Returns:**
```
Previous X coordinate in pixels.
```

**Syntax:**
```cpp
float prev_mouse_x();
```

<hr style="border-top:1px solid #eee" />

#### prev_mouse_y

**Description:**
```
Gets the previous mouse Y position from last frame.
```

**Returns:**
```
Previous Y coordinate in pixels.
```

**Syntax:**
```cpp
float prev_mouse_y();
```

<hr style="border-top:1px solid #eee" />

#### mouse_button

**Description:**
```
Gets the current mouse button code.
```

**Returns:**
```
Mouse button code.
```

**Syntax:**
```cpp
mouse_code mouse_button();
```

<hr style="border-top:1px solid #eee" />

#### scroll_offset_x

**Description:**
```
Gets horizontal scroll offset since last frame.
```

**Returns:**
```
Scroll offset in X.
```

**Syntax:**
```cpp
float scroll_offset_x();
```

<hr style="border-top:1px solid #eee" />

#### scroll_offset_y

**Description:**
```
Gets vertical scroll offset since last frame.
```

**Returns:**
```
Scroll offset in Y.
```

**Syntax:**
```cpp
float scroll_offset_y();
```

<hr style="border-top:1px solid #eee" />

#### is_left_button_pressed

**Description:**
```
Checks if the left mouse button is pressed.
```

**Returns:**
```
True if left button is pressed.
```

**Syntax:**
```cpp
bool is_left_button_pressed();
```

<hr style="border-top:1px solid #eee" />

#### is_right_button_pressed

**Description:**
```
Checks if the right mouse button is pressed.
```

**Returns:**
```
True if right button is pressed.
```

**Syntax:**
```cpp
bool is_right_button_pressed();
```

<hr style="border-top:1px solid #eee" />

#### is_middle_button_pressed

**Description:**
```
Checks if the middle mouse button is pressed.
```

**Returns:**
```
True if middle button is pressed.
```

**Syntax:**
```cpp
bool is_middle_button_pressed();
```

<hr style="border-top:1px solid #eee" />

#### is_any_mouse_button_pressed

**Description:**
```
Checks if any mouse button is pressed.
```

**Returns:**
```
True if any mouse button is pressed.
```

**Syntax:**
```cpp
bool is_any_mouse_button_pressed();
```

<hr style="border-top:1px solid #eee" />

#### add_mouse_moved_callback

**Description:**
```
Registers a callback for mouse movement events.
```

**Parameters:**

- `callback`: Function receiving X and Y deltas. 

**Syntax:**
```cpp
void add_mouse_moved_callback(const std::function<void(float, float)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_mouse_dragged_callback

**Description:**
```
Registers a callback for mouse drag events.
```

**Parameters:**

- `callback`: Function receiving X and Y deltas. 

**Syntax:**
```cpp
void add_mouse_dragged_callback(const std::function<void(float, float)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_mouse_pressed_callback

**Description:**
```
Registers a callback for mouse button press events.
```

**Parameters:**

- `callback`: Function receiving the pressed mouse code. 

**Syntax:**
```cpp
void add_mouse_pressed_callback(const std::function<void(mouse_code)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_mouse_released_callback

**Description:**
```
Registers a callback for mouse button release events.
```

**Parameters:**

- `callback`: Function receiving the released mouse code. 

**Syntax:**
```cpp
void add_mouse_released_callback(const std::function<void(mouse_code)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_mouse_horizontal_wheel_callback

**Description:**
```
Registers a callback for horizontal scroll wheel events.
```

**Parameters:**

- `callback`: Function receiving scroll offset in X. 

**Syntax:**
```cpp
void add_mouse_horizontal_wheel_callback(const std::function<void(float)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### add_mouse_verticel_wheel_callback

**Description:**
```
Registers a callback for vertical scroll wheel events.
```

**Parameters:**

- `callback`: Function receiving scroll offset in Y. 

**Syntax:**
```cpp
void add_mouse_verticel_wheel_callback(const std::function<void(float)>& callback);
```

<hr style="border-top:1px solid #eee" />

#### request_pointer_lock

**Description:**
```
Locks the mouse pointer to the window.
```

**Syntax:**
```cpp
void request_pointer_lock();
```

<hr style="border-top:1px solid #eee" />

#### request_pointer_hide

**Description:**
```
Hides the mouse pointer.
```

**Syntax:**
```cpp
void request_pointer_hide();
```

<hr style="border-top:1px solid #eee" />

#### request_pointer_unlock

**Description:**
```
Unlocks the mouse pointer from the window.
```

**Syntax:**
```cpp
void request_pointer_unlock();
```

<hr style="border-top:1px solid #eee" />

#### request_pointer_show

**Description:**
```
Shows the mouse pointer.
```

**Syntax:**
```cpp
void request_pointer_show();
```

<hr style="border-top:1px solid #eee" />
