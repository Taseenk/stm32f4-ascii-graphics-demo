# Colour & Text Attributes

Text attributes such as bold, dim, underline, blink, reverse, and strikethrough combined with foreground and background colours to define visually distinct interface regions. This document catalogs all available attributes and colour codes for terminal output.

## Text Attributes

Text attributes modify character appearance without affecting screen coordinates or underlying content. These styles are applied via ANSI escape sequences and persist for all subsequent output until a reset command is received.

### Attribute Enum

```c
typedef enum {
    TERM_ATTR_RESET,               // Reset all attributes to default
    TERM_ATTR_BOLD,                // Bright/bold text
    TERM_ATTR_DIM,                 // Faint/dim text
    TERM_ATTR_UNDERLINE,           // Underline text
    TERM_ATTR_BLINK,               // Blinking text
    TERM_ATTR_REVERSE,             // Reverse video (swap foreground/background)
    TERM_ATTR_STRIKE,              // Strikethrough text
    
    TERM_ATTR_RESET_BOLD,          // Disable bold (keep other attributes)
    TERM_ATTR_RESET_UNDERLINE,     // Disable underline
    TERM_ATTR_RESET_BLINK,         // Disable blink
    TERM_ATTR_RESET_REVERSE_MODE,  // Disable reverse video
    TERM_ATTR_RESET_STRIKE         // Disable strikethrough
} TerminalAttr_t;
```

### Setting Attributes

These functions manage the terminal’s text styles by issuing ANSI escape sequences to activate specific styles or selectively remove individual attributes. Once a style is applied, it remains persistent within the rendering state until modified by a selective or global reset.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalSetAttribute(TerminalAttr_t attr)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalResetStyle(void)

```c
// Example: Make subsequent text bold
TerminalSetAttribute(TERM_ATTR_BOLD);
TerminalPrint("Important");
 
// Exaple: Add underline without removing bold
TerminalSetAttribute(TERM_ATTR_UNDERLINE);
TerminalPrint("Bold and underlined");
 
// Exaple: Turn off bold only; underline remains
TerminalSetAttribute(TERM_ATTR_RESET_BOLD);
TerminalPrint("Just underlined now");
 
// Exaple: Reset everything
TerminalResetStyle();
TerminalPrint("Normal text");
```

### Attribute Details

| Attribute | ANSI Code | Visual Effect | Terminal Support |
| :--- | :--- | :--- | :--- |
| **Bold** | `ESC[1m` | Bright/heavier weight | Universal |
| **Dim** | `ESC[2m` | Faint/lighter weight | Common, may appear as normal on some terminals |
| **Underline** | `ESC[4m` | Single line below text | Universal |
| **Blink** | `ESC[5m` | Slow blink (~1 Hz) | Common, some terminals disable for accessibility |
| **Reverse** | `ESC[7m` | Swap foreground & background | Universal |
| **Strikethrough** | `ESC[9m` | Line through text | Modern terminals only (less reliable) |

!!! warning "Terminal compatibility"
    Blink and strikethrough are not supported by all terminal emulators. Test on your target terminals.

!!! note "Selective reset"
    Use `TERM_ATTR_RESET_BOLD`, `TERM_ATTR_RESET_UNDERLINE`, etc. to disable individual attributes without resetting colours. Full reset with `TERM_ATTR_RESET` will clear all styling *and* colours.

## Foreground Colours (Text Colour)

The module manages text colour through the basic 16-colour ANSI set or the 256-colour extended system. The user can select specific colours to define different object types or layers within a scene.

### Standard 16-Colour Palette (foreground)

```c
typedef enum {
    FG_BLACK = 30,
    FG_RED,           // 31
    FG_GREEN,         // 32
    FG_YELLOW,        // 33
    FG_BLUE,          // 34
    FG_MAGENTA,       // 35
    FG_CYAN,          // 36
    FG_WHITE,         // 37
    FG_DEFAULT = 39   // Terminal default (usually white)
} ForegroundColour_t;
```

### Extended 256-Colour Palette (foreground)

The module also supports xterm-256, which provides 216 RGB colours plus a 24-step grayscale ramp.

```c
// Example: Extended colours use an offset of 256
FG_DARK_GREEN = EXTENDED_COLOURS_OFFSET + 22,   // xterm index 22
FG_MEDIUM_GREEN = EXTENDED_COLOURS_OFFSET + 34, // xterm index 34
FG_BRIGHT_GREEN = EXTENDED_COLOURS_OFFSET + 82, // xterm index 82 (matrix green)
FG_DARK_RED = EXTENDED_COLOURS_OFFSET + 88,
FG_MEDIUM_RED = EXTENDED_COLOURS_OFFSET + 124,
FG_BRIGHT_RED = EXTENDED_COLOURS_OFFSET + 196,
```

### Setting Foreground Colour

These functions manage the terminal’s text colour by generating ANSI escape sequences that update the foreground state. High-level commands allow for the isolated modification of the text colour across standard 16-colour and extended 256-colour palettes. These foreground settings remain persistent within the rendering state until a subsequent colour command or a global style reset is issued.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalSetColour(ForegroundColour_t text_colour, BackgroundColour_t background_colour)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalSetTextColour(ForegroundColour_t text_colour)

```c
// Example: Standard 16-colour
TerminalSetTextColour(FG_RED);
TerminalPrint("Red text");
 
// Example: Extended 256-colour
TerminalSetTextColour(FG_BRIGHT_GREEN);
TerminalPrint("Matrix green");
 
// Example: Both foreground and background
TerminalSetColour(FG_WHITE, BG_BLUE);
TerminalPrint("White on blue");
```

## Background Colours

Background colours fill the space behind each character. The user typically applies these to render solid shapes, define screen regions, or manage the depth of field in animated frames.

### Standard 16-Colour Palette (background)

```c
typedef enum {
    BG_BLACK = 40,
    BG_RED,           // 41
    BG_GREEN,         // 42
    BG_YELLOW,        // 43
    BG_BLUE,          // 44
    BG_MAGENTA,       // 45
    BG_CYAN,          // 46
    BG_WHITE = 47,
    BG_DEFAULT = 49   // Terminal default
} BackgroundColour_t;
```

### Extended 256-Colour Palette (background)

```c
BG_DARK_BLUE = EXTENDED_COLOURS_OFFSET + 17,     // xterm index 17
BG_DARK_PURPLE = EXTENDED_COLOURS_OFFSET + 54,   // xterm index 54
BG_NEAR_BLACK_1 = EXTENDED_COLOURS_OFFSET + 232, // PLUGE test (darker than black)
BG_NEAR_BLACK_2 = EXTENDED_COLOURS_OFFSET + 234, // PLUGE test (lighter than black)
BG_DARK_GRAY = EXTENDED_COLOURS_OFFSET + 238,
```

### Setting Background Colour

Theis functions manage the terminal’s background state by generating ANSI escape sequences that isolate and update the backdrop colour. Supporting both standard 16-colour and extended 256-colour palettes, thise command allow for independent background modification without affecting the current foreground style. Once applied, the chosen background remains persistent until a subsequent update or a global style reset is issued.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalSetBackgroundColour(BackgroundColour_t background_colour)

```c
// Example: Setting both background and text colour separately
TerminalSetTextColour(FG_WHITE);
TerminalSetBackgroundColour(BG_DARK_BLUE);
TerminalPrint("White text on dark blue background");
```

## xterm-256 Colour Index Reference

The extended palette is accessed by adding a specific index to `EXTENDED_COLOURS_OFFSET`. The `BuildColourSequence_` function uses this offset to generate the `38;5;n` (foreground) or `48;5;n` (background) ANSI sequences.

### Index Ranges

| Index Range | Description | Implementation Context |
| :--- | :--- | :--- |
| 0–15 | Standard ANSI | Maps to basic 16-colour codes (30–37, 40–47). |
| 16–231 | RGB colour Cube | 216 fixed colours for UI elements and gradients. |
| 232–255 | Grayscale Ramp | 24 shades from near-black to near-white. |

### Extended Palette Key Indices (16–231)

The RGB colour cube provides 216 colours (indices 16–231) by varying Red, Green, and Blue through six levels. To use these colours, add the specific index to the `EXTENDED_COLOURS_OFFSET`. This section lists the key indices within the RGB colour cube that have specific constants defined in the module.

| Index | Constant Defined | Visual Description |
| :--- | :--- | :--- |
| **17** | `BG_DARK_BLUE` | Deep navy background |
| **22** | `FG_DARK_GREEN` | Deep forest green foreground |
| **34** | `FG_MEDIUM_GREEN` | Standard interface green foreground |
| **54** | `BG_DARK_PURPLE` | Deep purple/violet background |
| **82** | `FG_BRIGHT_GREEN` | Matrix/Neon green highlight foreground |
| **88** | `FG_DARK_RED` | Deep warning/error red foreground |
| **124** | `FG_MEDIUM_RED` | Mid-tone error state foreground |
| **196** | `FG_BRIGHT_RED` | Critical alert/highlight foreground |

```c
// Example: Esing the constant or the raw index offset
TerminalSetTextColour(FG_BRIGHT_GREEN);
TerminalSetBackgroundColour(EXTENDED_COLOURS_OFFSET + 82);
```

### Grayscale Reference (indices 232–255)

The grayscale ramp provides 24 shades (indices 232–255) ranging from near-black to near-white. To use these shades, add the specific index to the `EXTENDED_COLOURS_OFFSET`. This section lists the grayscale indices that have specific constants defined in the module for UI depth and contrast.

| Index | Constant defined | Visual Description |
| :--- | :--- | :--- |
| **232** | `BG_NEAR_BLACK_1` | Deep Charcoal (Step 1) |
| **234** | `BG_NEAR_BLACK_2` | Darkest Gray (Step 3) |
| **238** | `BG_DARK_GRAY` | Visible Slate Gray (Step 7) |
| **244** | `BG_MID_GRAY` | Neutral Mid-Tone (Step 13) |

```c
// Example: Esing the constant or the raw index offset
TerminalSetTextColour(BG_DARK_GRAY);
TerminalSetBackgroundColour(EXTENDED_COLOURS_OFFSET + 238);
```

## Common Combinations

The user can layer multiple styles and colours simultaneously to build complex frame states. This is used for rendering depth, highlights on moving objects, or distinct UI overlays during scene playback.

### Highlighted Text (Reverse Video)

```c
TerminalSetAttribute(TERM_ATTR_REVERSE);
TerminalPrint("Selected");
TerminalSetAttribute(TERM_ATTR_RESET_REVERSE_MODE);
```

### Bold Red Error Message

```c
TerminalSetAttribute(TERM_ATTR_BOLD);
TerminalSetTextColour(FG_RED);
TerminalPrint("ERROR");
TerminalResetStyle();
```

### Dim Secondary Text

```c
TerminalSetAttribute(TERM_ATTR_DIM);
TerminalSetTextColour(FG_GREEN);
TerminalPrint("Processing...");
TerminalResetStyle();
```

### Underlined Title with Custom Colour

```c
TerminalSetAttribute(TERM_ATTR_UNDERLINE);
TerminalSetTextColour(FG_BRIGHT_GREEN);  // Extended 256-colour
TerminalPrint("Scene: Matrix Rain");
TerminalSetAttribute(TERM_ATTR_RESET_UNDERLINE);
TerminalResetStyle();
```

## Reset and Default Colours

Resetting the terminal returns all styles to their original state. The user should call this after rendering a frame to ensure following frames or text displays normally.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalResetStyle(void)

```c
// Example: Reset after rendering with lots of colours and styles
TerminalResetStyle();
TerminalPrint("Back to normal");
```

## ANSI Escape Codes Reference

The module converts function calls into the raw escape sequences required by the terminal. This table shows exactly which codes are sent for each attribute and colour.

| Style | ANSI Code | Macro | Function |
| :--- | :--- | :--- | :--- |
| Reset all | `ESC[0m` | `ANSI_RESET_STYLE` | `TerminalResetStyle()` |
| Bold | `ESC[1m` | `ANSI_BOLD` | `TerminalSetAttribute(TERM_ATTR_BOLD)` |
| Dim | `ESC[2m` | `ANSI_DIM` | `TerminalSetAttribute(TERM_ATTR_DIM)` |
| Underline | `ESC[4m` | `ANSI_UNDERLINE` | `TerminalSetAttribute(TERM_ATTR_UNDERLINE)` |
| Blink | `ESC[5m` | `ANSI_BLINK` | `TerminalSetAttribute(TERM_ATTR_BLINK)` |
| Reverse | `ESC[7m` | `ANSI_REVERSE_MODE` | `TerminalSetAttribute(TERM_ATTR_REVERSE)` |
| Strikethrough | `ESC[9m` | `ANSI_STRIKETHROUGH` | `TerminalSetAttribute(TERM_ATTR_STRIKE)` |
| **Foreground colour** | `ESC[3Xm` (X=0–7) | — | `TerminalSetTextColour()` |
| **Background colour** | `ESC[4Xm` (X=0–7) | — | `TerminalSetBackgroundColour()` |
| **Extended FG (256)** | `ESC[38;5;Nm` (N=0–255) | — | `TerminalSetTextColour(EXTENDED_COLOURS_OFFSET + N)` |
| **Extended BG (256)** | `ESC[48;5;Nm` (N=0–255) | — | `TerminalSetBackgroundColour(EXTENDED_COLOURS_OFFSET + N)` |

## See Also

Other documents explain how to move the cursor or clear parts of the screen. These functions work together with colour settings to control the full terminal output during real-time animation.

* [Screen & Cursor Control](terminal-screen-cursor.md) — Positioning and clearing
* [Rendering Modes & Graphics](terminal-rendering.md) — Direct blocking output, double-buffering, and geometric primitives
