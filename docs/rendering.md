# rendering.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Configure the canvas and render draw modes.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Functions
<hr style="border-top:3px solid #ccc" />

#### create_canvas

**Description:**
```
Enable the scissor test and define the clipping rectangle.
```

**Parameters:**

- `x`: X-coordinate of the rectangle origin.
- `y`: Y-coordinate of the rectangle origin.
- `w`: Width of the clipping rectangle.
- `h`: Height of the clipping rectangle. 

**Syntax:**
```cpp
void create_canvas(float x, float y, float w, float h);
```

<hr style="border-top:1px solid #eee" />

#### resize_canvas

**Description:**
```
Update the existing scissor test rectangle.
```

**Parameters:**

- `x`: New X-coordinate of the rectangle origin.
- `y`: New Y-coordinate of the rectangle origin.
- `w`: New width of the clipping rectangle.
- `h`: New height of the clipping rectangle. 

**Syntax:**
```cpp
void resize_canvas(float x, float y, float w, float h);
```

<hr style="border-top:1px solid #eee" />

#### no_canvas

**Description:**
```
Disable the scissor test and remove clipping.
```

**Syntax:**
```cpp
void no_canvas();
```

<hr style="border-top:1px solid #eee" />

#### wireframe_linewidth

**Description:**
```
Set the line width for subsequent wireframe rendering.
```

**Parameters:**

- `line_width`: Thickness of the wireframe lines. 

**Syntax:**
```cpp
void wireframe_linewidth(float line_width);
```

<hr style="border-top:1px solid #eee" />

#### wireframe_color

**Description:**
```
Set the RGB color for subsequent wireframe rendering.
```

**Parameters:**

- `r`: Red component in [0.0, 1.0].
- `g`: Green component in [0.0, 1.0].
- `b`: Blue component in [0.0, 1.0]. 

**Syntax:**
```cpp
void wireframe_color(float r, float g, float b);
```

<hr style="border-top:1px solid #eee" />

#### enable_instance_draw_mode

**Description:**
```
Switch to instanced draw mode for subsequent renders.
```

**Syntax:**
```cpp
void enable_instance_draw_mode();
```

<hr style="border-top:1px solid #eee" />

#### enable_batched_draw_mode

**Description:**
```
Switch to batched draw mode for subsequent renders.
```

**Syntax:**
```cpp
void enable_batched_draw_mode();
```

<hr style="border-top:1px solid #eee" />
