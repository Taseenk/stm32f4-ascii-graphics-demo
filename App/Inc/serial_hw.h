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

/* typedefs ------------------------------------------------------------------*/
typedef struct {
	char message[UART_BUFFER_SIZE]; // The extracted, null-terminated string.
	size_t length;                  // The actual length of the message (excluding the null terminator).
} Rx_Message_t;

/* External Variables --------------------------------------------------------*/
extern Rx_Message_t rx_message; // Structure to hold the incoming rx message

/* Function prototypes -------------------------------------------------------*/
// Initialization & Management
void SerialReceiveInit(void);

// TX (Transmit) Functions
uint8_t SerialTransmitDMA(const char *str, uint16_t len);
uint8_t SerialPrintLn(const char *str);
uint8_t SerialPrint(const char *str);
uint8_t SerialPrintN(const char *str, uint16_t len);

// Peripheral Status
uint8_t SerialIsTransmitBusy(void);
uint8_t SerialHasError(void);

// RX (Receive) Processing
void SerialProcessData(void);

#endif /* __SERIAL_HW_H */