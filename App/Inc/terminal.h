/**
 ******************************************************************************
 * @file           : terminal.h
 * @brief          : ANSI/VT100 & Xterm-256 Terminal Control Driver.
 * Provides an interface for terminal-based UI including:
 * - Screen/Buffer management and coordinate control.
 * - Rich text attributes (Bold, Dim, Underline, Blink, Reverse).
 * - Support for standard 16 and extended 256-color palettes.
 * - Geometric drawing primitives (Lines, Rects, Circles).
 * @note           : All coordinates (col, row) are 1-based (1,1)
 ******************************************************************************
 */

#ifndef __TERMINAL_H
#define __TERMINAL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
// Screen Dimensions
#define TERMINAL_WIDTH          80 // Width of the terminal in characters
#define TERMINAL_HEIGHT         24 // Height of the terminal in characters
#define TERMINAL_STARTING_POS   1  // Starting position for cursor (1,1) in 1-based coordinates
#define TERMINAL_BUFFER_SIZE    (TERMINAL_WIDTH * TERMINAL_HEIGHT) // Total size of the terminal buffer
#define EXTENDED_COLOURS_OFFSET 256                                // Offset number of extended ANSI colours supported

// Escape Sequence Base
#define ANSI_ESC "\x1b[" // The standard ANSI Escape sequence initiator

// Cursor Control
#define ANSI_CURSOR_HOME      ANSI_ESC "H"    // Move cursor to home (1,1): ESC [ H
#define ANSI_CURSOR_INVISIBLE ANSI_ESC "?25l" // Hide the cursor: ESC [ ? 25 l
#define ANSI_CURSOR_VISIBLE   ANSI_ESC "?25h" // Show the cursor: ESC [ ? 25 h

// Screen Control
#define ANSI_CLEAR_SCREEN ANSI_ESC "2J" // Clear the entire screen: ESC [ 2 J
#define ANSI_CLEAR_LINE   ANSI_ESC "2K" // Clear the entire current line: ESC [ 2 K

// Text Attributes
#define ANSI_RESET_STYLE   ANSI_ESC "0m" // Reset all attributes: ESC [ 0 m
#define ANSI_BOLD          ANSI_ESC "1m" // Bold text: ESC [ 1 m
#define ANSI_DIM           ANSI_ESC "2m" // Dim/faint text: ESC [ 2 m
#define ANSI_ITALIC        ANSI_ESC "3m" // Italic text: ESC [ 3 m  (terminal dependent)
#define ANSI_UNDERLINE     ANSI_ESC "4m" // Underline text: ESC [ 4 m
#define ANSI_BLINK         ANSI_ESC "5m" // Slow blink: ESC [ 5 m
#define ANSI_BLINK_FAST    ANSI_ESC "6m" // Fast blink: ESC [ 6 m  (terminal dependent)
#define ANSI_REVERSE_MODE  ANSI_ESC "7m" // Reverse video (swap fg/bg): ESC [ 7 m
#define ANSI_HIDDEN        ANSI_ESC "8m" // Hidden/invisible text: ESC [ 8 m
#define ANSI_STRIKETHROUGH ANSI_ESC "9m" // Strikethrough text: ESC [ 9 m  (terminal dependent)

// Text Attribute Resets
#define ANSI_RESET_BOLD          ANSI_ESC "22m" // Reset bold and dim: ESC [ 22 m
#define ANSI_RESET_ITALIC        ANSI_ESC "23m" // Reset italic: ESC [ 23 m
#define ANSI_RESET_UNDERLINE     ANSI_ESC "24m" // Reset underline: ESC [ 24 m
#define ANSI_RESET_BLINK         ANSI_ESC "25m" // Reset blink: ESC [ 25 m
#define ANSI_RESET_REVERSE_MODE  ANSI_ESC "27m" // Reset reverse video: ESC [ 27 m
#define ANSI_RESET_HIDDEN        ANSI_ESC "28m" // Reset hidden: ESC [ 28 m
#define ANSI_RESET_STRIKETHROUGH ANSI_ESC "29m" // Reset strikethrough: ESC [ 29 m

// Common Combinations
#define ANSI_CLS_HOME      ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME
#define ANSI_HIDE_CLS_HOME ANSI_CURSOR_INVISIBLE ANSI_CLS_HOME

/* typedefs ------------------------------------------------------------------*/
// Enum for terminal text attributes
typedef enum {
	TERM_ATTR_RESET,
	TERM_ATTR_BOLD,
	TERM_ATTR_DIM,
	TERM_ATTR_UNDERLINE,
	TERM_ATTR_BLINK,
	TERM_ATTR_REVERSE,
	TERM_ATTR_STRIKE,

	TERM_ATTR_RESET_BOLD,
	TERM_ATTR_RESET_UNDERLINE,
	TERM_ATTR_RESET_BLINK,
	TERM_ATTR_RESET_REVERSE_MODE,
	TERM_ATTR_RESET_STRIKE
} TerminalAttr_t;

// Enums background colours, including standard ANSI and extended 256-colour options
typedef enum {
	BG_BLACK = 40,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE = 47,
	BG_DEFAULT = 49,

	// Extended colours (xterm-256 palette, offset by EXTENDED_COLOURS_OFFSET)
	BG_DARK_BLUE = EXTENDED_COLOURS_OFFSET + 17,     // xterm-256 index 17  — deep navy
	BG_DARK_PURPLE = EXTENDED_COLOURS_OFFSET + 54,   // xterm-256 index 54  — deep purple
	BG_NEAR_BLACK_1 = EXTENDED_COLOURS_OFFSET + 232, // xterm-256 index 232 — just below black (PLUGE -)
	BG_NEAR_BLACK_2 = EXTENDED_COLOURS_OFFSET + 234, // xterm-256 index 234 — just above black (PLUGE +)
	BG_DARK_GRAY = EXTENDED_COLOURS_OFFSET + 238,    // xterm-256 index 238 — visible dark grey
} BackgroundColour_t;

// Enums foreground colours, including standard ANSI and extended 256-colour options
typedef enum {
	FG_BLACK = 30,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	FG_DEFAULT = 39,

	// Extended Colors (xterm-256 palette, offset by EXTENDED_COLOURS_OFFSET)
	FG_DARK_GREEN = EXTENDED_COLOURS_OFFSET + 22,   // xterm-256 index 22  — deep forest green
	FG_MEDIUM_GREEN = EXTENDED_COLOURS_OFFSET + 34, // xterm-256 index 34  — mid green
	FG_BRIGHT_GREEN = EXTENDED_COLOURS_OFFSET + 82, // xterm-256 index 82  — matrix bright green
	FG_DARK_RED = EXTENDED_COLOURS_OFFSET + 88,     // xterm-256 index 88  — deep red
	FG_MEDIUM_RED = EXTENDED_COLOURS_OFFSET + 124,  // xterm-256 index 124 — mid red
	FG_BRIGHT_RED = EXTENDED_COLOURS_OFFSET + 196,  // xterm-256 index 196 — bright red
} ForegroundColour_t;

/* Function prototypes -------------------------------------------------------*/
// Initialisation
void TerminalInit(uint8_t cursor, uint16_t col, uint16_t row);

// Screen control
void TerminalSetDimensions(uint16_t col, uint16_t row);
void TerminalClearScreen(void);
void TerminalClearAndHome(void);

// Cursor control
void TerminalCursorHome(void);
void TerminalInvisibleCursor(void);
void TerminalVisibleCursor(void);
void TerminalSetCursorPos(uint16_t col, uint16_t row);

// Style and colour control
void TerminalSetAttribute(TerminalAttr_t attr);
void TerminalResetStyle(void);
void TerminalSetColour(ForegroundColour_t text_colour, BackgroundColour_t background_colour);
void TerminalSetTextColour(ForegroundColour_t text_colour);
void TerminalSetBackgroundColour(BackgroundColour_t background_colour);

// Direct serial output
void TerminalPrintN(const char *str, uint16_t len);
void TerminalPrint(const char *str);
void TerminalPrintString(const char *str, uint16_t col, uint16_t row);

// Framebuffer management
void TerminalBufferFlush(void);
void TerminalBufferClear(void);
void TerminalBufferDrawChar(char c, uint16_t col, uint16_t row);
void TerminalBufferDrawString(const char *str, uint16_t col, uint16_t row);

// Graphics primitives
void TerminalBufferDrawRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h);
void TerminalBufferFillRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h);
void TerminalBufferDrawLine(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void TerminalBufferDrawTriangle(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TerminalBufferDrawCircle(char c, uint16_t col, uint16_t row, uint16_t r);

#endif /* __TERMINAL_H */