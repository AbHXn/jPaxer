# JPAXER: JSON Parser from Scratch in C

JPAXER is a JSON parser and manipulator library written entirely in C from scratch.
It was designed as a fun learning project to explore **C memory management, pointers, and JSON parsing** in a low-level environment.

---

## Features

* Parse JSON objects, arrays, strings, numbers (integers and doubles), booleans, and null values.
* Create, read, update, and delete JSON nodes dynamically.
* Print JSON with color-coded syntax for easier debugging.
* Save JSON to files with proper indentation.
* Manual memory management with custom `_malloc` and arena-style allocation.
* Error tracking with red-colored messages, line numbers, and pointer indicators.
* Supports nested JSON objects, arrays, and complex structures, with multiple values per node.

---

## Scope

* **Designed for learning and experimentation** with C, pointers, and parsing logic.
* Handles typical JSON structures and syntax.
* Focused on clarity and understanding rather than production-grade robustness.
* Demonstrates **manual memory management** and **custom error handling**.

---

## Limitations

* Some recoverable errors are skipped but logged.
* All errors (allocation, read, or object errors) are captured and printed in red with contextual information.
* Strings in JSON are currently safe because they are allocated dynamically or referenced statically. Other types (int, double, bool) **must be dynamically allocated** to avoid dangling pointers.
* No automatic garbage collection. Users must call `free_memory()` to release all allocated memory.
* Does not support extremely large JSON files efficiently (no streaming parsing).
* No full compliance with the JSON standard (e.g., Unicode escapes, scientific notation parsing may be limited).

---

## Memory Management

* Custom memory allocator `_malloc` is used to improve allocation speed.
* Avoid passing stack-allocated variables directly to `add` or `set` for non-string values.
* Use helper functions like `get_J_INT(long value)` or `get_J_DOUBLE(double value)` to safely allocate memory for numeric values.
* Always free the memory after usage using `free_memory()` to avoid memory leaks.

---

## Error Handling

JPAXER provides structured error handling:

* Color-coded terminal messages (red for errors, syntax colors for JSON output).
* Line number and pointer position to help locate issues in JSON input.
* Centralized error API for programmatic access:

  * `get_read_error()`
  * `get_object_error()`
  * `get_allocation_err()`
* Errors include allocation failures, invalid keys, null values, read buffer issues, and more.

---

## Usage Example

```c
#include "jpaxer.h"

int main() {
	// open json file
    FILE* jfile1 = fopen( "jfiles/test1.json", "r" );

	if( !jfile1 ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* first_json = parse_JSON_from_FILE( jfile1 );
	fclose( jfile1 );

	FILE* jfile2 = fopen( "jfiles/test2.json", "r" );

	if( !jfile2 ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* second_json = parse_JSON_from_FILE( jfile2 );
	fclose( jfile2 );

	get( first_json, 2, "configurations", "0" );
	get( second_json, 2, "0", "teamMembers" );

	print_JSON_node( first_json );
	print_JSON_node( second_json );

	set( first_json, "params", ( void* )second_json, J_OBJECT );

	back( second_json, 1 );

	get( second_json, 1, "details" );
	add( first_json, "new_details", ( void* )second_json, J_OBJECT );
	add( first_json, "whoisthis", get_J_STRING("admin"), J_STRING );

	add( first_json, "user_id", get_J_INT(123), J_INT );

	print_JSON_node( first_json );

	FILE* jile = fopen( "TEST.json", "w" );
	JSON_FLUSH( first_json, jile, 2 );
    free_memory();

    return 0;
}
```

---

## Developer Notes

* Strings in JSON can be safely passed as string literals because they are immutable and won't be modified by JPAXER.
* All non-string types (int, double, bool) **must be heap-allocated** using helper functions (`get_J_INT`, `get_J_DOUBLE`, `get_J_BOOL`) to avoid dangling pointers.
* Functions are provided for JSON traversal:

  * `__read__` / `get()` for nested key reading
  * `__update__` / `set()` for updating values
  * `__create__` / `add()` for creating new keys
  * `__delete__` / `del()` for removing keys
  * `__back__` / `back()` to move up the tree
* The parser and object system are entirely **handcrafted** with low-level C memory management for educational purposes.

---

## License

JPAXER is released for educational purposes and personal learning projects. You may freely modify, study, and experiment with it.

---

## Disclaimer

* JPAXER is **not production-ready**.
* Use with care, especially regarding memory allocation, as improper usage may lead to memory leaks or undefined behavior.
* Error handling is designed for debugging and learning rather than complete fault tolerance.
