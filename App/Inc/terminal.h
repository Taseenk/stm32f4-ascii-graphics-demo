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
// ANSI Terminal control functions
void TerminalCursorHome(void);
void TerminalClearScreen(void);
void TerminalClearAndHome(void);
void TerminalSetCursorPos(uint8_t row, uint8_t col);

// Framebuffer management functions
void TerminalClearBuffer(void);
void TerminalDrawChar(char c, uint8_t row, uint8_t col);

#endif /* __TERMINAL_H */