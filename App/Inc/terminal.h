/**
 ******************************************************************************
 * @file           : terminal.h
 * @brief          : Header for ANSI/VT100 Terminal Graphics & Control Driver.
 * @details        : This file defines the commands, colors, and functions
 * available to the rest of the application for:
 * - Setting screen dimensions and buffer sizes.
 * - Using ANSI escape codes for cursor and color control.
 * - Calling drawing functions for text and shapes.
 * @note           : All coordinates (col, row) are 1-based (1,1)
 ******************************************************************************
 */

#ifndef __TERMINAL_H
#define __TERMINAL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define TERMINAL_WIDTH 				80                                      // Width of the terminal in characters
#define TERMINAL_HEIGHT 			24                                      // Height of the terminal in characters
#define TERMINAL_STARTING_POS		1										// Starting position for cursor (1,1) in 1-based coordinates
#define TERMINAL_BUFFER_SIZE 		(TERMINAL_WIDTH * TERMINAL_HEIGHT)		// Total size of the terminal buffer
#define EXTENDED_COLOURS_OFFSET 	256                             		// Offset number of extended ANSI colours supported

#define SPACE_CHAR 					' ' 		// Space character used for clearing the terminal buffer

// Standard ANSI terminal control codes
#define ANSI_ESC 				"\x1b["				// The standard ANSI Escape sequence initiator
#define ANSI_CURSOR_HOME 		ANSI_ESC "H"		// Move cursor to home (1,1): ESC [ H
#define ANSI_CLEAR_SCREEN 		ANSI_ESC "2J"       // Clear the entire screen: ESC [ 2 J
#define ANSI_CLEAR_LINE 		ANSI_ESC "2K"		// Clear the entire current line: ESC [ 2 K
#define ANSI_CURSOR_INVISIBLE 	ANSI_ESC "?25l" 	// Hide the cursor: ESC [ ? 25 l
#define ANSI_CURSOR_VISIBLE 	ANSI_ESC "?25h"   	// Show the cursor: ESC [ ? 25 h
#define ANSI_RESET_STYLE 		ANSI_ESC "0m"		// Reset all text formatting and colors: ESC [ 0 m
#define ANSI_REVERSE_MODE 		ANSI_ESC "7m"		// Enable reverse mode (swap foreground and background colors): ESC [ 7 m
#define ANSI_RESET_REVERSE_MODE ANSI_ESC "27m"		// Disable reverse mode: ESC [ 27 m

// ANSI Common combination control codes
#define ANSI_CLS_HOME ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME
#define ANSI_HIDE_CLS_HOME ANSI_CURSOR_INVISIBLE ANSI_CLS_HOME

/* typedefs ------------------------------------------------------------------*/
typedef enum {
	BG_BLACK = 40,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE,
	BG_DEFAULT = 49,

	// Extended colours (offset by 256)
    BG_DARK_BLUE    = EXTENDED_COLOURS_OFFSET + 17,   // xterm-256 index 17  — deep navy
    BG_DARK_PURPLE  = EXTENDED_COLOURS_OFFSET + 54,   // xterm-256 index 54  — deep purple
    BG_NEAR_BLACK_1 = EXTENDED_COLOURS_OFFSET + 232,  // xterm-256 index 232 — just below black (PLUGE -)
    BG_NEAR_BLACK_2 = EXTENDED_COLOURS_OFFSET + 234,  // xterm-256 index 234 — just above black (PLUGE +)
    BG_DARK_GRAY    = EXTENDED_COLOURS_OFFSET + 238,  // xterm-256 index 238 — visible dark grey
} BackgroundColour_t;

typedef enum {
	// Standard Colors (30-39)
	FG_BLACK = 30,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	FG_DEFAULT = 39,

	// Extended Colors (Offset by 256)
	FG_DARK_GREEN = EXTENDED_COLOURS_OFFSET + 22,
	FG_MEDIUM_GREEN = EXTENDED_COLOURS_OFFSET + 34,
	FG_BRIGHT_GREEN = EXTENDED_COLOURS_OFFSET + 82,
	FG_DARK_RED     = EXTENDED_COLOURS_OFFSET + 88,
    FG_MEDIUM_RED   = EXTENDED_COLOURS_OFFSET + 124,
    FG_BRIGHT_RED   = EXTENDED_COLOURS_OFFSET + 196 
} ForegroundColour_t;

/* Function prototypes -------------------------------------------------------*/
// Initialization and frame handling
void TerminalInit(uint8_t cursor);
void TerminalFlush(void);

// ANSI Terminal control functions
void TerminalCursorHome(void);
void TerminalClearScreen(void);
void TerminalClearAndHome(void);

void TerminalSerialPrintString(const char *str, uint16_t col, uint16_t row);

// Cursor functions
void TerminalInvisibleCursor(void);
void TerminalVisibleCursor(void);
void TerminalSetCursorPos(uint16_t col, uint16_t row);

// Style and color control functions
void TerminalResetStyle(void);
void TerminalSetColour(ForegroundColour_t text_colour, BackgroundColour_t background_colour);
void TerminalSetTextColour(ForegroundColour_t text_colour);
void TerminalSetBackgroundColour(BackgroundColour_t background_colour);

// Framebuffer management functions
void TerminalClearBuffer(void);
void TerminalDrawChar(char c, uint16_t col, uint16_t row);
void TerminalDrawString(const char *str, uint16_t col, uint16_t row);

// Graphics primitives functions
void TerminalDrawRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h);
void TerminalFillRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h);
void TerminalDrawLine(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void TerminalDrawTriangle(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TerminalDrawCircle(char c, uint16_t col, uint16_t row, uint16_t r);

#endif /* __TERMINAL_H */