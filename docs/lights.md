# lights.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Light management utilities for directional and point lights within the scene.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `light_type`

**Description:**
```
Types of lights supported.
```

**Syntax:**
```cpp
enum class light_type {
  POINT,
  DIRECTIONAL,
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### struct `point_light_desc`

**Description:**
```
Parameters for creating a point light.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `position` | `vec3` | Position of the light in world space |
| `ambient` | `color` | Ambient color component |
| `diffuse` | `color` | Diffuse color component |
| `specular` | `color` | Specular color component |
| `specular_enabled` | `bool` | Enable specular highlights |
| `cast_shadows` | `bool` | Enable shadow casting |
| `max_range` | `float` | Maximum effective range |
| `falloff_start` | `float` | Distance where falloff begins |

**Syntax:**
```cpp
struct point_light_desc {
  vec3 position;
  color ambient;
  color diffuse;
  color specular;
  bool specular_enabled = false;
  bool cast_shadows = false;
  float max_range = 400.0f;
  float   falloff_start = 250.0f;
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### struct `directional_light_desc`

**Description:**
```
Parameters for creating a directional light.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `direction` | `vec3` | Direction vector of the light |
| `ambient` | `color` | Ambient color component |
| `diffuse` | `color` | Diffuse color component |
| `specular` | `color` | Specular color component |
| `specular_enabled` | `bool` | Enable specular highlights |
| `cast_shadows` | `bool` | Enable shadow casting |

**Syntax:**
```cpp
struct directional_light_desc {
  vec3 direction;
  color ambient;
  color diffuse;
  color specular;
  bool specular_enabled = false;
  bool cast_shadows = false;
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### no_lights

**Description:**
```
Remove all lights from the current scene.
```

**Syntax:**
```cpp
void no_lights();
```

<hr style="border-top:1px solid #eee" />

#### directional_light

**Description:**
```
Add a new directional light to the scene.
```

**Parameters:**

- `desc`: Descriptor struct containing light parameters.

**Returns:**
```
Identifier of the created directional light.
```

**Syntax:**
```cpp
light_id directional_light(const directional_light_desc& desc);
```

<hr style="border-top:1px solid #eee" />

#### point_light

**Description:**
```
Add a new point light to the scene.
```

**Parameters:**

- `desc`: Descriptor struct containing light parameters.

**Returns:**
```
Identifier of the created point light.
```

**Syntax:**
```cpp
light_id point_light(const point_light_desc& desc);
```

<hr style="border-top:1px solid #eee" />

#### light_position

**Description:**
```
Update the position of an existing light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light (POINT or DIRECTIONAL).
- `x`: X-coordinate in world space.
- `y`: Y-coordinate in world space.
- `z`: Z-coordinate in world space. 

**Syntax:**
```cpp
void light_position(light_id id, light_type type, float x, float y, float z);
```

<hr style="border-top:1px solid #eee" />

#### light_direction

**Description:**
```
Update the direction vector of an existing light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light.
- `x`: X-component of the direction vector.
- `y`: Y-component of the direction vector.
- `z`: Z-component of the direction vector. 

**Syntax:**
```cpp
void light_direction(light_id id, light_type type, float x, float y, float z);
```

<hr style="border-top:1px solid #eee" />

#### light_ambient

**Description:**
```
Set the ambient color of a light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light.
- `r`: Red channel (0.0 to 1.0).
- `g`: Green channel (0.0 to 1.0).
- `b`: Blue channel (0.0 to 1.0). 

**Syntax:**
```cpp
void light_ambient(light_id id, light_type type, float r, float g, float b);
```

<hr style="border-top:1px solid #eee" />

#### light_diffuse

**Description:**
```
Set the diffuse color of a light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light.
- `r`: Red channel (0.0 to 1.0).
- `g`: Green channel (0.0 to 1.0).
- `b`: Blue channel (0.0 to 1.0). 

**Syntax:**
```cpp
void light_diffuse(light_id id, light_type type, float r, float g, float b);
```

<hr style="border-top:1px solid #eee" />

#### light_specular

**Description:**
```
Set the specular color of a light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light.
- `r`: Red channel (0.0 to 1.0).
- `g`: Green channel (0.0 to 1.0).
- `b`: Blue channel (0.0 to 1.0). 

**Syntax:**
```cpp
void light_specular(light_id id, light_type type, float r, float g, float b);
```

<hr style="border-top:1px solid #eee" />

#### light_enable_specular

**Description:**
```
Enable or disable specular highlights on a light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light.
- `enable`: True to enable specular, false to disable. 

**Syntax:**
```cpp
void light_enable_specular(light_id id, light_type type, bool enable);
```

<hr style="border-top:1px solid #eee" />

#### light_enable_shadows

**Description:**
```
Enable or disable shadow casting for a light.
```

**Parameters:**

- `id`: Identifier of the light.
- `type`: Type of the light.
- `enable`: True to enable shadows, false to disable. 

**Syntax:**
```cpp
void light_enable_shadows(light_id id, light_type type, bool enable);
```

<hr style="border-top:1px solid #eee" />
