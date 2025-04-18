# time_date.h

<style>
  .file-summary { font-size: 1.2em; font-style: italic; margin-bottom: 1em; }
  table { width: 100%; table-layout: fixed; border-collapse: collapse; }
  th, td { border: 1px solid #ddd; padding: 8px; word-wrap: break-word; }
  th { background-color: #f4f4f4; }
</style>

<p class="file-summary">Utilities for retrieving current system date/time.</p>

<hr style="border-top:5px solid #aaa" />

## `ppp`

### Functions
<hr style="border-top:3px solid #ccc" />

#### day

**Description:**
```
Get current day of month (1-31).
```

**Syntax:**
```cpp
int day();
```

<hr style="border-top:1px solid #eee" />

#### hour

**Description:**
```
Get current hour of day (0-23).
```

**Syntax:**
```cpp
int hour();
```

<hr style="border-top:1px solid #eee" />

#### minute

**Description:**
```
Get current minute of hour (0-59).
```

**Syntax:**
```cpp
int minute();
```

<hr style="border-top:1px solid #eee" />

#### second

**Description:**
```
Get current second of minute (0-59).
```

**Syntax:**
```cpp
int second();
```

<hr style="border-top:1px solid #eee" />

#### month

**Description:**
```
Get current month (1-12).
```

**Syntax:**
```cpp
int month();
```

<hr style="border-top:1px solid #eee" />

#### year

**Description:**
```
Get current year (e.g., 2025).
```

**Syntax:**
```cpp
int year();
```

<hr style="border-top:1px solid #eee" />

#### millis

**Description:**
```
Get milliseconds elapsed since program start.
```

**Syntax:**
```cpp
long long millis();
```

<hr style="border-top:1px solid #eee" />
