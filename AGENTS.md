# D-Flat Agent Guidelines

This document provides guidance for agentic coding agents working in this repository.

## Project Overview

D-Flat is a C Text User Interface Library (public domain, CC0 1.0) for DOS. It provides a windowing system with dialog boxes, menus, controls, and an editor (memopad). The code targets multiple compilers: Borland C++ 3.1/4.0, Turbo C++ 1.01, and Watcom C.

## Build Commands

### Build System

This project uses makefiles (not CMake or modern build systems). Three makefiles are provided:

```bash
# Borland C++ (makefile.bcc)
make -f makefile.bcc

# Turbo C++ (makefile.tcc)  
make -f makefile.tcc

# Watcom C (makefile.wat)
wcl -f makefile.wat
```

### Build Targets

```bash
# Build everything (default target)
make -f makefile.bcc all

# Build only the library
make -f makefile.bcc dflat.lib

# Build the memopad demo application
make -f makefile.bcc memopad.exe

# Build the help file compiler tools
make -f makefile.bcc huffc.exe fixhelp.exe

# Generate help file (requires huffc.exe and fixhelp.exe)
make -f makefile.bcc memopad.hlp

# Clean build artifacts
del *.obj *.lib *.exe
```

### Compiler-Specific Options

The makefiles define these key macros (can be commented out to modify build):

- `BUILD_FULL_DFLAT` - Enable all features (full build)
- `TESTING_DFLAT` - Enable testing/logging features
- `-v` - Debug symbols (Borland)
- `-Od` - Disable optimizations (Borland)

### Single File Compilation

To compile a single source file (example for Borland):

```bash
bcc -c -d -ml yourfile.c
```

Flags: `-c` = compile only, `-d` = merge duplicates, `-ml` = large model

## Testing

**This project has no automated test suite.** The "test" patterns found in the code (e.g., `TestAttribute`, `TestCriticalError`) are internal functions, not unit tests.

To verify changes:
1. Build the library: `make -f makefile.bcc dflat.lib`
2. Build memopad: `make -f makefile.bcc memopad.exe`
3. Run memopad in a DOS environment or emulator (DOSBox)
4. Manually test functionality

## Code Style Guidelines

### File Organization

- **Headers (.h)**: Declarations, prototypes, type definitions, macros
- **Sources (.c)**: Implementation
- **One source file per module**: window.c, video.c, message.c, etc.
- **Header guards**: Use `#ifndef NAME_H` / `#define NAME_H` / `#endif`

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Macros | UPPER_CASE | `#define MAXTEXTLEN 65000U` |
| Enums | PascalCase or UPPER_CASE | `enum Condition { ISRESTORED, ISMINIMIZED, ... }` |
| Types (typedef) | PascalCase | `typedef enum {FALSE, TRUE} BOOL;` |
| Functions | PascalCase or camelCase | `CreateWindow()`, `SendMessage()` |
| Variables | camelCase or lowercase | `wnd`, `Class`, `attrib` |
| Struct members | lowercase or camelCase | `wnd->rc`, `wnd->title` |
| Constants | UPPER_CASE | `MAXMESSAGES`, `DELAYTICKS` |
| File names | lowercase | `window.c`, `dflat.h` |

### Prefixes (Hungarian Notation - used sparingly)

- `wnd` - Window pointer variable: `WINDOW wnd`
- `ttl` - Title string: `const char *ttl`
- `p` / `p1` / `p2` - Message parameters: `PARAM p1, PARAM p2`

### Formatting

- **Indentation**: Tabs (8 spaces typical)
- **Line length**: Keep under 80 characters when practical
- **Braces**: Opening brace on same line as control statement
- **Spaces**: Use spaces around operators: `if (height == -1)`

### Types and Data Structures

- Use `BOOL` (enum {FALSE, TRUE}) for boolean values, not `int` or `char`
- Use `int` for most integers; use `unsigned` when appropriate
- Use `size_t` for memory allocation sizes
- Use `long` for message parameters: `typedef long PARAM`
- Use `enum` for related constants (messages, window classes, conditions)
- Use `#define` for numeric constants, not `const int`

### Memory Management

- Use D-Flat's allocation functions: `DFcalloc()`, `DFmalloc()`, `DFrealloc()`
- Always check allocation return for NULL
- Free memory with standard `free()` (DF* functions wrap these)

### Header Includes

- Each .c file includes `"dflat.h"` first (or the minimal required headers)
- System headers in angle brackets: `#include <stdio.h>`
- Local headers in quotes: `#include "dflat.h"`
- Order: main header, then project headers, then system headers

### Error Handling

- Use `setjmp()`/`longjmp()` for critical error recovery
- Return error codes from functions where appropriate
- Use `NULL` to indicate failure for pointer returns

### Message System

D-Flat uses a message-passing architecture:

- Messages defined in `dflatmsg.h` via `DFlatMsg()` macro
- Window procedures handle messages: `int (*wndproc)(WINDOW, MESSAGE, PARAM, PARAM)`
- Send messages with `SendMessage(wnd, message, p1, p2)`

### Platform-Specific Code

- Preprocessor defines: `BCPP` (Borland), `TURBOC`, `WATCOM`, `MSC` (Microsoft), `__SMALLER_C__`
- Use `#ifdef` guards for platform-specific code blocks
- Inline assembly uses `asm` keyword (with appropriate guards)

### Comments

- Use C-style comments: `/* comment */`
- Place comments on their own line or at end of code lines
- Comment each function's purpose at definition
- Comment complex logic and non-obvious code

### Code Patterns

**Function prototype style:**
```c
WINDOW CreateWindow(
    CLASS Class,
    const char *ttl,
    int left, int top,
    int height, int width,
    void *extension,
    WINDOW parent,
    int (*wndproc)(struct window *, enum messages, PARAM, PARAM),
    int attrib);
```

**If statement style:**
```c
if (condition) {
    action();
}
```

**Struct access:**
```c
wnd->rc.lf = left;
wnd->rc.tp = top;
```

### Common Patterns

- Window attributes tested with `TestAttribute(wnd, FLAG)`
- Attributes added with `AddAttribute(wnd, FLAG)`
- Window creation always checks for NULL
- Screen coordinates use 0-based indexing (0-79 x, 0-24 y)

### Portability Notes

- This is a DOS-era library; runs in DOS, DOSBox, or similar emulators
- Uses BIOS int 0x10 (video), 0x16 (keyboard), 0x33 (mouse)
- Assumes 80x25 minimum screen (supports 43/50 line modes)
- Video memory access for text mode rendering

## Key Files

| File | Purpose |
|------|---------|
| `dflat.h` | Main header, includes all sub-headers |
| `window.c` | Window creation and management |
| `message.c` | Message dispatch system |
| `dflatmsg.h` | Message enum definitions |
| `classes.h` | Window class definitions |
| `classdef.h` | Window attribute flags |
| `memopad.c` | Demo editor application |
| `makefile.bcc` | Borland C++ build file |

## Development Workflow

1. Make changes to source files
2. Rebuild: `make -f makefile.bcc all`
3. Test in DOS environment
4. Verify with manual testing

No automated tests exist. All verification is manual.