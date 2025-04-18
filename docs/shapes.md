# shapes.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Draw 2D and 3D primitive shapes.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `shape_mode_type`

**Description:**
```
Positioning modes for 2D shapes.
```

**Syntax:**
```cpp
enum class shape_mode_type {
  CENTER,
  CORNER,
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### enum class `normal_mode_type`

**Description:**
```
Normal vector generation modes for 3D shapes.
```

**Syntax:**
```cpp
enum class normal_mode_type {
  FLAT,
  SMOOTH,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### enable_shadows

**Description:**
```
Enable shadow casting for subsequent shapes.
```

**Syntax:**
```cpp
void enable_shadows();
```

<hr style="border-top:1px solid #eee" />

#### disable_shadows

**Description:**
```
Disable shadow casting for subsequent shapes.
```

**Syntax:**
```cpp
void disable_shadows();
```

<hr style="border-top:1px solid #eee" />

#### rect_mode

**Description:**
```
Set rectangle and square coordinate mode.
```

**Parameters:**

- `mode`: CENTER or CORNER. 

**Syntax:**
```cpp
void rect_mode(shape_mode_type mode);
```

<hr style="border-top:1px solid #eee" />

#### ellipse_mode

**Description:**
```
Set ellipse and circle coordinate mode.
```

**Parameters:**

- `mode`: CENTER or CORNER. 

**Syntax:**
```cpp
void ellipse_mode(shape_mode_type mode);
```

<hr style="border-top:1px solid #eee" />

#### triangle_mode

**Description:**
```
Set triangle coordinate mode.
```

**Parameters:**

- `mode`: CENTER or CORNER. 

**Syntax:**
```cpp
void triangle_mode(shape_mode_type mode);
```

<hr style="border-top:1px solid #eee" />

#### normal_mode

**Description:**
```
Set normal generation mode for 3D shapes.
```

**Parameters:**

- `mode`: FLAT or SMOOTH. 

**Syntax:**
```cpp
void normal_mode(normal_mode_type mode);
```

<hr style="border-top:1px solid #eee" />

#### enable_wireframe_mode

**Description:**
```
Toggle wireframe overlay.
```

**Parameters:**

- `enable`: True to enable wireframe. 

**Syntax:**
```cpp
void enable_wireframe_mode(bool enable);
```

<hr style="border-top:1px solid #eee" />

#### enable_solid_mode

**Description:**
```
Toggle solid face rendering.
```

**Parameters:**

- `enable`: True to enable solid faces. 

**Syntax:**
```cpp
void enable_solid_mode(bool enable);
```

<hr style="border-top:1px solid #eee" />

#### circle

**Description:**
```
Draw a circle.
```

**Parameters:**

- `x`: Center X-coordinate.
- `y`: Center Y-coordinate.
- `r`: Radius.
- `detail`: Number of segments (default: 25).

**Returns:**
```
ID of generated geometry.
```

**Syntax:**
```cpp
geometry_id circle(float x, float y, float r, int detail = 25);
```

<hr style="border-top:1px solid #eee" />

#### ellipse

**Description:**
```
Draw an ellipse.
```

**Syntax:**
```cpp
geometry_id ellipse(float x, float y, float w, float h, int detail = 25);
```

<hr style="border-top:1px solid #eee" />

#### line

**Description:**
```
Draw a line between two points.
```

**Syntax:**
```cpp
geometry_id line(float x1, float y1, float x2, float y2);
```

<hr style="border-top:1px solid #eee" />

#### point

**Description:**
```
Draw a single 2D point.
```

**Syntax:**
```cpp
geometry_id point(float x, float y);
```

<hr style="border-top:1px solid #eee" />

#### rect

**Description:**
```
Draw a rectangle.
```

**Returns:**
```
ID of generated geometry.
```

**Syntax:**
```cpp
geometry_id rect(float x, float y, float w, float h);
```

<hr style="border-top:1px solid #eee" />

#### square

**Description:**
```
Draw a square.
```

**Syntax:**
```cpp
geometry_id square(float x, float y, float s);
```

<hr style="border-top:1px solid #eee" />

#### triangle

**Description:**
```
Draw a triangle.
```

**Syntax:**
```cpp
geometry_id triangle(float x1, float y1, float x2, float y2, float x3, float y3);
```

<hr style="border-top:1px solid #eee" />

#### polygon

**Description:**
```
Draw a four-sided polygon.
```

**Syntax:**
```cpp
geometry_id polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
```

<hr style="border-top:1px solid #eee" />

#### box

**Description:**
```
Draw a 3D box.
```

**Syntax:**
```cpp
geometry_id box(float width, float height, float depth);
```

<hr style="border-top:1px solid #eee" />

#### cone

**Description:**
```
Draw a 3D cone.
```

**Syntax:**
```cpp
geometry_id cone(float radius, float height, int detail = 24, bool cap = true);
```

<hr style="border-top:1px solid #eee" />

#### cylinder

**Description:**
```
Draw a 3D cylinder.
```

**Syntax:**
```cpp
geometry_id cylinder(float radius, float height, int detail = 24, bool bottom_cap = true, bool top_cap = true);
```

<hr style="border-top:1px solid #eee" />

#### plane

**Description:**
```
Draw a 3D plane.
```

**Syntax:**
```cpp
geometry_id plane(float width, float height);
```

<hr style="border-top:1px solid #eee" />

#### sphere

**Description:**
```
Draw a 3D sphere.
```

**Syntax:**
```cpp
geometry_id sphere(float radius, int detail = 24);
```

<hr style="border-top:1px solid #eee" />

#### torus

**Description:**
```
Draw a 3D torus.
```

**Syntax:**
```cpp
geometry_id torus(float radius, float tube_radius, int detailx = 24, int detaily = 16);
```

<hr style="border-top:1px solid #eee" />

#### point

**Description:**
```
Draw a single 3D point.
```

**Syntax:**
```cpp
geometry_id point(float x, float y, float z);
```

<hr style="border-top:1px solid #eee" />

#### tetrahedron

**Description:**
```
Draw a tetrahedron.
```

**Syntax:**
```cpp
geometry_id tetrahedron(float width, float height);
```

<hr style="border-top:1px solid #eee" />

#### octahedron

**Description:**
```
Draw an octahedron.
```

**Syntax:**
```cpp
geometry_id octahedron(float width, float height);
```

<hr style="border-top:1px solid #eee" />
