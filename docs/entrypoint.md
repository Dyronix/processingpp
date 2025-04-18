# entrypoint.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Entrypoint of the sketch</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### struct `app_params`

**Description:**
```
Holds parameters for application initialization.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `window_width` | `int` | Window width in pixels (default: 1280). |
| `window_height` | `int` | Window height in pixels (default: 720). |

**Syntax:**
```cpp
struct app_params {
  int window_width = 1280;
  int window_height = 720;
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### has_argument

**Description:**
```
Determines whether a specific command-line argument is present.
```

**Parameters:**

- `argc`: Argument count.
- `argv`: Argument values.
- `target`: The argument to look for.

**Returns:**
```
True if the argument is found, false otherwise.
```

**Syntax:**
```cpp
bool has_argument(int argc, char** argv, const char* target);
```

<hr style="border-top:1px solid #eee" />

#### find_argument

**Description:**
```
Finds the index of a specific command-line argument.
```

**Parameters:**

- `argc`: Argument count.
- `argv`: Argument values.
- `target`: The argument to find.

**Returns:**
```
Index of the argument if found, or -1 if not found.
```

**Syntax:**
```cpp
int find_argument(int argc, char** argv, const char* target);
```

<hr style="border-top:1px solid #eee" />

#### find_argument_with_value

**Description:**
```
Finds the index of a specific command-line argument and retrieves its value.
```

**Parameters:**

- `argc`: Argument count.
- `argv`: Argument values.
- `target`: The argument to find.
- `value`: Out parameter that receives the argument's value.

**Returns:**
```
Index of the argument if found (value set), or -1 if not found.
```

**Syntax:**
```cpp
int find_argument_with_value(int argc, char** argv, const char* target, const char** value);
```

<hr style="border-top:1px solid #eee" />

#### entry

**Description:**
```
Entry point for application initialization.
```

**Parameters:**

- `argc`: Argument count.
- `argv`: Argument values.

**Returns:**
```
Configured application parameters.
```

**Syntax:**
```cpp
extern app_params entry(int argc, char** argv);
```

<hr style="border-top:1px solid #eee" />

#### setup

**Description:**
```
User-defined setup function, called once after initialization.
```

**Syntax:**
```cpp
extern void setup();
```

<hr style="border-top:1px solid #eee" />

#### draw

**Description:**
```
User-defined draw function, called each frame.
```

**Syntax:**
```cpp
extern void draw();
```

<hr style="border-top:1px solid #eee" />
