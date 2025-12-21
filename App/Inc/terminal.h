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
#define UART_BUFFER_SIZE    128      	// Size of the buffer for UART Rx/Tx (in bytes)
#define TERMINAL_EOL        "\r\n"    // End of line characters for the terminal

// Standard ANSI terminal control codes
#define ANSI_ESC                "\x1b["         // The standard ANSI Escape sequence initiator
#define ANSI_CURSOR_HOME        ANSI_ESC "H"    // Move cursor to home (1,1): ESC [ H
#define ANSI_CLEAR_SCREEN       ANSI_ESC "2J"   // Clear the entire screen: ESC [ 2 J
#define ANSI_CLEAR_LINE         ANSI_ESC "2K"   // Clear the entire current line: ESC [ 2 K
#define ANSI_CURSOR_INVISIBLE   ANSI_ESC "?25l" // Hide the cursor: ESC [ ? 25 l
#define ANSI_CURSOR_VISIBLE     ANSI_ESC "?25h" // Show the cursor: ESC [ ? 25 h

// ANSI Common combination control codes
#define ANSI_CLS_HOME       ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME
#define ANSI_HIDE_CLS_HOME  ANSI_CURSOR_INVISIBLE ANSI_CLS_HOME

/* Externs -------------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
uint8_t TerminalPrint(const char *str);
uint8_t TerminalPrintN(const char *str, uint16_t len);
uint8_t TerminalPrintNewLine(const char *str);
uint8_t TerminalPrintDMA(const char *str);

// ANSI Terminal control functions
void TerminalCursorHome(void);
void TerminalClearScreen(void);
void TerminalClearAndHome(void);
void TerminalSetCursorPos(uint8_t row, uint8_t col);


#endif /* __TERMINAL_H */