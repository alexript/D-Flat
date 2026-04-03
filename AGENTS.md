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

### Unit Tests (TinyCC Port)

The project now includes a unit testing framework for the TinyCC Windows port:

```bash
# Build and run tests
cd tests
c:\tcc\make.exe -f Makefile.test run

# Just build
c:\tcc\make.exe -f Makefile.test

# Clean
c:\tcc\make.exe -f Makefile.test clean
```

### Testing Framework

- **Single-header library**: `tests/df_test.h` (stb-style)
- **Implementation**: Define `DF_TEST_IMPLEMENTATION` before including
- **Test files**: `test_rect.c`, `test_keys.c`, etc.
- **Minimal D-Flat header**: `tests/test_dflat.h` (for testing without DOS dependencies)

### Running DOS Build Tests

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

- Preprocessor defines: `BCPP` (Borland), `TURBOC`, `WATCOM`, `MSC` (Microsoft), `__SMALLER_C__`, `__TINYC__`
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
- **TinyCC Windows port**: Uses Windows Console API instead of BIOS

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
| `tests/df_test.h` | Unit testing framework (single-header) |
| `tests/test_*.c` | Unit test files |
| `tinycc/tcccompat.h` | TinyCC DOS compatibility layer |
| `tinycc/tccwin32.h` | Win32 function declarations |
| `tinycc/tccwin32.c` | Win32 Console API implementation (~660 lines) |
| `console.h` | Platform Abstraction Layer (PAL) interface |
| `win32/console_win32.c` | Win32 PAL implementation (~120 lines) |
| `dos/console_dos.c` | DOS PAL implementation (~100 lines) |

## Development Workflow

1. Make changes to source files
2. Rebuild: `make -f makefile.bcc all`
3. For TinyCC port: `cd tests && make run`
4. Test in DOS environment or Windows console
5. Verify with automated tests (when available)

## TinyCC Port Status

### Completed Phases

- **Phase 1**: Unit testing infrastructure
  - `tests/df_test.h` - Single-header testing framework
  - `tests/test_rect.c`, `tests/test_keys.c` - Unit tests
  - `tests/test_dflat.h` - Minimal D-Flat headers for testing

- **Phase 2**: Compatibility infrastructure
  - `tinycc/tcccompat.h` - DOS compatibility macros and types (far/near, REGS, interrupts)
  - `tinycc/tccwin32.h` - Win32 function declarations
  - `tinycc/tccwin32.c` - Win32 Console API implementation (~660 lines)
  - `tests/test_tcccompat.c` - Tests for compatibility layer (9 tests)
  - `tests/test_tccwin32.c` - Tests for Win32 functions (12 tests)

- **Phase 3**: Platform Abstraction Layer (PAL)
  - `console.h` - Platform-independent console interface (~200 lines)
  - `win32/console_win32.c` - Win32 PAL implementation (~120 lines)
  - `dos/console_dos.c` - DOS PAL implementation (~100 lines)
  - `tests/test_console.c` - Tests for Console_* API (17 tests)

### Current Status

- **Test Suite**: 58/58 tests passing
- **Next**: Phase 4 - Modify D-Flat headers (dflat.h, system.h, video.h) to use PAL