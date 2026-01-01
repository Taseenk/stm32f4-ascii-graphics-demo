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

// Framebuffer management functions
void TerminalClearBuffer(void);
void TerminalDrawChar(char c, uint16_t col, uint16_t row);
void TerminalDrawString(const char *str, uint16_t col, uint16_t row);
void TerminalDrawRect(char c, int16_t col, int16_t row, uint16_t w, uint16_t h);
void TerminalFillRect(char c, int16_t col, int16_t row, uint16_t w, uint16_t h);

#endif /* __TERMINAL_H */