# transform.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Coordinate system transformation stack and operations.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Functions
<hr style="border-top:3px solid #ccc" />

#### push

**Description:**
```
Pushes the current transformation matrix onto the stack.
```

**Syntax:**
```cpp
void push();
```

<hr style="border-top:1px solid #eee" />

#### pop

**Description:**
```
Pops the top transformation matrix from the stack.
```

**Syntax:**
```cpp
void pop();
```

<hr style="border-top:1px solid #eee" />

#### active_transform

**Description:**
```
Retrieves the active transformation matrix.
```

**Returns:**
```
Current world transform as a mat4.
```

**Syntax:**
```cpp
mat4 active_transform();
```

<hr style="border-top:1px solid #eee" />

#### rotate

**Description:**
```
Rotates the coordinate system by a given angle about the origin.
```

**Parameters:**

- `angle`: Rotation angle (interpreted according to angle_mode()). 

**Syntax:**
```cpp
void rotate(float angle);
```

<hr style="border-top:1px solid #eee" />

#### rotate

**Description:**
```
Rotates the coordinate system around an arbitrary axis.
```

**Parameters:**

- `axisx`: X component of rotation axis.
- `axisy`: Y component of rotation axis.
- `axisz`: Z component of rotation axis.
- `angle`: Rotation angle (interpreted according to angle_mode()). 

**Syntax:**
```cpp
void rotate(float axisx, float axisy, float axisz, float angle);
```

<hr style="border-top:1px solid #eee" />

#### rotate

**Description:**
```
Rotates the coordinate system around the specified axis vector.
```

**Parameters:**

- `axis`: Rotation axis as a vec3.
- `angle`: Rotation angle (interpreted according to angle_mode()). 

**Syntax:**
```cpp
void rotate(const vec3& axis, float angle);
```

<hr style="border-top:1px solid #eee" />

#### scale

**Description:**
```
Scales the coordinate system in X and Y.
```

**Parameters:**

- `x`: Scale factor in X.
- `y`: Scale factor in Y. 

**Syntax:**
```cpp
void scale(float x, float y);
```

<hr style="border-top:1px solid #eee" />

#### scale

**Description:**
```
Scales the coordinate system in X, Y, and Z.
```

**Parameters:**

- `x`: Scale factor in X.
- `y`: Scale factor in Y.
- `z`: Scale factor in Z. 

**Syntax:**
```cpp
void scale(float x, float y, float z);
```

<hr style="border-top:1px solid #eee" />

#### scale

**Description:**
```
Scales the coordinate system by a 2D vector.
```

**Parameters:**

- `scale`: Scale factors as a vec2. 

**Syntax:**
```cpp
void scale(const vec2& scale);
```

<hr style="border-top:1px solid #eee" />

#### scale

**Description:**
```
Scales the coordinate system by a 3D vector.
```

**Parameters:**

- `scale`: Scale factors as a vec3. 

**Syntax:**
```cpp
void scale(const vec3& scale);
```

<hr style="border-top:1px solid #eee" />

#### translate

**Description:**
```
Translates the coordinate system in X and Y.
```

**Parameters:**

- `x`: Translation in X.
- `y`: Translation in Y. 

**Syntax:**
```cpp
void translate(float x, float y);
```

<hr style="border-top:1px solid #eee" />

#### translate

**Description:**
```
Translates the coordinate system in X, Y, and Z.
```

**Parameters:**

- `x`: Translation in X.
- `y`: Translation in Y.
- `z`: Translation in Z. 

**Syntax:**
```cpp
void translate(float x, float y, float z);
```

<hr style="border-top:1px solid #eee" />

#### translate

**Description:**
```
Translates the coordinate system by a 2D vector.
```

**Parameters:**

- `trans`: Translation vector as a vec2. 

**Syntax:**
```cpp
void translate(const vec2& trans);
```

<hr style="border-top:1px solid #eee" />

#### translate

**Description:**
```
Translates the coordinate system by a 3D vector.
```

**Parameters:**

- `trans`: Translation vector as a vec3. 

**Syntax:**
```cpp
void translate(const vec3& trans);
```

<hr style="border-top:1px solid #eee" />
