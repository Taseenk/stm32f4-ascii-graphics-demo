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

/* Externs -------------------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
uint8_t ConsolePrint(const char *str);
uint8_t ConsolePrintN(const char *str, uint16_t len);
uint8_t ConsolePrintNewLine(const char *str);
uint8_t ConsolePrintDMA(const char *str);


#endif /* __TERMINAL_H */