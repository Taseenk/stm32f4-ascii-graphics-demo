# Screen & Cursor Control

Terminal output relies on control sequences to clear the display, position the cursor, and toggle visibility states. This document defines the foundational operations required for terminal scene management.

Coordinate systems within this module makes use of a 1-based indexing where the top-left corner is defined as (1, 1). This implementation maintains parity with ANSI/VT100 conventions and ensures consistency when translating hardware cursor positions.

## Coordinate System

The terminal is 80 columns wide and 24 rows tall. Columns are numbered 1–80 (left to right), rows are numbered 1–24 (top to bottom).

```c
// Example: Position the cursor at column 40, row 12 (centre of screen)
TerminalSetCursorPos(40, 12);
```

!!! warning "Out-of-bounds coordinates"
    Coordinates outside the defined range (1–80 for columns, 1–24 for rows) are silently discarded. The function terminates without transmitting an ANSI sequence. For example, a call to TerminalSetCursorPos(100, 50) results in no state change. the cursor remains at its previous valid position.

!!! note "Coordinate normalization"
    If a coordinate is 0, the module automatically normalizes it to 1. This is a safety feature. The ANSI terminal cannot position a cursor where either the column (0, y) or row (x, 0) are set to 0.

## Initialization

Before any graphics or text can be rendered, the terminal environment must be established via a call to `TerminalInit()`. Executed once at system startup, this function initializes the terminal state, configures the physical dimensions of the emulator, and prepares the internal framebuffers required for double-buffering.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalInit(uint8_t cursor, uint16_t col, uint16_t row)

This function does the following:

1. Sets terminal dimensions via ANSI escape sequence
2. Clears the screen and homes the cursor to (1, 1)
3. Shows or hides the cursor based on the parameter
4. Initializes internal framebuffers (used for buffered rendering)
5. Resets all text styling to default (no bold, dim, colour, etc.)

```c
// Example: initialization hide cursor, 80×24 terminal
TerminalInit(FALSE, 80, 24);
```

!!! tip "Cursor visibility"
    For scenes with text-only rendering, hiding the cursor (`FALSE`) reduces visual clutter. For interactive dashboards or command prompts, show the cursor (`TRUE`).

## Screen Control

Screen control functions manage the global state of the display. These operations are typically used at the start of a new frame or during transitions between different UI scenes to ensure the drawing surface is clean and correctly sized.

### Clear and Home

These functions provide a way to wipe the screen of all existing characters. While `TerminalClearScreen` only erases the content, `TerminalClearAndHome` is the standard for frame resets, as it also resets the hardware cursor to the top-left corner.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalClearScreen(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalClearAndHome(void)

```c
// Example: Reset display at the start of each frame
TerminalClearAndHome();
TerminalSetAttribute(TERM_ATTR_RESET);  // Ensure no leftover styles
```

### Set Dimensions

This function allows the module to request specific window dimensions from the terminal emulator, ensuring the display matches the layout expected by the internal logic. It works by sending an Xterm resize sequence that most emulators, such as PuTTY or Tera Term, will follow.

!!! note "Verification Limitation"
    This is a one-way command. The function has no way to check if the terminal actually resized or if the user's software even supports the request.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalSetDimensions(uint16_t col, uint16_t row)

!!! note "Buffer Limitations"
    Requesting a larger dimension does not automatically expand the terminal buffer. This must be adjusted manually. The terminal only supports an 80x24 character layout, regardless of the dimensions reported by the terminal.

```c
// Example: Request a 80×24 terminal
TerminalSetDimensions(80, 24);
```

## Cursor Control

The cursor marks where characters will appear next. Instead of printing everything in order, these functions allow for jumping around to specific grid coordinates. This makes it easy to handle partial refreshes like updating a header or a status bar without flickering or redrawing the entire screen.

### Positioning

Unlike standard serial printing that moves sequentially from left to right, these functions enable jumps to any specific coordinate on the terminal. This flexibility provides the foundation for constructing complex layouts, dashboards, and dynamic scenes.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalCursorHome(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalSetCursorPos(uint16_t col, uint16_t row)

```c
// Example: Position at column 20, row 5
TerminalSetCursorPos(20, 5);
 
// Example: Return to top-left
TerminalCursorHome();
```

### Visibility

Cursor visibility serves as a purely aesthetic toggle and does not affect the underlying rendering logic. While a blinking cursor can be a distraction in graphical scenes or animations, these functions enable its suppression during rendering and its restoration when user input is required.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalInvisibleCursor(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalVisibleCursor(void)

```c
// Example: Hide cursor for a scene with only graphics output
TerminalInvisibleCursor();
 
// Example: Show cursor for a menu or text input
TerminalVisibleCursor();
```

## ANSI Escape Codes Reference

These codes are sent directly by the functions above. Included for reference when working with raw `TerminalPrint()` or debugging.

| Operation | ANSI Code | Header Macro | Function |
| :--- | :--- | :--- | :--- |
| Clear screen | `ESC[2J` | `ANSI_CLEAR_SCREEN` | `TerminalClearScreen()` |
| Cursor home | `ESC[H` | `ANSI_CURSOR_HOME` | `TerminalCursorHome()` |
| Hide cursor | `ESC[?25l` | `ANSI_CURSOR_INVISIBLE` | `TerminalInvisibleCursor()` |
| Show cursor | `ESC[?25h` | `ANSI_CURSOR_VISIBLE` | `TerminalVisibleCursor()` |
| Set Position | `ESC[r;cH` | — | `TerminalSetCursorPos()` |
| Set Dimensions | `ESC[8;r;ct` | — | `TerminalSetDimensions()` |

!!! example "Direct sequence output"
    The header file defines convenient macros for these codes. For example:
    ```c
    #define ANSI_ESC "\x1b[" // The standard ANSI Escape initiator
    #define ANSI_CURSOR_HOME ANSI_ESC "H"  // Expands to "\x1b[H"
    ```
    Use these macros if manually constructing escape sequences with `TerminalPrint()`.

## Common Patterns

### Initialize and Clear Before Each Scene

```c
// At scene start
TerminalClearAndHome();
TerminalSetAttribute(TERM_ATTR_RESET);
```

### Position Text at a Specific Location

```c
// Move to column 10, row 5, then print
TerminalSetCursorPos(10, 5);
TerminalPrint("Hello, World!");
```

### Hide Cursor During Animation, Show During Input

```c
// During scene rendering
TerminalInvisibleCursor();

// render frames ...
 
// When returning to shell/menu
TerminalVisibleCursor();
```

## See Also

Other modules cover character styling and color management. Applying these attributes to positioned elements enables frame construction and scene depth management. For shape and geometric primitive drawing, refer to the main graphics module implementation.

- [Color & Text Attributes](terminal-color-attributes.md) — Styling text with bold, colour, blink, etc.
- [Rendering Modes & Graphics](terminal-rendering.md) — Sending characters and drawing primitives
