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
#define UART_RX_BUFFER_SIZE     64      // Size of the circular buffer for UART Rx (in bytes)
#define CARRIAGE_RETURN         '\r'    // Delimiter character to signify the end of a message

/* typedefs ------------------------------------------------------------------*/
typedef struct{
	uint8_t RxBuffer[UART_RX_BUFFER_SIZE];  // Circular buffer to store incoming UART data via DMA
	uint8_t RxOldPos;                       // Read/Start position in the RxBuffer
}Rx_Message_t;

/* function prototypes -------------------------------------------------------*/
void consoleReceiveInit(UART_HandleTypeDef *huart); // Initializes UART Rx using DMA in circular mode
void consoleProcessData(UART_HandleTypeDef *huart); // Processes data found in the circular buffer

#endif /* __SYSTEM_H */
