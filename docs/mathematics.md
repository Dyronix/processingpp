# mathematics.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Common mathematical functions and noise utilities.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp::math`

### Functions
<hr style="border-top:3px solid #ccc" />

#### noise

**Description:**
```
Generates Perlin noise at a given 1D coordinate.
```

**Parameters:**

- `x`: Input coordinate.

**Returns:**
```
Noise value.
```

**Syntax:**
```cpp
double noise(double x);
```

<hr style="border-top:1px solid #eee" />

#### noise

**Description:**
```
Generates Perlin noise at a given 2D coordinate.
```

**Parameters:**

- `x`: X-coordinate.
- `y`: Y-coordinate.

**Returns:**
```
Noise value.
```

**Syntax:**
```cpp
double noise(double x, double y);
```

<hr style="border-top:1px solid #eee" />

#### noise

**Description:**
```
Generates Perlin noise at a given 3D coordinate.
```

**Parameters:**

- `x`: X-coordinate.
- `y`: Y-coordinate.
- `z`: Z-coordinate.

**Returns:**
```
Noise value.
```

**Syntax:**
```cpp
double noise(double x, double y, double z);
```

<hr style="border-top:1px solid #eee" />

#### noise_detail

**Description:**
```
Sets the frequency and octaves for Perlin noise generation.
```

**Parameters:**

- `frequency`: Base frequency.
- `octaves`: Number of octaves. 

**Syntax:**
```cpp
void noise_detail(double frequency, int octaves);
```

<hr style="border-top:1px solid #eee" />

#### noise_seed

**Description:**
```
Seeds the Perlin noise generator.
```

**Parameters:**

- `seed`: Seed value. 

**Syntax:**
```cpp
void noise_seed(int seed);
```

<hr style="border-top:1px solid #eee" />
