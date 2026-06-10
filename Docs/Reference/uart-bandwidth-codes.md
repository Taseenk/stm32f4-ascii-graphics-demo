# ANSI Code Reference

Escape sequences (ANSI/VT100 codes) control text formatting, colours, and cursor position. Each code consumes bytes from the frame's transmission "budget". This page catalogs the codes used in the project and quantifies their cost.

## Screen Control

Screen control sequences manage the display area without sending character data. Clear operations remove existing content from the screen or from the current line. These sequences are typically used at scene transitions or when switching between rendering modes.

Depending on the scene, clearing the entire screen is a one-time cost per scene, not a per-frame operation. The following table details the most common control sequences and their associated byte sizes.

| Sequence | Bytes | Purpose |
| --- | --- | --- |
| `ESC[2J` | 4 | Clear entire screen |
| `ESC[K` | 3 | Clear to end of line |

!!! example "Screen Clear Implementation"
    ```c
    // Clear the entire screen
    // Byte breakdown:
    // ESC (1) + [ (1) + 2 (1) + J (1) = 4 bytes total
    PRINT ESC[2J
    ```

## Cursor Control

Cursor positioning is fundamental when using a direct/blocking print rendering. Instead of transmitting the entire screen buffer, cursor positioning commands move the terminal cursor to a specific location, allowing partial updates without redrawing the entire screen. The following tables illustrates how different coordinate lengths and command types affect the total byte count sent to the terminal.

### Home Cursor

This command provides a fixed-length shortcut to return the cursor to the top-left origin of the display. Relying on a preset home state instead of defined coordinates, it maximizes byte efficiency.

| Sequence | Bytes | Example |
| --- | --- | --- |
| `ESC[H` | 3 | Home (row 1, col 1) |

### Absolute Cursor Positioning

The command moves the cursor to a specific screen row and column. Byte counts increase as coordinates transition from single digits (1–9) to double digits (10–80).

| Sequence | Bytes | Example |
| --- | --- | --- |
| `ESC[1;1H` | 6 | Row 1, col 1 (single digits) |
| `ESC[12;34H` | 8 | Row 12, col 34 (double digits) |
| `ESC[24;80H` | 8 | Row 24, col 80 |

### Relative Cursor Movement

These commands shift the cursor up, down, left, or right from its current spot. Instead of needing to know the exact screen position, the command only needs to know how many spaces to move in a specific direction.

| Sequence | Bytes | Example |
| --- | --- | --- |
| `ESC[nA` | 4–5 | Move up N rows |
| `ESC[nB` | 4–5 | Move down N rows |
| `ESC[nC` | 4–5 | Move right N columns |
| `ESC[nD` | 4–5 | Move left N columns |

!!! warning "Total Transmission Overhead"
    Keep in mind that these byte counts represent only the overhead for moving the cursor. The additional bytes required to print the actual content or characters at the new destination must also be factored in.

!!! example "Cursor Movement Implementation"
    ```c
    // Move the cursor to row 12, column 34
    // Byte breakdown:
    // ESC (1) + [ (1) + 12 (2) + ; (1) + 34 (2) + H (1) = 8 bytes total
    PRINT ESC[12;34H
    ```

## Text Attributes

Text attributes modify character appearance without affecting screen coordinates or underlying content. The following tables details the byte requirements for text style modifications.

### Standard Text Attributes

These commands turn on specific visual styles for text. Attributes like bold or underline utilize a single digit parameter, making them very efficient.

| Sequence | Bytes | Purpose |
| :---- | :---- | :---- |
| `\x1b[0m` | 4 | Reset All |
| `\x1b[1m` | 4 | Bold |
| `\x1b[2m` | 4 | Dim |
| `\x1b[4m` | 4 | Underline |
| `\x1b[5m` | 4 | Blink |
| `\x1b[7m` | 4 | Reverse |
| `\x1b[9m` | 4 | Strikethrough |

### Selective Attributes Reset

Selective reset commands are used to disable one specific style while leaving other active attributes untouched.

| Sequence | Bytes | Purpose |
| :---- | :---- | :---- |
| `\x1b[22m` | 5 | Reset Bold |
| `\x1b[24m` | 5 | Reset Underline |
| `\x1b[25m` | 5 | Reset Blink |
| `\x1b[27m` | 5 | Reset Reverse |
| `\x1b[29m` | 5 | Reset Strike |

!!! tip "Attribute Persistence"
    Attributes remain active until a reset is sent. Forgetting to reset an attribute can lead to unintended visual artifacts in subsequent frames or text blocks.

!!! example "Attribute Implementation"
    ```c
    // Apply bold
    // Bold: ESC[1m (4 bytes)
    SET text_style TO BOLD

    // Resetting only the bold attribute
    // Byte breakdown:
    // ESC (1) + [ (1) + 22 (2) + m (1) = 5 bytes total
    DISABLE BOLD_ATTRIBUTE
    ```

## Foreground and Background Colours

Colour management operations in the terminal define the visual identity of interface regions. These sequences specify the foreground or background colour before character data is transmitted. The following tables details the byte requirements for the various colour palettes supported by the module.

### Standard 16 Colour Palette Reference

Each command in the standard palette uses a two-digit code to represent a specific colour. Because these codes are always two digits, the total byte count remains fixed at 5 bytes per sequence.

| Sequence | Bytes | Purpose |
| :--- | :--- | :--- |
| `ESC[3Xm` | 5 | Standard Foreground (30–37) |
| `ESC[4Xm` | 5 | Standard Background (40–47) |
| `ESC[39m` | 5 | Default Foreground |
| `ESC[49m` | 5 | Default Background |

| Basic Color | Foreground | Background | Bytes |
| --- | --- | --- | --- |
| Black | `ESC[30m` | `ESC[40m` | 5 |
| Red | `ESC[31m` | `ESC[41m` | 5 |
| Green | `ESC[32m` | `ESC[42m` | 5 |
| Yellow | `ESC[33m` | `ESC[43m` | 5 |
| Blue | `ESC[34m` | `ESC[44m` | 5 |
| Magenta | `ESC[35m` | `ESC[45m` | 5 |
| Cyan | `ESC[36m` | `ESC[46m` | 5 |
| White | `ESC[37m` | `ESC[47m` | 5 |

### Extended 256-Colour Palette

The Extended 256-colour sequences are significantly larger because they require a multi-parameter format to specify the xterm index.

| Sequence | Bytes | Purpose |
| :--- | :--- | :--- |
| `ESC[38;5;Nm` | 7–11 | Extended Foreground (0–255) |
| `ESC[48;5;Nm` | 7–11 | Extended Background (0–255) |

!!! example "Colour Sequence Implementation"
    ```c
    // Set a standard red foreground
    // Byte breakdown:
    // ESC (1) + [ (1) + 3 (1) + 1 (1) + m (1) = 5 bytes total
    SET text_colour TOO RED

    // Set an extended bright green foreground (index 82)
    // Byte breakdown:
    // ESC (1) + [ (1) + 38 (2) + ; (1) + 5 (1) + ; (1) + 82 (2) + m (1) = 10 bytes total
    SET text_color TO BRIGHT_GREEN_INDEX_82
    ```

## Sequence Optimization Strategies

The efficiency of terminal updates depends on the total data volume transmitted. Every byte saved in an escape sequence reduces the time the serial interface is occupied, allowing for higher frame rates and more responsive visuals.

### Using Basic Colours Overextended Palette

Selecting the standard 16 colour palette is a fundamental way to reduce byte counts. Each standard colour command costs only 5 bytes. In contrast, an extended palette command requires between 7 and 11 bytes because the index is sent as an ASCII string where values in the range (10–80) or higher require more digits than those in the (1–9) range.

Evaluate whether a specific shade from the extended palette is necessary for the display or if a standard colour provides sufficient contrast for the UI element.

### Minimize Attribute Changes

Efficiency is gained by grouping characters with identical formatting to prevent sending redundant escape sequences. Because terminal attributes remain active until a reset command or a new attribute is sent, reapplying the same style to every character increases data overhead without changing the visual output.

| Attribute Combination | Individual Command | Combined Command | Total Individual Bytes | **Optimized Total Bytes** |
| :--- | :--- | :--- | :--- | :--- |
| Bold + Red FG | `ESC[1m` `ESC[31m` | `ESC[1:31m` | 9 | **6** |
| Bold + Underline | `ESC[1m` `ESC[4m` | `ESC[1:4m` | 8 | **5** |
| Red FG + Blue BG | `ESC[31m` `ESC[44m` | `ESC[31:44m` | 10 | **7** |
| Bold + Dim + Blue FG | `ESC[1m` `ESC[2m` `ESC[34m` | `ESC[1:2:34m` | 13 | **8** |

!!! example "Optimization Logic"
    ```c
    // Inefficient: Individual formatting for characters A and B
    // ESC[31m + ESC[0m + ESC[31m + ESC[0m = 5 + 4 + 5 + 4 = 18 bytes overhead
    PRINT ESC[31m + "A" + ESC[0m + ESC[31m + "B" + ESC[0m

    // Optimized: Leverages the persistent nature of the red attribute
    // ESC[31m + ESC[0m = 5 + 4 = 9 bytes overhead
    PRINT ESC[31m + "A" + "B" + ESC[0m
    ```

### Combine Text Graphics

Multiple attributes can be applied in a single command by separating the parameters with a separator character within the same escape sequence. This method is more efficient than sending individual commands because it avoids repeating the escape header and the closing character for every style change.

!!! example "Compound Attribute Implementation"
    ```c
    // Inefficient: Two separate commands
    // ESC[1m (4 bytes) + ESC[31m (5 bytes) = 9 bytes
    APPLY STYLE BOLD
    SET Colour TOO RED

    // Optimized: Single combined command
    // ESC[1:31m = 6 bytes
    APPLY COMBINED_STYLE (BOLD AND RED)
    ```

## See Also

For implementation details, coordinate systems, and function definitions, refer to the primary documentation files:

- [Screen & Cursor Control](terminal-screen-cursor.md) – Foundations for screen clearing, cursor positioning logic, and 1-based coordinate indexing.
- [Colour & Text Attributes](terminal-colour-attributes.md) – Comprehensive list of ANSI macros, enums, and styling functions used to apply visual attributes.
- [Rendering Modes & Graphics](terminal-rendering.md) – Detailed explanation of blocking direct output versus non-blocking buffered rendering and geometric primitives.
