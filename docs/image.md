# image.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Create, load, manipulate, and draw images.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### struct `image`

**Description:**
```
Describe an image resource.
```

**Fields:**

| Member | Type | Description |
|---|---|---|
| `id` | `image_id` | Identifier of the image. |
| `width` | `int` | Width in pixels. |
| `height` | `int` | Height in pixels. |
| `channels` | `int` | Number of color channels. |

**Syntax:**
```cpp
struct image {
  image_id id;
  int width;
  int height;
  int channels;
};
```

<hr style="border-top:1px solid #eee" />

<hr style="border-top:3px solid #ccc" />

#### enum class `image_mode_type`

**Description:**
```
Positioning modes for images.
```

**Syntax:**
```cpp
enum class image_mode_type {
  CENTER,
  CORNER,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### image_id

**Description:**
```
Type alias for unique image identifier.
```

**Syntax:**
```cpp
using image_id = unsigned int;
```

<hr style="border-top:1px solid #eee" />

#### pixels_u8

**Description:**
```
Type alias for 8-bit pixel type.
```

**Syntax:**
```cpp
using pixels_u8 = unsigned char;
```

<hr style="border-top:1px solid #eee" />

#### pixels_s32

**Description:**
```
Type alias for 32-bit pixel type.
```

**Syntax:**
```cpp
using pixels_s32 = unsigned int;
```

<hr style="border-top:1px solid #eee" />

#### pixels_u8_ptr

**Description:**
```
Type alias for pointer to 8-bit pixel array.
```

**Syntax:**
```cpp
using pixels_u8_ptr = pixels_u8*;
```

<hr style="border-top:1px solid #eee" />

#### pixels_s32_ptr

**Description:**
```
Type alias for pointer to 32-bit pixel array.
```

**Syntax:**
```cpp
using pixels_s32_ptr = pixels_s32*;
```

<hr style="border-top:1px solid #eee" />

#### create

**Description:**
```
Create a blank or data-backed image.
```

**Parameters:**

- `w`: Width of image.
- `h`: Height of image.
- `c`: Number of channels.
- `data`: Optional initial pixel data.

**Returns:**
```
Descriptor for the new image.
```

**Syntax:**
```cpp
image create(float w, float h, int c, pixels_u8_ptr data);
```

<hr style="border-top:1px solid #eee" />

#### rotate_clockwise

**Description:**
```
Rotate pixels clockwise by 90 degrees increments.
```

**Parameters:**

- `a`: Pointer to source pixel array.
- `w`: Width of original image.
- `h`: Height of original image.
- `n`: Number of 90 degrees rotations.

**Returns:**
```
Rotated pixel vector.
```

**Syntax:**
```cpp
std::vector<pixels_s32> rotate_clockwise(const pixels_s32_ptr a, int w, int h, int n);
```

<hr style="border-top:1px solid #eee" />

#### rotate_counterclockwise

**Description:**
```
Rotate pixels counter-clockwise by 90 degrees increments.
```

**Syntax:**
```cpp
std::vector<pixels_s32> rotate_counterclockwise(const pixels_s32_ptr a, int w, int h, int n);
```

<hr style="border-top:1px solid #eee" />

#### image_mode

**Description:**
```
Set image drawing mode (CENTER or CORNER).
```

**Parameters:**

- `m`: Drawing mode. 

**Syntax:**
```cpp
void image_mode(image_mode_type m);
```

<hr style="border-top:1px solid #eee" />

#### load

**Description:**
```
Load an image file into memory.
```

**Parameters:**

- `path`: Filesystem path to the image.

**Returns:**
```
Descriptor for the loaded image.
```

**Syntax:**
```cpp
image load(std::string_view path);
```

<hr style="border-top:1px solid #eee" />

#### draw

**Description:**
```
Draw a loaded image to the canvas.
```

**Syntax:**
```cpp
void draw(image_id id, float x, float y, float w, float h);
```

<hr style="border-top:1px solid #eee" />

#### no_tint

**Description:**
```
Disable any active tint.
```

**Syntax:**
```cpp
void no_tint();
```

<hr style="border-top:1px solid #eee" />

#### tint

**Description:**
```
Apply a grayscale tint.
```

**Syntax:**
```cpp
void tint(int g);
```

<hr style="border-top:1px solid #eee" />

#### tint

**Description:**
```
Apply a grayscale tint with alpha.
```

**Syntax:**
```cpp
void tint(int g, int a);
```

<hr style="border-top:1px solid #eee" />

#### tint

**Description:**
```
Apply RGBA tint color.
```

**Syntax:**
```cpp
void tint(int r, int g, int b, int a);
```

<hr style="border-top:1px solid #eee" />

#### tint

**Description:**
```
Apply tint using a color struct.
```

**Syntax:**
```cpp
void tint(const color& c);
```

<hr style="border-top:1px solid #eee" />

#### load_pixels

**Description:**
```
Read pixels from the canvas into an array.
```

**Syntax:**
```cpp
unsigned char* load_pixels(int x, int y, int w, int h);
```

<hr style="border-top:1px solid #eee" />

#### load_pixels

**Description:**
```
Read pixels from a loaded image.
```

**Syntax:**
```cpp
unsigned char* load_pixels(image_id id);
```

<hr style="border-top:1px solid #eee" />

#### update_pixels

**Description:**
```
Update a loaded image's pixel data from the canvas.
```

**Syntax:**
```cpp
void update_pixels(image_id id);
```

<hr style="border-top:1px solid #eee" />

#### save_pixels

**Description:**
```
Save raw pixel data to a PNG file.
```

**Syntax:**
```cpp
void save_pixels(std::string_view name, unsigned char* data, int w, int h, int c);
```

<hr style="border-top:1px solid #eee" />

#### save_pixels

**Description:**
```
Save the current canvas pixels to a PNG file.
```

**Syntax:**
```cpp
void save_pixels(std::string_view name, int w, int h);
```

<hr style="border-top:1px solid #eee" />

#### save_pixels

**Description:**
```
Save a loaded image's pixels to a PNG file.
```

**Syntax:**
```cpp
void save_pixels(std::string_view name, image_id id);
```

<hr style="border-top:1px solid #eee" />

#### pixels_as_u8

**Description:**
```
Read raw canvas pixels (RGBA).
```

**Returns:**
```
Pointer to pixel data.
```

**Syntax:**
```cpp
pixels_u8_ptr pixels_as_u8();
```

<hr style="border-top:1px solid #eee" />

#### pixels_as_u32

**Description:**
```
Read raw canvas pixels as 32-bit values.
```

**Returns:**
```
Pointer to pixel data.
```

**Syntax:**
```cpp
pixels_s32_ptr pixels_as_u32();
```

<hr style="border-top:1px solid #eee" />
