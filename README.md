# JPAXER 🟦 JSON Parser from Scratch in C

![C](https://img.shields.io/badge/Language-C-blue)
![License](https://img.shields.io/badge/License-Educational-green)
![Build](https://img.shields.io/badge/Build-Experimental-orange)

JPAXER is a **fast, memory-efficient JSON parser and manipulator library** written entirely in C from scratch.  
It was designed as a learning project to explore **C memory management, pointers, and JSON parsing** while handling complex JSON structures efficiently.

---

## 🌟 Features

- Parse **JSON objects, arrays, strings, numbers (integers and doubles), booleans, and null values**.
- **Create, read, update, and delete JSON nodes dynamically**.
- **Print JSON with color-coded syntax** for easier debugging.
- **Save JSON to files** with proper indentation.
- **Manual memory management** with custom `_malloc` and arena-style allocation.
- **Error tracking** with red-colored messages, line numbers, and pointer indicators.
- Support for **nested JSON objects, arrays, and multiple values per node**.
- Incremental parsing and **safe handling of large JSON structures**.
- Custom **key/value management** enabling fast lookup and updates.
- Lightweight, portable, and dependency-free C code.

---

## ⚙ Scope

- **Learning and experimentation** with C, pointers, and parsing logic.
- Handles typical JSON structures and syntax.
- Focused on **clarity and understanding** rather than production-grade robustness.
- Demonstrates **manual memory management** and **custom error handling**.

---

## ⚠ Limitations

- Some recoverable errors are skipped but logged.
- **Strings are safe**, but `int`, `double`, `bool` values must be heap-allocated using helper functions.
- No automatic garbage collection — call `free_memory()` to release all memory.
- Not optimized for extremely large JSON files (no streaming parser yet).
- Partial compliance with the JSON standard (e.g., limited Unicode escape, scientific notation parsing).

---

## 💾 Memory Management

- Uses **custom `_malloc` and arena allocator** for efficiency.
- Avoid passing stack-allocated variables for non-string types; use helpers:
  - `get_J_INT(long value)`
  - `get_J_DOUBLE(double value)`
  - `get_J_BOOL(bool value)`
- Always **free memory after usage** using `free_memory()`.
- Updates and deletes do not reclaim individual values; memory is released when the arena is reset.

---

## ❌ Error Handling

- Color-coded terminal messages (red for errors, syntax colors for JSON output).
- Line number and pointer position help locate issues in JSON input.
- Centralized error API:
  - `get_read_error()`
  - `get_object_error()`
  - `get_allocation_err()`
- Covers allocation failures, invalid keys, null values, read buffer issues, and more.

---

## 📝 API Reference

### Parsing JSON

| Function | Signature | Description |
|----------|-----------|-------------|
| Parse from file | `JSON_NODE* parse_JSON_from_FILE(FILE* json_file);` | Parse a JSON file and return the root node. |
| Parse from string | `JSON_NODE* parse_JSON_from_str(char* json_str);` | Parse a JSON string and return the root node. |

### Node Access & Manipulation

| Function | Signature | Description |
|----------|-----------|-------------|
| Get nested node | `JSON_NODE* get(JSON_NODE** root, size_t args_count, const char* key1, const char* key2, ...);` | Retrieve a nested node using a key chain. |
| Set value | `bool set(JSON_NODE** root, const char* key, void* new_data, JSON_DTYPE type);` | Update a node’s value. |
| Add node | `bool add(JSON_NODE** root, const char* key, void* new_data, JSON_DTYPE type);` | Add a new key-value pair. |
| Delete node | `bool del(JSON_NODE** root, const char* key);` | Remove a key-value pair. |
| Move up tree | `bool back(JSON_NODE** root, int steps);` | Navigate back in the tree by given steps. |

### Supported Data Types

| Type | Description |
|------|------------|
| `J_STRING` | String value |
| `J_OBJECT` | JSON object |
| `J_BOOL` | Boolean value |
| `J_NULL` | Null value |
| `J_DOUBLE` | Floating-point number |
| `J_INT` | Integer number |

### Utility Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| Save JSON | `void save_JSON_to_FILE(JSON_NODE* data, FILE* json_file);` | Write JSON to a file with proper formatting. |
| Print JSON | `void print_JSON_NODE(JSON_NODE* node);` | Print JSON with color-coded syntax. |

### Helper Functions for Creating Values

| Function | Signature | Description |
|----------|-----------|-------------|
| Integer | `void* get_J_INT(long int data);` | Allocate a JSON integer value. |
| Double | `void* get_J_DOUBLE(double data);` | Allocate a JSON double value. |
| Boolean | `void* get_J_BOOL(bool data);` | Allocate a JSON boolean value. |
| String | `void* get_J_STRING(char* data);` | Allocate a JSON string value. |

---

## 🚀 Usage Example

```c
#include <stdio.h>
#include "jpaxer.h"

int main() {
    FILE* file1 = fopen("test1.json", "r");
    JSON_NODE* json1 = parse_JSON_from_FILE(file1);
    fclose(file1);

    get(json1, 2, "configurations", "0");
    add(json1, "user_id", get_J_INT(123), J_INT);
    print_JSON_NODE(json1);

    FILE* file_out = fopen("output.json", "w");
    save_JSON_to_FILE(json1, file_out);
    fclose(file_out);

    free_memory();
    return 0;
}
