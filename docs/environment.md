# environment.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">State of the sketch</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Functions
<hr style="border-top:3px solid #ccc" />

#### print

**Description:**
```
Prints a message to the console.
```

**Parameters:**

- `message`: The message to print. 

**Syntax:**
```cpp
void print(std::string_view message);
```

<hr style="border-top:1px solid #eee" />

#### print

**Description:**
```
Prints a C-string message to the console.
```

**Parameters:**

- `message`: The C-string to print. 

**Syntax:**
```cpp
void print(const char* message);
```

<hr style="border-top:1px solid #eee" />

#### frame_rate

**Description:**
```
Sets the application's target frame rate.
```

**Parameters:**

- `frame_rate`: Frames per second to target. 

**Syntax:**
```cpp
void frame_rate(unsigned int frame_rate);
```

<hr style="border-top:1px solid #eee" />

#### average_frame_rate

**Description:**
```
Gets the average frame rate over recent frames.
```

**Returns:**
```
Average frames per second.
```

**Syntax:**
```cpp
unsigned int average_frame_rate();
```

<hr style="border-top:1px solid #eee" />

#### frame_rate

**Description:**
```
Gets the current frame rate.
```

**Returns:**
```
Current frames per second.
```

**Syntax:**
```cpp
unsigned int frame_rate();
```

<hr style="border-top:1px solid #eee" />

#### frame_count

**Description:**
```
Gets the number of frames rendered since start.
```

**Returns:**
```
Frame count.
```

**Syntax:**
```cpp
unsigned int frame_count();
```

<hr style="border-top:1px solid #eee" />

#### delta_time

**Description:**
```
Gets the time elapsed since the last frame.
```

**Returns:**
```
Delta time in seconds.
```

**Syntax:**
```cpp
float delta_time();
```

<hr style="border-top:1px solid #eee" />

#### total_time

**Description:**
```
Gets the total running time of the application.
```

**Returns:**
```
Total time in seconds.
```

**Syntax:**
```cpp
float total_time();
```

<hr style="border-top:1px solid #eee" />

#### window_width

**Description:**
```
Gets the width of the application window.
```

**Returns:**
```
Window width in pixels.
```

**Syntax:**
```cpp
float window_width();
```

<hr style="border-top:1px solid #eee" />

#### window_height

**Description:**
```
Gets the height of the application window.
```

**Returns:**
```
Window height in pixels.
```

**Syntax:**
```cpp
float window_height();
```

<hr style="border-top:1px solid #eee" />

#### canvas_width

**Description:**
```
Gets the width of the drawing canvas.
```

**Returns:**
```
Canvas width in pixels.
```

**Syntax:**
```cpp
float canvas_width();
```

<hr style="border-top:1px solid #eee" />

#### canvas_height

**Description:**
```
Gets the height of the drawing canvas.
```

**Returns:**
```
Canvas height in pixels.
```

**Syntax:**
```cpp
float canvas_height();
```

<hr style="border-top:1px solid #eee" />

#### cwd

**Description:**
```
Returns the current working directory.
```

**Returns:**
```
Path to the working directory as a string view.
```

**Syntax:**
```cpp
std::string_view cwd();
```

<hr style="border-top:1px solid #eee" />
