/**
 ******************************************************************************
 * @file           : terminal.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __TERMINAL_H
#define __TERMINAL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define TERMINAL_WIDTH 80                                       // Width of the terminal in characters
#define TERMINAL_HEIGHT 24                                      // Height of the terminal in characters
#define TERMINAL_BUFFER_SIZE (TERMINAL_WIDTH * TERMINAL_HEIGHT) // Total size of the terminal buffer

#define SPACE_CHAR ' ' // Space character used for clearing the terminal buffer

// Standard ANSI terminal control codes
#define ANSI_ESC "\x1b["                      // The standard ANSI Escape sequence initiator
#define ANSI_CURSOR_HOME ANSI_ESC "H"         // Move cursor to home (1,1): ESC [ H
#define ANSI_CLEAR_SCREEN ANSI_ESC "2J"       // Clear the entire screen: ESC [ 2 J
#define ANSI_CLEAR_LINE ANSI_ESC "2K"         // Clear the entire current line: ESC [ 2 K
#define ANSI_CURSOR_INVISIBLE ANSI_ESC "?25l" // Hide the cursor: ESC [ ? 25 l
#define ANSI_CURSOR_VISIBLE ANSI_ESC "?25h"   // Show the cursor: ESC [ ? 25 h

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
    BG_DEFAULT = 49
} BackgroundColour_t;

typedef enum
{
    FG_BLACK = 30,
    FG_RED,
    FG_GREEN,
    FG_YELLOW,
    FG_BLUE,
    FG_MAGENTA,
    FG_CYAN,
    FG_WHITE,
    FG_DEFAULT = 39
} ForegroundColour_t;

/* Function prototypes -------------------------------------------------------*/
// Initialization and frame handling
void TerminalInit(uint8_t cursor);
void TerminalFlush(void);

// ANSI Terminal control functions
void TerminalCursorHome(void);
void TerminalClearScreen(void);
void TerminalClearAndHome(void);
void TerminalInvisibleCursor(void);
void TerminalVisibleCursor(void);
void TerminalSetCursorPos(uint16_t col, uint16_t row);
void TerminalSetTextColour(ForegroundColour_t text_colour);

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