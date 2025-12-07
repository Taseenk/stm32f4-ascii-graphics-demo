/**
 ******************************************************************************
 * @file           : system.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __SYSTEM_H
#define __SYSTEM_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// STM32 specific libraries
#include "stm32f407xx.h"
#include "stm32f4xx_hal_uart.h"

// Standard libraries
#include <stdint.h>
#include <stddef.h>

// User includes
#include "console.h"

/* defines -------------------------------------------------------------------*/
#define UART_RX_BUFFER_SIZE     128      	// Size of the circular buffer for UART Rx (in bytes)
#define CARRIAGE_RETURN         '\r'    	// Delimiter character to signify the end of a message
#define NULL_TERMINATOR			'\0'		// String termination character

/* typedefs ------------------------------------------------------------------*/
typedef struct{
	uint8_t buffer[UART_RX_BUFFER_SIZE];	// Circular buffer to store incoming UART data via DMA
	uint8_t read_index;                     // Read/Start position in the RxBuffer
}UART_RxDmaBuffer_t;

typedef struct{
	char message[UART_RX_BUFFER_SIZE];		// The extracted, null-terminated string.
    size_t length;							// The actual length of the message (excluding the null terminator).	
}Rx_Message_t;

/* function prototypes -------------------------------------------------------*/
void ConsoleReceiveInit(UART_HandleTypeDef *huart); // Initializes UART Rx using DMA in circular mode
void ConsoleProcessData(UART_HandleTypeDef *huart); // Processes data found in the circular buffer

#endif /* __SYSTEM_H */
