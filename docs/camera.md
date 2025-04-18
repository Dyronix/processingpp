# camera.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Create and control scene cameras (perspective, orthographic, font).</p>

<hr style="border-top:5px solid #aaa" />

## `ppp::camera::tags`

### Functions
<hr style="border-top:3px solid #ccc" />

#### perspective

**Description:**
```
Retrieve the identifier for the perspective camera.
```

**Returns:**
```
String tag for perspective camera.
```

**Syntax:**
```cpp
std::string_view perspective();
```

<hr style="border-top:1px solid #eee" />

#### orthographic

**Description:**
```
Retrieve the identifier for the orthographic camera.
```

**Returns:**
```
String tag for orthographic camera.
```

**Syntax:**
```cpp
std::string_view orthographic();
```

<hr style="border-top:1px solid #eee" />

#### font

**Description:**
```
Retrieve the identifier for the font-rendering camera.
```

**Returns:**
```
String tag for font camera.
```

**Syntax:**
```cpp
std::string_view font();
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### struct `scene_camera`

**Description:**
```
Represents a scene camera with position, orientation, and target.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `eye` | `vec3` | Camera position in world space. |
| `center` | `vec3` | Target point the camera looks at. |
| `up` | `vec3` | Up vector defining camera orientation. |
| `radius` | `float` | Distance from eye to center. |
| `azimuth` | `float` | Horizontal rotation angle around target. |
| `polar` | `float` | Vertical rotation angle around target. |

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `scene_camera()` | `scene_camera()` | Default constructor initializing common defaults. |
| `set_position` | `void set_position(float x, float y, float z)` | Set the camera's eye position. |
| `set_center` | `void set_center(float cx, float cy, float cz)` | Set the camera's target point. |
| `set_up_direction` | `void set_up_direction(float ux, float uy, float uz)` | Set the camera's up direction. |

**Syntax:**
```cpp
struct scene_camera {
  scene_camera();
  vec3 eye;
  vec3 center;
  vec3 up;
  float radius;
  float azimuth;
  float polar;
  void set_position(float x, float y, float z);
  void set_center(float cx, float cy, float cz);
  void set_up_direction(float ux, float uy, float uz);
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### struct `orbit_control_options`

**Description:**
```
Options for interactive orbit control.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `zoom_sensitivity` | `float` | Mouse zoom sensitivity. |
| `rotation_sensitivity` | `float` | Mouse rotation sensitivity. |
| `panning_sensitivity` | `float` | Mouse panning sensitivity. |
| `min_zoom` | `float` | Minimum allowed zoom distance. |
| `max_zoom` | `float` | Maximum allowed zoom distance. |

**Syntax:**
```cpp
struct orbit_control_options {
  float zoom_sensitivity = 0.0f;
  float rotation_sensitivity = 0.0f;
  float panning_sensitivity = 0.0f;
  float min_zoom = 1.0f;
  float max_zoom = 100.0f;
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### active_camera_position

**Description:**
```
Get the active camera's eye position.
```

**Returns:**
```
vec3 Eye position of active camera.
```

**Syntax:**
```cpp
vec3 active_camera_position();
```

<hr style="border-top:1px solid #eee" />

#### active_camera_target

**Description:**
```
Get the active camera's target point.
```

**Returns:**
```
vec3 Look-at center of active camera.
```

**Syntax:**
```cpp
vec3 active_camera_target();
```

<hr style="border-top:1px solid #eee" />

#### active_camera_up

**Description:**
```
Get the active camera's up vector.
```

**Returns:**
```
vec3 Up direction of active camera.
```

**Syntax:**
```cpp
vec3 active_camera_up();
```

<hr style="border-top:1px solid #eee" />

#### create_camera

**Description:**
```
Create and activate a scene camera with optional tag.
```

**Parameters:**

- `tag`: Optional identifier for the new camera.

**Returns:**
```
Initialized scene_camera object.
```

**Syntax:**
```cpp
scene_camera create_camera(std::string_view tag = {});
```

<hr style="border-top:1px solid #eee" />

#### set_scene_camera

**Description:**
```
Set the active camera using a scene_camera object.
```

**Parameters:**

- `c`: Scene camera instance.
- `tag`: Optional identifier for the camera. 

**Syntax:**
```cpp
void set_scene_camera(scene_camera c, std::string_view tag = {});
```

<hr style="border-top:1px solid #eee" />

#### set_scene_camera

**Description:**
```
Set the active camera by specifying parameters directly.
```

**Parameters:**

- `ex`: Eye X-coordinate.
- `ey`: Eye Y-coordinate.
- `ez`: Eye Z-coordinate.
- `cx`: Center X-coordinate.
- `cy`: Center Y-coordinate.
- `cz`: Center Z-coordinate.
- `ux`: Up X-component.
- `uy`: Up Y-component.
- `uz`: Up Z-component.
- `tag`: Optional identifier for the camera. 

**Syntax:**
```cpp
void set_scene_camera( float ex, float ey, float ez = 800.0f, float cx = 0.0f, float cy = 0.0f, float cz = 0.0f, float ux = 0.0f, float uy = 1.0f, float uz = 0.0f, std::string_view tag = {});
```

<hr style="border-top:1px solid #eee" />

#### activate_camera

**Description:**
```
Activate a previously created camera by its tag.
```

**Parameters:**

- `tag`: Identifier of the camera to activate. 

**Syntax:**
```cpp
void activate_camera(std::string_view tag);
```

<hr style="border-top:1px solid #eee" />

#### ortho

**Description:**
```
Configure and activate an orthographic projection.
```

**Parameters:**

- `left`: Left plane of orthographic frustum.
- `right`: Right plane of orthographic frustum.
- `bottom`: Bottom plane of orthographic frustum.
- `top`: Top plane of orthographic frustum.
- `near`: Near clipping plane.
- `far`: Far clipping plane.
- `tag`: Optional camera identifier. 

**Syntax:**
```cpp
void ortho( float left, float right, float bottom, float top, float near, float far, std::string_view tag = {});
```

<hr style="border-top:1px solid #eee" />

#### perspective

**Description:**
```
Configure and activate a perspective projection.
```

**Parameters:**

- `fovy`: Vertical field of view in degrees.
- `aspect`: Aspect ratio (width/height).
- `near`: Near clipping plane distance.
- `far`: Far clipping plane distance.
- `tag`: Optional camera identifier. 

**Syntax:**
```cpp
void perspective( float fovy, float aspect, float near, float far, std::string_view tag = {});
```

<hr style="border-top:1px solid #eee" />

#### orbit_control

**Description:**
```
Enable interactive orbit controls for the camera.
```

**Parameters:**

- `opts`: Orbit control sensitivity and zoom limits.
- `tag`: Identifier for the camera to control. 

**Syntax:**
```cpp
void orbit_control(orbit_control_options opts, std::string_view tag = {});
```

<hr style="border-top:1px solid #eee" />
