# model.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Model loading and drawing utilities for in-memory and file-based models.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `model_file_type`

**Description:**
```
Supported model file types.
```

**Syntax:**
```cpp
enum class model_file_type {
  OBJ,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### model_id

**Description:**
```
Type alias for unique identifier for a model.
```

**Syntax:**
```cpp
using model_id = unsigned long long;
```

<hr style="border-top:1px solid #eee" />

#### create_model

**Description:**
```
Create a model from in-memory model data.
```

**Parameters:**

- `model_string`: String containing the model file contents.
- `file_type`: Type of the model file (e.g., OBJ).

**Returns:**
```
Identifier of the newly created model.
```

**Syntax:**
```cpp
model_id create_model(std::string_view model_string, model_file_type file_type);
```

<hr style="border-top:1px solid #eee" />

#### load_model

**Description:**
```
Load a model from a file on disk.
```

**Parameters:**

- `model_path`: Filesystem path to the model file.

**Returns:**
```
Identifier of the loaded model.
```

**Syntax:**
```cpp
model_id load_model(std::string_view model_path);
```

<hr style="border-top:1px solid #eee" />

#### draw

**Description:**
```
Render a loaded model.
```

**Parameters:**

- `model_id`: Identifier of the model to render. 

**Syntax:**
```cpp
void draw(model_id model_id);
```

<hr style="border-top:1px solid #eee" />
