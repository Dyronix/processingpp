# input_output.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">File loading utilities.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Functions
<hr style="border-top:3px solid #ccc" />

#### load_binary_file

**Description:**
```
Load the contents of a binary file into a byte vector.
```

**Parameters:**

- `file_path`: Path to the binary file.

**Returns:**
```
Vector of bytes representing file contents.
```

**Syntax:**
```cpp
std::vector<std::byte> load_binary_file(std::string_view file_path);
```

<hr style="border-top:1px solid #eee" />

#### load_text_file

**Description:**
```
Load the contents of a text file into a byte vector.
```

**Parameters:**

- `file_path`: Path to the text file.

**Returns:**
```
Vector of bytes representing file contents.
```

**Syntax:**
```cpp
std::vector<std::byte> load_text_file(std::string_view file_path);
```

<hr style="border-top:1px solid #eee" />
