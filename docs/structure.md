# structure.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Structural setting of the sketch</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Functions
<hr style="border-top:3px solid #ccc" />

#### on_draw_begin

**Description:**
```
Registers a callback invoked at the beginning of each draw cycle.
```

**Parameters:**

- `draw_begin`: Function to call before drawing begins. 

**Syntax:**
```cpp
void on_draw_begin(std::function<void()> draw_begin);
```

<hr style="border-top:1px solid #eee" />

#### on_draw_end

**Description:**
```
Registers a callback invoked at the end of each draw cycle.
```

**Parameters:**

- `draw_end`: Function to call after drawing ends. 

**Syntax:**
```cpp
void on_draw_end(std::function<void()> draw_end);
```

<hr style="border-top:1px solid #eee" />

#### redraw

**Description:**
```
Requests a redraw of the display.
```

**Syntax:**
```cpp
void redraw();
```

<hr style="border-top:1px solid #eee" />

#### loop

**Description:**
```
Starts the application loop, causing continuous updates and rendering.
```

**Syntax:**
```cpp
void loop();
```

<hr style="border-top:1px solid #eee" />

#### no_loop

**Description:**
```
Stops the continuous application loop.
```

**Syntax:**
```cpp
void no_loop();
```

<hr style="border-top:1px solid #eee" />

#### is_headless

**Description:**
```
Checks if the application is running in headless mode (no graphics).
```

**Returns:**
```
True if headless mode is enabled, false otherwise.
```

**Syntax:**
```cpp
bool is_headless();
```

<hr style="border-top:1px solid #eee" />

#### is_looping

**Description:**
```
Checks if the application loop is currently running.
```

**Returns:**
```
True if looping, false otherwise.
```

**Syntax:**
```cpp
bool is_looping();
```

<hr style="border-top:1px solid #eee" />

#### headless

**Description:**
```
Switches the application into headless mode (no graphical output).
```

**Syntax:**
```cpp
void headless();
```

<hr style="border-top:1px solid #eee" />

#### quit

**Description:**
```
Requests the application to quit.
```

**Syntax:**
```cpp
void quit();
```

<hr style="border-top:1px solid #eee" />
