# JPAXER 🟦 JSON Parser from Scratch in C

![C](https://img.shields.io/badge/Language-C-blue)
![License](https://img.shields.io/badge/License-Educational-green)
![Build](https://img.shields.io/badge/Status-Experimental-orange)

---

## 🌟 Overview

JPAXER is a **lightweight, memory-efficient JSON parser and manipulator** written entirely in C from scratch.  
It provides a fully dynamic JSON object tree that allows **creation, reading, updating, and deletion of JSON nodes**.  

JPAXER is designed to demonstrate **manual memory management using arena allocation**, **error tracking with line numbers and pointer indicators**, and **low-level parsing techniques**.  
It handles nested JSON objects, arrays, and complex structures while providing **color-coded JSON printing** for easy debugging.

---

## ✅ Features

- Parse **JSON objects, arrays, strings, numbers (integers & doubles), booleans, and null values**.
- Dynamically **create, read, update, and delete JSON nodes**.
- **Print JSON with color-coded syntax** for clear visualization and debugging.
- **Save JSON to files** with proper formatting and indentation.
- **Manual memory management** with **arena-style allocation** through helper functions.
- **Error tracking** with line numbers, pointer indicators, and color-coded messages.
- Support for **nested JSON structures and multiple values per node**.

---

## ⚡ Advantages Over Other JSON Parsers

| Feature | JPAXER | Advantage |
|---------|--------|-----------|
| Memory management | Arena allocation | Fast allocation for multiple nodes, avoids fragmentation |
| Error handling | Line numbers, pointer indicators, color-coded messages | Easier debugging and error localization |
| Node manipulation | Full create, read, update, delete support | Dynamic JSON tree operations |
| Printing | Color-coded JSON | Improves readability and debugging |
| Nested structures | Supports arrays, objects, and multiple values per node | Handles complex JSON easily |
| Dependencies | Zero dependencies | Portable and lightweight |

---

## ❌ Limitations

1. **Arena-only allocation:** No heap allocation is used directly; all values must be allocated via provided helper functions.  
2. **Memory release:** Updates and deletions do not free individual nodes; all memory is released only when the arena is reset.  
3. **Large JSON files:** Storing the entire tree in memory may limit handling of extremely large files efficiently.  
4. **Partial standard compliance:** Unicode escapes, scientific notation, and some edge-case JSON structures may not be fully supported.  

**Note:** JPAXER reads files incrementally in buffers but **retains the full JSON tree in memory**—this is not full streaming parsing.

---

## 💾 Memory Management

- All values (non-strings) must be allocated using helper functions:  
  - `get_J_INT(long value)`  
  - `get_J_DOUBLE(double value)`  
  - `get_J_BOOL(bool value)`  
  - `get_J_STRING(char* value)` for strings.  
- **Free all memory** at the end using `free_memory()`.  
- Arena allocator ensures fast allocation but individual deletes do **not** reclaim memory.

---

## 📝 API Reference

### Parsing JSON

| Function | Signature | Description |
|----------|-----------|-------------|
| Parse from file | `JSON_NODE* parse_JSON_from_FILE(FILE* json_file);` | Parse JSON file and return root node. |
| Parse from string | `JSON_NODE* parse_JSON_from_str(char* json_str);` | Parse JSON string and return root node. |

### Node Access & Manipulation

| Function | Signature | Description |
|----------|-----------|-------------|
| Get nested node | `JSON_NODE* get(JSON_NODE** root, size_t args_count, const char* key1, ...);` | Retrieve nested node using key chain. |
| Set value | `bool set(JSON_NODE** root, const char* key, void* new_data, JSON_DTYPE type);` | Update a node’s value. |
| Add node | `bool add(JSON_NODE** root, const char* key, void* new_data, JSON_DTYPE type);` | Add a new key-value pair. |
| Delete node | `bool del(JSON_NODE** root, const char* key);` | Remove a key-value pair. |
| Move up tree | `bool back(JSON_NODE** root, int steps);` | Navigate up the tree. |

### Supported Data Types

| Type | Description |
|------|------------|
| `J_STRING` | String value |
| `J_OBJECT` | JSON object |
| `J_BOOL` | Boolean |
| `J_NULL` | Null value |
| `J_DOUBLE` | Floating-point number |
| `J_INT` | Integer |

### Utility Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| Save JSON | `void save_JSON_to_FILE(JSON_NODE* data, FILE* json_file);` | Write JSON to file with formatting. |
| Print JSON | `void print_JSON_NODE(JSON_NODE* node);` | Print JSON with color-coded syntax. |

### Helper Functions for Values

| Function | Signature | Description |
|----------|-----------|-------------|
| Integer | `void* get_J_INT(long int data);` | Allocate JSON integer value. |
| Double | `void* get_J_DOUBLE(double data);` | Allocate JSON double value. |
| Boolean | `void* get_J_BOOL(bool data);` | Allocate JSON boolean value. |
| String | `void* get_J_STRING(char* data);` | Allocate JSON string value. |

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

---

## ⚠ Disclaimer

**WARNING:** JPAXER is **not production-ready**. Memory management relies on arena allocation and may result in dangling pointers or out-of-bounds access if misused.  
All memory is released only when the arena is reset. Use at your own risk.  
The author is **not responsible** for any damage or data loss. This library is designed strictly for **educational and experimental purposes**.

