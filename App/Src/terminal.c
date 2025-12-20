/**
 ******************************************************************************
 * @file           : terminal.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "terminal.h"

// STM32 specific libraries
#include "stm32f4xx_hal_uart.h"

// Standard libraries
#include <string.h>

/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
static UART_HandleTypeDef *p_uart = &huart2;

/* Functions -----------------------------------------------------------------*/
/**
 * @fn uint8_t ConsolePrint(const char *str)
 * @brief Prints a null-terminated string using the private UART handle
 * @param str The string to transmit
 * @return TRUE if transmission was successful, FALSE otherwise.
*/
uint8_t ConsolePrint(const char *str)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (p_uart->Instance == NULL || str == NULL)
		return FALSE;

	// Calculate the number of characters/bytes to send
	uint16_t len = (uint16_t)strlen(str);

    // Call the HAL UART transmit function to send the string
	// HAL_MAX_DELAY ensures the function BLOCKS until all data is transmitted
	// Will return the status of the HAL UART transmit
    HAL_StatusTypeDef status = HAL_UART_Transmit(p_uart, (uint8_t *)str, len, HAL_MAX_DELAY);

    // Check if the transmission was successful
    if (status != HAL_OK) {
        // Return transmission failed
        return FALSE; 
    }

    // Return Transmission successful
    return TRUE;
}
