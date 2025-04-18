# json.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">JSON utilities using nlohmann::json.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `binary_json_type`

**Description:**
```
Supported binary JSON formats for serialization/deserialization.
```

**Syntax:**
```cpp
enum class binary_json_type {
  MESSAGE_PACK,
  CBOR,
  UBJSON,
  BJSON,
  BJDATA,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### load_text_json

**Description:**
```
Load JSON data from a text file.
```

**Parameters:**

- `path`: Filesystem path to the JSON text file.

**Returns:**
```
Parsed json object.
```

**Syntax:**
```cpp
json load_text_json(std::string_view path);
```

<hr style="border-top:1px solid #eee" />

#### load_binary_json

**Description:**
```
Load JSON data from a binary file of specified format.
```

**Parameters:**

- `path`: Filesystem path to the binary JSON file.
- `binary_type`: Format of the binary JSON (default: BJSON).

**Returns:**
```
Parsed json object.
```

**Syntax:**
```cpp
json load_binary_json(std::string_view path, binary_json_type binary_type = binary_json_type::BJSON);
```

<hr style="border-top:1px solid #eee" />

#### save_json

**Description:**
```
Save a json object to a text file with pretty printing.
```

**Parameters:**

- `path`: Filesystem path where the JSON will be written.
- `obj`: JSON object to serialize and save. 

**Syntax:**
```cpp
void save_json(std::string_view path, json obj);
```

<hr style="border-top:1px solid #eee" />
