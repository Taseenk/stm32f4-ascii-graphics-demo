/**
 ******************************************************************************
 * @file           : console.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __CONSOLE_H
#define __CONSOLE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// STM32 specific libraries
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"

// Standard libraries
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* defines -------------------------------------------------------------------*/
#define NEWLINE_STR			"\r\n"	    // Carriage return + New line characters

// Standard ANSI terminal control codes
#define ANSI_ESC                    "\x1b["         // The standard ANSI Escape sequence initiator
#define CLEAR_SCREEN_CMD            ANSI_ESC "2J"   // Clear the entire screen: ESC [ 2 J
#define ANSI_CLEAR_LINE_CMD         ANSI_ESC "2K"   // Clear the entire current line: ESC [ 2 K
#define ANSI_CURSOR_HOME_CMD        ANSI_ESC "H"    // Move cursor to home (1,1): ESC [ H
#define ANSI_CURSOR_INVISIBLE_CMD   ANSI_ESC "?25l" // Hide the cursor: ESC [ ? 25 l
#define ANSI_CURSOR_VISIBLE_CMD     ANSI_ESC "?25h" // Show the cursor: ESC [ ? 25 h

// ANSI Common combination control codes
#define ANSI_CLS_HOME_CMD       CLEAR_SCREEN_CMD ANSI_CURSOR_HOME_CMD
#define ANSI_HIDE_CLS_HOME_CMD  ANSI_CURSOR_INVISIBLE_CMD ANSI_CLS_HOME_CMD

/* externs -------------------------------------------------------------------*/

/* function prototypes -------------------------------------------------------*/
HAL_StatusTypeDef ConsolePrint(UART_HandleTypeDef *huart, const char *str);
HAL_StatusTypeDef ConsolePrintN(UART_HandleTypeDef *huart, const char *str, size_t len);
HAL_StatusTypeDef ConsolePrintNewLine(UART_HandleTypeDef *huart, const char *str);

HAL_StatusTypeDef ConsoleCursorHome(UART_HandleTypeDef *huart);
HAL_StatusTypeDef ConsoleClearScreen(UART_HandleTypeDef *huart);
HAL_StatusTypeDef ConsoleClearAndHome(UART_HandleTypeDef *huart);
HAL_StatusTypeDef ConsoleSetCursorPos(UART_HandleTypeDef *huart, uint8_t row, uint8_t col);

#endif /* __CONSOLE_H */
