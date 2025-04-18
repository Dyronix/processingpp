# material.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Material and shader management utilities for rendering.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp::material::tags::unlit`

### Functions
<hr style="border-top:3px solid #ccc" />

#### color

**Description:**
```
Get the shader tag for unlit color.
```

**Returns:**
```
String identifier of the unlit color shader.
```

**Syntax:**
```cpp
std::string_view color();
```

<hr style="border-top:1px solid #eee" />

#### texture

**Description:**
```
Get the shader tag for unlit texture.
```

**Returns:**
```
String identifier of the unlit texture shader.
```

**Syntax:**
```cpp
std::string_view texture();
```

<hr style="border-top:1px solid #eee" />

#### font

**Description:**
```
Get the shader tag for unlit font.
```

**Returns:**
```
String identifier of the unlit font shader.
```

**Syntax:**
```cpp
std::string_view font();
```

<hr style="border-top:1px solid #eee" />

#### normal

**Description:**
```
Get the shader tag for unlit normal visualization.
```

**Returns:**
```
String identifier of the unlit normal shader.
```

**Syntax:**
```cpp
std::string_view normal();
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:5px solid #aaa" />

## `ppp::material::tags::lit`

### Functions
<hr style="border-top:3px solid #ccc" />

#### color

**Description:**
```
Get the shader tag for lit color.
```

**Returns:**
```
String identifier of the lit color shader.
```

**Syntax:**
```cpp
std::string_view color();
```

<hr style="border-top:1px solid #eee" />

#### texture

**Description:**
```
Get the shader tag for lit texture.
```

**Returns:**
```
String identifier of the lit texture shader.
```

**Syntax:**
```cpp
std::string_view texture();
```

<hr style="border-top:1px solid #eee" />

#### specular

**Description:**
```
Get the shader tag for lit specular.
```

**Returns:**
```
String identifier of the lit specular shader.
```

**Syntax:**
```cpp
std::string_view specular();
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `shading_model`

**Description:**
```
Shading model to use for materials.
```

**Syntax:**
```cpp
enum class shading_model {
  LIT,
  UNLIT,
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### enum class `shading_blending`

**Description:**
```
Blending mode for materials.
```

**Syntax:**
```cpp
enum class shading_blending {
  OPAQUE,
  TRANSPARENT,
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### struct `shader_program`

**Description:**
```
Represents a shader program and its uniforms.
```

**Methods:**

| Method | Signature | Description |
|---|---|---|
| `shader_program()` | `shader_program()` | Default constructor initializing id to invalid. |
| `shader_program(shader_program_id in_id)` | `shader_program(shader_program_id in_id)` | Construct a shader_program from an existing id. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, bool value)` | Set a boolean uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, int value)` | Set an integer uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, float value)` | Set a float uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, const vec2& value)` | Set a vec2 uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, const vec3& value)` | Set a vec3 uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, const vec4& value)` | Set a vec4 uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, const mat2& value)` | Set a mat2 uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, const mat3& value)` | Set a mat3 uniform. |
| `set_uniform` | `void set_uniform(std::string_view uniform_name, const mat4& value)` | Set a mat4 uniform. |

**Syntax:**
```cpp
struct shader_program {
  shader_program();
  shader_program(shader_program_id in_id);
  void set_uniform(std::string_view uniform_name, bool value);
  void set_uniform(std::string_view uniform_name, int value);
  void set_uniform(std::string_view uniform_name, float value);
  void set_uniform(std::string_view uniform_name, const vec2& value);
  void set_uniform(std::string_view uniform_name, const vec3& value);
  void set_uniform(std::string_view uniform_name, const vec4& value);
  void set_uniform(std::string_view uniform_name, const mat2& value);
  void set_uniform(std::string_view uniform_name, const mat3& value);
  void set_uniform(std::string_view uniform_name, const mat4& value);
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### shader_program_id

**Description:**
```
Type alias for unique identifier for a shader program.
```

**Syntax:**
```cpp
using shader_program_id = unsigned int;
```

<hr style="border-top:1px solid #eee" />

#### texture

**Description:**
```
Bind an image texture to the current shader.
```

**Parameters:**

- `image_id`: Identifier of the image to bind. 

**Syntax:**
```cpp
void texture(unsigned int image_id);
```

<hr style="border-top:1px solid #eee" />

#### reset_textures

**Description:**
```
Reset all bound textures for the current shader.
```

**Syntax:**
```cpp
void reset_textures();
```

<hr style="border-top:1px solid #eee" />

#### shader

**Description:**
```
Activate a shader by its tag.
```

**Parameters:**

- `tag`: String tag of the shader to activate. 

**Syntax:**
```cpp
void shader(std::string_view tag);
```

<hr style="border-top:1px solid #eee" />

#### normal_material

**Description:**
```
Use the default normal unlit material.
```

**Returns:**
```
Shader program configured for unlit normal rendering.
```

**Syntax:**
```cpp
shader_program normal_material();
```

<hr style="border-top:1px solid #eee" />

#### specular_material

**Description:**
```
Use the default specular lit material.
```

**Returns:**
```
Shader program configured for lit specular rendering.
```

**Syntax:**
```cpp
shader_program specular_material();
```

<hr style="border-top:1px solid #eee" />

#### create_shader

**Description:**
```
Create a new shader program from source strings.
```

**Parameters:**

- `tag`: Unique tag for the shader.
- `vertex_source`: GLSL source code for the vertex shader.
- `fragment_source`: GLSL source code for the fragment shader.
- `shading_model`: Shading model to use (lit or unlit).
- `shading_blend`: Blending mode for transparency.

**Returns:**
```
Shader program representing the compiled shader.
```

**Syntax:**
```cpp
shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
```

<hr style="border-top:1px solid #eee" />

#### create_shader

**Description:**
```
Create a new shader program with geometry shader support.
```

**Parameters:**

- `tag`: Unique tag for the shader.
- `vertex_source`: GLSL source code for the vertex shader.
- `fragment_source`: GLSL source code for the fragment shader.
- `geometry_source`: GLSL source code for the geometry shader.
- `shading_model`: Shading model to use.
- `shading_blend`: Blending mode for transparency.

**Returns:**
```
Shader program representing the compiled shader.
```

**Syntax:**
```cpp
shader_program create_shader(std::string_view tag, std::string_view vertex_source, std::string_view fragment_source, std::string_view geometry_source, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
```

<hr style="border-top:1px solid #eee" />

#### load_shader

**Description:**
```
Load or compile a shader program from file paths.
```

**Parameters:**

- `tag`: Unique tag for the shader.
- `vertex_path`: Filesystem path to the vertex shader file.
- `fragment_path`: Filesystem path to the fragment shader file.
- `shading_model`: Shading model to use.
- `shading_blend`: Blending mode.

**Returns:**
```
Shader program representing the loaded shader.
```

**Syntax:**
```cpp
shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
```

<hr style="border-top:1px solid #eee" />

#### load_shader

**Description:**
```
Load or compile a shader program with geometry shader support.
```

**Parameters:**

- `tag`: Unique tag for the shader.
- `vertex_path`: Filesystem path to the vertex shader file.
- `fragment_path`: Filesystem path to the fragment shader file.
- `geometry_path`: Filesystem path to the geometry shader file.
- `shading_model`: Shading model to use.
- `shading_blend`: Blending mode.

**Returns:**
```
Shader program representing the loaded shader.
```

**Syntax:**
```cpp
shader_program load_shader(std::string_view tag, std::string_view vertex_path, std::string_view fragment_path, std::string_view geometry_path, shading_model shading_model = shading_model::UNLIT, shading_blending shading_blend = shading_blending::OPAQUE);
```

<hr style="border-top:1px solid #eee" />

#### get_shader

**Description:**
```
Retrieve a shader program by its tag.
```

**Parameters:**

- `tag`: String tag of the shader.

**Returns:**
```
Shader program if found; otherwise invalid.
```

**Syntax:**
```cpp
shader_program get_shader(std::string_view tag);
```

<hr style="border-top:1px solid #eee" />
