# trigonometry.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Trigonometric functions and angle utilities.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Types

<hr style="border-top:3px solid #ccc" />

#### enum class `angle_mode_type`

**Description:**
```
Units used for angular measurements.
```

**Syntax:**
```cpp
enum class angle_mode_type {
  DEGREES,
  RADIANS,
};
```

<hr style="border-top:1px solid #eee" />

### Functions
<hr style="border-top:3px solid #ccc" />

#### angle_mode

**Description:**
```
Sets the unit system for angle measurements.
```

**Parameters:**

- `mode`: Desired angle mode (DEGREES or RADIANS). 

**Syntax:**
```cpp
void angle_mode(angle_mode_type mode);
```

<hr style="border-top:1px solid #eee" />

#### angle_mode

**Description:**
```
Retrieves the current angle measurement mode.
```

**Returns:**
```
Current angle mode.
```

**Syntax:**
```cpp
angle_mode_type angle_mode();
```

<hr style="border-top:1px solid #eee" />
