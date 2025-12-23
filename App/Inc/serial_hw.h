/**
 ******************************************************************************
 * @file           : serial_hw.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __SERIAL_HW_H
#define __SERIAL_HW_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define UART_BUFFER_SIZE 128 // Size of the buffer for UART Rx/Tx (in bytes)
#define TERMINAL_EOL "\r\n"  // End of line characters for the terminal
#define CARRIAGE_RETURN '\r' // Delimiter character to signify the end of a message
#define NULL_TERMINATOR '\0' // String termination character

/* typedefs ------------------------------------------------------------------*/
typedef struct {
	char message[UART_BUFFER_SIZE]; // The extracted, null-terminated string.
	size_t length;                  // The actual length of the message (excluding the null terminator).
} Rx_Message_t;

/* External Variables --------------------------------------------------------*/
extern Rx_Message_t rx_message; // Structure to hold the incoming rx message

/* Function prototypes -------------------------------------------------------*/
// Basic terminal output functions
uint8_t SerialPrint(const char *str);
uint8_t SerialPrintN(const char *str, uint16_t len);
uint8_t SerialPrintLn(const char *str);

// DMA related functions
uint8_t SerialTransmitDMA(const char *str);
void SerialReceiveInit(void);
void SerialProcessData(void);

#endif /* __SERIAL_HW_H */