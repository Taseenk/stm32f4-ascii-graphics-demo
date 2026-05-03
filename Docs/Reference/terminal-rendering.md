# Rendering Modes & Graphics

The terminal module supports two rendering strategies: direct blocking output, and non-blocking double-buffered frames. The buffered mode provides graphics primitives (lines, rectangles, circles, triangles) for drawing shapes.

## Direct Serial Output (Blocking)

Direct output sends characters and escape sequences immediately to the serial port and blocks until transmission completes.

### Function Reference (Blocking)

Three functions handle direct output. `TerminalPrint()` sends a null-terminated string at the current cursor position. `TerminalPrintN()` sends a fixed number of bytes without requiring a null terminator. `TerminalPrintString()` positions the cursor first, then sends the string.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalPrint(const char *str)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalPrintN(const char *str, uint16_t len)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalPrintString(const char *str, uint16_t col, uint16_t row)

### When to Use (Blocking)

Direct output is appropriate for sparse text updates. Use it when rendering only a few characters per frame or when text must appear immediately without buffering delay. Avoid direct output for full-screen rendering. Sending 1920 characters each frame at 921,600 baud requires roughly 20ms per frame, which blocks the main loop.

## Framebuffer & Double Buffering (Non-blocking)

Double buffering separates rendering from transmission. Code writes to a back buffer. When ready, a buffer swap triggers DMA transmission of the entire frame without blocking the CPU.

### Function Reference (Non-blocking)

Five functions manage buffered rendering. `TerminalIsBufferReady()` checks if the previous DMA transfer is complete. `TerminalBufferFlush()` swaps buffers and initiates transmission. `TerminalBufferClear()` fills the back buffer with spaces. `TerminalBufferDrawChar()` and `TerminalBufferDrawString()` write to the buffer at specified positions.

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: uint8_t TerminalIsBufferReady(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferFlush(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferClear(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferDrawChar(char c, uint16_t col, uint16_t row)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferDrawString(const char *str, uint16_t col, uint16_t row)

!!! warning "Buffer ready check"
    Always call `TerminalIsBufferReady()` before writing to the back buffer. If a DMA transfer is in progress, writing corrupts the transmitted frame. Skip the frame if not ready.

### When to Use (Non-blocking)

Use buffered output for complete scene redraws, animation, and graphical scenes. Buffering eliminates flicker by guaranteeing complete frames only. Avoid buffering for small, latency-sensitive updates (a status line or cursor position that must appear instantly).

## Graphics Primitives

Drawing functions render to the back buffer in buffered mode. Call `TerminalBufferFlush()` to transmit the rendered frame.

### Function Reference

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferDrawRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferFillRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferDrawLine(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferDrawTriangle(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void TerminalBufferDrawCircle(char c, uint16_t col, uint16_t row, uint16_t r)

### Drawing with Characters

Every primitive uses a character `c` as the drawing symbol. Common choices:

| Character | Use Case |
| :--- | :--- |
| `'#'` | Solid boxes, grid backgrounds |
| `'*'` | Highlights, markers |
| `'─'` | Horizontal edges (Unicode) |
| `'│'` | Vertical edges (Unicode) |
| `'┌'`, `'┐'`, `'└'`, `'┘'` | Box corners (Unicode) |
| `'●'` | Circles, dots |
| `' '` (space) | Erasing |

### Line Drawing

Lines are drawn via Bresenham's algorithm, which rasterizes from (x0, y0) to (x1, y1) with no floating-point math. The algorithm automatically detects line orientation and applies the optimized variant.

```c
// Draw a diagonal line from top-left to bottom-right
TerminalBufferDrawLine('#', 1, 1, 80, 24);

// Draw a vertical line down column 40
TerminalBufferDrawLine('│', 40, 1, 40, 24);
```

### Circle Drawing

Circles are drawn via the midpoint circle algorithm. Radius is in character units. A radius of 5 produces a circle roughly 10 columns wide.

```c
// Draw a circle at column 40, row 12, radius 5
TerminalBufferDrawCircle('●', 40, 12, 5);

// Draw a larger circle with a different character
TerminalBufferDrawCircle('*', 30, 10, 8);
```

### Triangle Drawing

Triangles are drawn as three connected line segments.

```c
// Draw a triangle with vertices at (20,5), (50,5), (35,20)
TerminalBufferDrawTriangle('#', 20, 5, 50, 5, 35, 20);
```

## Coordinate Bounds and Clipping

All drawing functions accept 1-based coordinates. Shapes extending outside the 80x24 screen are silently clipped. No error is generated for out-of-bounds operations.

```c
// This circle extends beyond the right edge; right side is clipped
TerminalBufferDrawCircle('*', 75, 12, 10);

// This line crosses off-screen coordinates; only visible portion renders
TerminalBufferDrawLine('─', 0, 10, 50, 10);
```

## Mixing Direct and Buffered Rendering

Direct and buffered modes can coexist if screen regions do not overlap. Designate specific rows for each mode. For example:

- Buffered rendering: rows 1 through 23 (main scene)
- Direct rendering: row 24 (status line only)

Overlapping regions result in flicker or corruption.

```c
// Example: Direct ouput of a status line
TerminalSetCursorPos(1, 24);
TerminalPrint("Status: OK");

// Example: buffered rendering for the main area
if (TerminalIsBufferReady()) {
    TerminalBufferClear();
    // ... render main scene to rows 1-23 ...
    TerminalBufferFlush();
}
```

## See Also

Positioning and clearing the screen provides the canvas for rendering. Text attributes and colour settings enhance the visual output of rendered characters and shapes. Together, these modules form the complete terminal control system for animation and scene construction.

- [Screen & Cursor Control](terminal-screen-cursor.md) - Positioning without rendering
- [Colour & Text Attributes](terminal-colour-attributes.md) - Styling rendered text
