# color.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Color utilities and drawing state for RGBA colors.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### struct `color`

**Description:**
```
RGBA color representation.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `}` | `struct { int red, green, blue, alpha;` | Color channels |
| `data` | `std::array<int, 4>` | Raw channel data |

**Syntax:**
```cpp
struct color {
  struct { int red, green, blue, alpha; };
  std::array<int, 4> data;
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### red

**Description:**
```
Get red component of color.
```

**Syntax:**
```cpp
int red(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### green

**Description:**
```
Get green component of color.
```

**Syntax:**
```cpp
int green(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### blue

**Description:**
```
Get blue component of color.
```

**Syntax:**
```cpp
int blue(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### alpha

**Description:**
```
Get alpha component of color.
```

**Syntax:**
```cpp
int alpha(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### background

**Description:**
```
Set background color (grayscale).
```

**Syntax:**
```cpp
void background(int grayscale);
```

<hr style="border-top:1px solid #eee" />

#### background

**Description:**
```
Set background color (grayscale + alpha).
```

**Syntax:**
```cpp
void background(int grayscale, int alpha);
```

<hr style="border-top:1px solid #eee" />

#### background

**Description:**
```
Set background color (RGBA channels).
```

**Syntax:**
```cpp
void background(int r, int g, int b, int a);
```

<hr style="border-top:1px solid #eee" />

#### background

**Description:**
```
Set background color from color struct.
```

**Syntax:**
```cpp
void background(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### clear

**Description:**
```
Clear canvas with current background.
```

**Syntax:**
```cpp
void clear();
```

<hr style="border-top:1px solid #eee" />

#### clear

**Description:**
```
Clear canvas with grayscale background.
```

**Syntax:**
```cpp
void clear(int grayscale);
```

<hr style="border-top:1px solid #eee" />

#### clear

**Description:**
```
Clear canvas with grayscale and alpha.
```

**Syntax:**
```cpp
void clear(int grayscale, int alpha);
```

<hr style="border-top:1px solid #eee" />

#### clear

**Description:**
```
Clear canvas with RGBA background.
```

**Syntax:**
```cpp
void clear(int r, int g, int b, int a);
```

<hr style="border-top:1px solid #eee" />

#### clear

**Description:**
```
Clear canvas with color struct background.
```

**Syntax:**
```cpp
void clear(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### no_fill

**Description:**
```
Disable filling shapes.
```

**Syntax:**
```cpp
void no_fill();
```

<hr style="border-top:1px solid #eee" />

#### no_stroke

**Description:**
```
Disable drawing shape outlines.
```

**Syntax:**
```cpp
void no_stroke();
```

<hr style="border-top:1px solid #eee" />

#### fill

**Description:**
```
Enable filling with grayscale.
```

**Syntax:**
```cpp
void fill(int grayscale);
```

<hr style="border-top:1px solid #eee" />

#### fill

**Description:**
```
Enable filling with grayscale + alpha.
```

**Syntax:**
```cpp
void fill(int grayscale, int alpha);
```

<hr style="border-top:1px solid #eee" />

#### fill

**Description:**
```
Enable filling with RGBA.
```

**Syntax:**
```cpp
void fill(int r, int g, int b, int a);
```

<hr style="border-top:1px solid #eee" />

#### fill

**Description:**
```
Enable filling with color struct.
```

**Syntax:**
```cpp
void fill(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### stroke

**Description:**
```
Enable stroke with grayscale.
```

**Syntax:**
```cpp
void stroke(int grayscale);
```

<hr style="border-top:1px solid #eee" />

#### stroke

**Description:**
```
Enable stroke with grayscale + alpha.
```

**Syntax:**
```cpp
void stroke(int grayscale, int alpha);
```

<hr style="border-top:1px solid #eee" />

#### stroke

**Description:**
```
Enable stroke with RGBA.
```

**Syntax:**
```cpp
void stroke(int r, int g, int b, int a);
```

<hr style="border-top:1px solid #eee" />

#### stroke

**Description:**
```
Enable stroke with color struct.
```

**Syntax:**
```cpp
void stroke(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### stroke_weight

**Description:**
```
Set stroke width.
```

**Syntax:**
```cpp
void stroke_weight(float w);
```

<hr style="border-top:1px solid #eee" />

#### inner_stroke

**Description:**
```
Enable inner stroke with grayscale.
```

**Syntax:**
```cpp
void inner_stroke(int grayscale);
```

<hr style="border-top:1px solid #eee" />

#### inner_stroke

**Description:**
```
Enable inner stroke with grayscale + alpha.
```

**Syntax:**
```cpp
void inner_stroke(int grayscale, int alpha);
```

<hr style="border-top:1px solid #eee" />

#### inner_stroke

**Description:**
```
Enable inner stroke with RGBA.
```

**Syntax:**
```cpp
void inner_stroke(int r, int g, int b, int a);
```

<hr style="border-top:1px solid #eee" />

#### inner_stroke

**Description:**
```
Enable inner stroke with color struct.
```

**Syntax:**
```cpp
void inner_stroke(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### inner_stroke_weight

**Description:**
```
Set inner stroke width.
```

**Syntax:**
```cpp
void inner_stroke_weight(float w);
```

<hr style="border-top:1px solid #eee" />

#### lerp_color

**Description:**
```
Linearly interpolate between two colors.
```

**Parameters:**

- `c1`: Start color.
- `c2`: End color.
- `t`: Interpolation factor [0,1].

**Returns:**
```
Interpolated color.
```

**Syntax:**
```cpp
color lerp_color(const color& c1, const color& c2, float t);
```

<hr style="border-top:1px solid #eee" />
