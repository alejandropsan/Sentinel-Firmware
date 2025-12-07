# Coding Style Guide

This document defines the coding standards and style guidelines for Sentinel Firmware. Following these guidelines ensures code consistency, readability, and maintainability.

## Table of Contents

- [General Principles](#general-principles)
- [File Organization](#file-organization)
- [Formatting](#formatting)
- [Naming Conventions](#naming-conventions)
- [Comments and Documentation](#comments-and-documentation)
- [C Language Standards](#c-language-standards)
- [Error Handling](#error-handling)
- [Memory Management](#memory-management)
- [Platform-Specific Guidelines](#platform-specific-guidelines)
- [Tools and Automation](#tools-and-automation)

---

## General Principles

### Code Quality

- **Readability over cleverness**: Write code that is easy to understand
- **Consistency**: Follow existing patterns in the codebase
- **Simplicity**: Prefer simple solutions over complex ones
- **Maintainability**: Write code that is easy to modify and extend
- **Performance**: Consider performance, but not at the cost of readability

### Best Practices

- One logical change per commit
- Keep functions small and focused
- Minimize global variables
- Use const correctness
- Check all return values
- Validate all inputs

## File Organization

### File Structure

```c
// 1. File header comment
/**
 * @file example.c
 * @brief Brief description of file purpose
 */

// 2. System includes
#include <stdio.h>
#include <stdlib.h>

// 3. Library includes
#include <furi.h>
#include <gui/gui.h>

// 4. Local includes
#include "example.h"
#include "example_private.h"

// 5. Defines and macros
#define TAG "Example"
#define MAX_BUFFER_SIZE 256

// 6. Type definitions
typedef struct {
    int value;
    char* name;
} ExampleStruct;

// 7. Static function declarations
static void example_helper(void);

// 8. Global variables (minimize usage)
static int global_counter = 0;

// 9. Function implementations
```

### Header Files

Every `.h` file must have include guards:

```c
#pragma once

// or alternatively:

#ifndef EXAMPLE_H
#define EXAMPLE_H

// header content

#endif // EXAMPLE_H
```

**Prefer `#pragma once`** for simplicity and better compiler support.

### Include Order

1. System headers (`<stdio.h>`, `<stdlib.h>`)
2. Furi framework headers (`<furi.h>`)
3. GUI and library headers (`<gui/gui.h>`)
4. Local project headers (`"example.h"`)

Do not sort includes alphabetically within groups.

## Formatting

### Automatic Formatting

The project uses **clang-format** for automatic code formatting.

```bash
# Format all changed files
./fbt format

# Check formatting without changes
./fbt format_check
```

**Always run `./fbt format` before committing code.**

### Indentation

- **Use 4 spaces** for indentation
- **Never use tabs**
- Continuation lines: 4 space indent

```c
// Good
if(condition) {
    do_something();
}

// Bad
if(condition) {
  do_something();  // 2 spaces - wrong
}
```

### Line Length

- **Maximum 99 characters** per line
- Break long lines logically

```c
// Good
void function_with_many_parameters(
    int first_param,
    int second_param,
    int third_param) {
    // implementation
}

// Bad
void function_with_many_parameters(int first_param, int second_param, int third_param) {  // Too long
```

### Braces

Use **K&R style** (opening brace on same line):

```c
// Good
if(condition) {
    do_something();
} else {
    do_something_else();
}

// Good
void function(void) {
    // implementation
}

// Bad
if(condition)
{
    do_something();
}
```

#### Single Statement Blocks

For single statements, braces are optional but recommended:

```c
// Acceptable
if(condition) do_something();

// Preferred
if(condition) {
    do_something();
}

// Required for multi-line
if(condition) {
    do_something();
    do_something_else();
}
```

### Spacing

#### Function Calls

**No space** before parentheses:

```c
// Good
function(arg1, arg2);
if(condition) {

// Bad
function (arg1, arg2);
if (condition) {
```

#### Operators

Space around binary operators:

```c
// Good
int result = a + b * c;
if(x == y && z != w) {

// Bad
int result=a+b*c;
if(x==y&&z!=w) {
```

No space around unary operators:

```c
// Good
i++;
*ptr = value;
!condition

// Bad
i ++;
* ptr = value;
! condition
```

#### Pointers

Pointer asterisk aligned **left** (with type):

```c
// Good
int* ptr;
char* string;
void* data;

// Bad
int *ptr;
int * ptr;
```

#### Function Declarations

```c
// Good - parameters on new lines
void long_function_name(
    int first_parameter,
    char* second_parameter,
    size_t third_parameter);

// Good - short declaration
void short_func(int x, int y);

// Bad - mixed
void function(int x,
              char* y, size_t z);
```

### Control Structures

```c
// if-else
if(condition) {
    statement;
} else if(other_condition) {
    other_statement;
} else {
    default_statement;
}

// for loops
for(int i = 0; i < count; i++) {
    process(i);
}

// while loops
while(condition) {
    do_something();
}

// switch statements
switch(value) {
case VALUE_1:
    handle_case_1();
    break;
case VALUE_2:
    handle_case_2();
    break;
default:
    handle_default();
    break;
}
```

## Naming Conventions

### General Rules

- Use **snake_case** for all names
- Use **SCREAMING_SNAKE_CASE** for macros and constants
- Be descriptive but concise
- Avoid abbreviations unless well-known

### Functions

```c
// Good
void subghz_transmit_signal(void);
bool nfc_read_card(NfcCard* card);
static void process_internal_data(void);

// Bad
void SubGhzTransmitSignal(void);  // PascalCase
void tx(void);                     // Too cryptic
void nfc_ReadCard(void);          // Mixed case
```

### Variables

```c
// Good
int counter;
char* file_path;
uint32_t max_frequency;

// Bad
int Counter;           // PascalCase
char* filePath;        // camelCase
uint32_t maxFreq;      // Abbreviation
```

### Type Definitions

```c
// Structures
typedef struct {
    int field1;
    char* field2;
} MyStructure;

// Enums
typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_STOPPED,
} SystemState;

// Function pointers
typedef void (*CallbackFunction)(void* context);
```

### Constants and Macros

```c
// Constants
#define MAX_BUFFER_SIZE 256
#define DEFAULT_TIMEOUT 1000
#define TAG "MyModule"

// Macros
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
```

### File Names

- Use **snake_case** for file names
- Match header and source file names
- Use descriptive names

```
// Good
subghz_transmitter.c
subghz_transmitter.h
nfc_card_reader.c

// Bad
SubGhzTransmitter.c
nfc-card-reader.c
transmit.c  // Too vague
```

## Comments and Documentation

### Doxygen Documentation

Use **Doxygen-style** comments for public APIs:

```c
/**
 * @file example.h
 * @brief Brief description of the file
 *
 * Detailed description of the file purpose and contents.
 */

/**
 * @brief Brief description of the function
 *
 * Detailed description of what the function does,
 * including any important behavior or side effects.
 *
 * @param first_param Description of first parameter
 * @param second_param Description of second parameter
 * @return Description of return value
 */
int example_function(int first_param, char* second_param);
```

### Inline Comments

```c
// Single-line comment for simple explanations
int counter = 0; // Initialize counter

/*
 * Multi-line comment for complex explanations
 * that require more detail
 */
```

### Comment Guidelines

- **DO**: Explain why, not what
- **DO**: Document non-obvious behavior
- **DO**: Explain complex algorithms
- **DON'T**: State the obvious
- **DON'T**: Leave commented-out code

```c
// Good
// Delay required for hardware stabilization
furi_delay_ms(100);

// Bad
// Delay 100 milliseconds
furi_delay_ms(100);
```

### TODO and FIXME

```c
// TODO: Implement error handling
// FIXME: Race condition when accessing shared data
// NOTE: This assumes frequency is in Hz
```

## C Language Standards

### Language Version

- Use **C11** or **C17** standard
- Avoid compiler-specific extensions when possible
- Use standard library functions when available

### Data Types

#### Standard Types

```c
// Good - use standard types
#include <stdint.h>
#include <stdbool.h>

uint8_t byte_value;
uint32_t large_value;
bool flag;
size_t buffer_size;

// Bad - platform-dependent
unsigned int value;  // Size varies by platform
int flag;           // Use bool instead
```

#### Type Safety

```c
// Good - explicit casting
int value = (int)float_value;
void* ptr = (void*)int_ptr;

// Bad - implicit conversion
int value = float_value;
```

### Boolean Values

```c
// Good
#include <stdbool.h>
bool is_valid = true;
bool is_error = false;

if(is_valid) {
    // ...
}

// Bad
int is_valid = 1;
if(is_valid == true) {  // Redundant comparison
```

### Null Pointers

```c
// Good
if(ptr == NULL) {
    // handle null
}

char* str = NULL;

// Bad
if(!ptr) {  // Less clear
}

char* str = 0;
```

### Assertions

Use `furi_assert()` to check preconditions:

```c
void process_data(int* data, size_t size) {
    furi_assert(data);      // Check non-null
    furi_assert(size > 0);  // Check valid size

    // Process data
}
```

## Error Handling

### Return Values

- **Check all return values**
- Use bool for success/failure
- Use specific error codes when needed

```c
// Good
bool read_file(const char* path, char* buffer, size_t size) {
    furi_assert(path);
    furi_assert(buffer);

    if(!validate_path(path)) {
        return false;
    }

    // Read file
    return true;
}

// Usage
if(!read_file(path, buffer, size)) {
    FURI_LOG_E(TAG, "Failed to read file");
    return;
}
```

### Error Logging

```c
#define TAG "MyModule"

// Different log levels
FURI_LOG_E(TAG, "Error: %s", error_message);     // Error
FURI_LOG_W(TAG, "Warning: %s", warning_message); // Warning
FURI_LOG_I(TAG, "Info: %s", info_message);       // Info
FURI_LOG_D(TAG, "Debug: %s", debug_message);     // Debug
```

### Resource Cleanup

Always clean up resources, even on error:

```c
bool process_file(const char* path) {
    File* file = NULL;
    char* buffer = NULL;
    bool success = false;

    file = storage_file_alloc();
    buffer = malloc(BUFFER_SIZE);

    if(!file || !buffer) {
        goto cleanup;
    }

    // Process file
    success = true;

cleanup:
    if(buffer) free(buffer);
    if(file) storage_file_free(file);

    return success;
}
```

## Memory Management

### Allocation

```c
// Good - check allocation
int* array = malloc(size * sizeof(int));
if(!array) {
    FURI_LOG_E(TAG, "Memory allocation failed");
    return false;
}

// Initialize memory
memset(array, 0, size * sizeof(int));
```

### Deallocation

```c
// Good - free and nullify
free(ptr);
ptr = NULL;

// Good - check before freeing
if(ptr) {
    free(ptr);
    ptr = NULL;
}
```

### Furi Memory Functions

Prefer Furi memory functions when available:

```c
// Furi allocation
void* ptr = malloc(size);

// Furi string handling
FuriString* str = furi_string_alloc();
furi_string_set(str, "Hello");
// ... use string
furi_string_free(str);
```

### Memory Leaks

- Every `malloc()` must have a corresponding `free()`
- Use `goto cleanup` pattern for error handling
- Avoid allocating in loops
- Profile memory usage

## Platform-Specific Guidelines

### Furi Framework

#### String Handling

```c
// Use FuriString for dynamic strings
FuriString* str = furi_string_alloc();
furi_string_printf(str, "Value: %d", value);
const char* c_str = furi_string_get_cstr(str);
// ... use string
furi_string_free(str);
```

#### Threads and Synchronization

```c
// Use Furi threading
FuriThread* thread = furi_thread_alloc();
furi_thread_set_name(thread, "WorkerThread");
furi_thread_set_callback(thread, worker_function);
furi_thread_start(thread);
// ...
furi_thread_join(thread);
furi_thread_free(thread);
```

#### Message Queues

```c
// Use FuriMessageQueue
FuriMessageQueue* queue = furi_message_queue_alloc(8, sizeof(Event));
// ... use queue
furi_message_queue_free(queue);
```

### GUI Applications

#### View Structure

```c
// Allocate view
View* view = view_alloc();
view_set_context(view, app);
view_set_draw_callback(view, draw_callback);
view_set_input_callback(view, input_callback);

// Add to view dispatcher
view_dispatcher_add_view(app->view_dispatcher, ViewId, view);
```

#### Scene Manager

```c
// Use scene manager for navigation
SceneManager* scene_manager = scene_manager_alloc(&scene_handlers, app);
scene_manager_next_scene(scene_manager, SceneMain);
```

## Tools and Automation

### Pre-commit Checklist

Before committing code:

```bash
# 1. Format code
./fbt format

# 2. Run linter
./fbt lint

# 3. Build firmware
./fbt updater_package

# 4. Run tests (if available)
./fbt firmware_test
```

### Editor Configuration

The project includes `.editorconfig`:

- End of line: LF (Unix style)
- Charset: UTF-8
- Insert final newline: Yes
- Indent style: Spaces
- Indent size: 4

### Clang-Format Configuration

The project uses `.clang-format` with these key settings:

- **IndentWidth**: 4
- **ColumnLimit**: 99
- **BreakBeforeBraces**: Attach (K&R style)
- **PointerAlignment**: Left
- **SpaceBeforeParens**: Never
- **Standard**: C++20 (for C++ compatibility)

### IDE Setup

#### VSCode

Install recommended extensions:
- C/C++
- clang-format
- EditorConfig

Settings in `.vscode/settings.json`:
```json
{
    "editor.formatOnSave": true,
    "C_Cpp.clang_format_style": "file"
}
```

## Examples

### Good Example

```c
/**
 * @file subghz_transmitter.c
 * @brief SubGHz signal transmission module
 */

#include <furi.h>
#include <furi_hal_subghz.h>
#include "subghz_transmitter.h"

#define TAG "SubGhzTX"
#define MAX_TRANSMIT_TIME 5000

/**
 * @brief Transmit SubGHz signal
 *
 * @param frequency Frequency in Hz
 * @param data Data buffer to transmit
 * @param size Size of data buffer
 * @return true on success, false on error
 */
bool subghz_transmit_signal(uint32_t frequency, const uint8_t* data, size_t size) {
    furi_assert(data);
    furi_assert(size > 0);

    if(frequency < 300000000 || frequency > 900000000) {
        FURI_LOG_E(TAG, "Invalid frequency: %lu", frequency);
        return false;
    }

    // Initialize radio
    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    furi_hal_subghz_set_frequency(frequency);

    // Transmit data
    furi_hal_subghz_start_async_tx(data, size);
    furi_delay_ms(MAX_TRANSMIT_TIME);
    furi_hal_subghz_stop_async_tx();

    FURI_LOG_I(TAG, "Transmitted %zu bytes at %lu Hz", size, frequency);
    return true;
}
```

### Bad Example

```c
// No file header
#include "subghz_transmitter.h"
#include <furi_hal_subghz.h>  // Wrong order
#include <furi.h>

// Bad function name (camelCase)
bool transmitSignal(uint32_t freq, const uint8_t* d, size_t s) {  // Cryptic names
    // No assertions
    // No validation

    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    furi_hal_subghz_set_frequency(freq);
    furi_hal_subghz_start_async_tx(d, s);  // No error checking
    furi_delay_ms(5000);  // Magic number
    furi_hal_subghz_stop_async_tx();

    return true;  // Always returns true
}
```

---

## Summary

- **Format**: Run `./fbt format` before committing
- **Indent**: 4 spaces, no tabs
- **Naming**: snake_case for functions/variables, SCREAMING_SNAKE_CASE for macros
- **Braces**: K&R style (opening brace on same line)
- **Comments**: Doxygen for APIs, explain why not what
- **Pointers**: Left-aligned (`int* ptr`)
- **Errors**: Always check return values, use assertions
- **Memory**: Always free allocated memory

When in doubt, **follow existing code patterns** in the repository.

For questions or clarifications, open a GitHub Discussion or ask in the community channels.
