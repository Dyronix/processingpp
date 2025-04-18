# typography.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Load fonts and render text to the canvas.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `text_mode_type`

**Description:**
```
Text positioning modes.
```

**Syntax:**
```cpp
enum class text_mode_type {
  CENTER,
  CORNER,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### @brief

**Description:**
```
Type alias for unique identifier for fonts.
```

**Syntax:**
```cpp
* @brief Text positioning modes. */ enum class text_mode_type : std::uint8_t { CENTER, CORNER };
```

<hr style="border-top:1px solid #eee" />

#### text_size

**Description:**
```
Set the font size for subsequent text operations.
```

**Parameters:**

- `size`: Font size in pixels. 

**Syntax:**
```cpp
void text_size(unsigned int size);
```

<hr style="border-top:1px solid #eee" />

#### text_mode

**Description:**
```
Set the text positioning mode (CENTER or CORNER).
```

**Parameters:**

- `mode`: Text mode. 

**Syntax:**
```cpp
void text_mode(text_mode_type mode);
```

<hr style="border-top:1px solid #eee" />

#### text

**Description:**
```
Render a string at the specified coordinates.
```

**Parameters:**

- `str`: String to draw.
- `x`: X-coordinate for text placement.
- `y`: Y-coordinate for text placement. 

**Syntax:**
```cpp
void text(std::string_view str, float x, float y);
```

<hr style="border-top:1px solid #eee" />

#### text_font

**Description:**
```
Set the active font for text rendering.
```

**Parameters:**

- `font`: Identifier of the loaded font. 

**Syntax:**
```cpp
void text_font(const font_id& font);
```

<hr style="border-top:1px solid #eee" />

#### load_font

**Description:**
```
Load a font and generate its glyph atlas.
```

**Parameters:**

- `path`: Filesystem path to font file.
- `size`: Pixel size of glyphs (default: 16).
- `count`: Number of characters to include (default: 128).

**Returns:**
```
Identifier for the newly loaded font.
```

**Syntax:**
```cpp
font_id load_font(std::string_view path, unsigned int size = 16, unsigned int count = 128);
```

<hr style="border-top:1px solid #eee" />
