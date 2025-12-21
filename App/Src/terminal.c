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
#include <stdint.h>
#include <string.h>
#include <stdio.h>

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
        return FALSE; 
    }

    // Return Transmission successful
    return TRUE;
}

/**
 * @fn  uint8_t ConsolePrintN(const char *str, uint16_t len)
 * @brief Prints a string of a specified length using the UART handle, avoiding strlen.
 * @param str The string (or buffer) to transmit. Does not need to be null-terminated.
 * @param len The exact number of bytes to transmit.
 * @return TRUE if transmission was successful, FALSE otherwise.
 */
uint8_t ConsolePrintN(const char *str, uint16_t len)
{
    // Ensure both the UART handle and the data pointer are not NULL
	if (p_uart->Instance == NULL || str == NULL)
		return FALSE;

    // Call the HAL UART transmit function to send the string
	// HAL_MAX_DELAY ensures the function BLOCKS until all data is transmitted
	// Will return the status of the HAL UART transmit
    HAL_StatusTypeDef status = HAL_UART_Transmit(p_uart, (uint8_t *)str, len, HAL_MAX_DELAY);

    // Check if the transmission was successful
    if (status != HAL_OK) {
        return FALSE; 
    }

    // Return Transmission successful
    return TRUE;
}

/**
 * @fn uint8_t ConsolePrintNewLine(const char *str)
 * @brief Prints a null-terminated string followed by a newline using the private UART handle
 * @param str The string to transmit
 * @return TRUE if transmission was successful, FALSE otherwise.
*/
uint8_t ConsolePrintNewLine(const char *str)
{
    // Ensure both the UART handle and the data pointer are not NULL
	if (p_uart->Instance == NULL || str == NULL)
		return FALSE;

    // Create a temporary buffer to hold the string with newline
    char buffer[UART_BUFFER_SIZE];

    // Append newline characters to the string
    uint8_t len = snprintf(buffer, sizeof(buffer), "%s" TERMINAL_EOL, str);

    // Check for snprintf errors or buffer overflow
    if (len <= 0 || len >= sizeof(buffer))
        return FALSE;

    // Call the HAL UART transmit function to send the string
	// HAL_MAX_DELAY ensures the function BLOCKS until all data is transmitted
	// Will return the status of the HAL UART transmit
    HAL_StatusTypeDef status = HAL_UART_Transmit(p_uart, (uint8_t *)buffer, len, HAL_MAX_DELAY);

    // Check if the transmission was successful
    if (status != HAL_OK) {
        return FALSE; 
    }

    // Return Transmission successful
    return TRUE;
}

/**
 * @fn uint8_t ConsolePrintDMA(const char *str)
 * @brief Prints a null-terminated string using DMA (Non-blocking)
 * @param str The string to transmit (Must remain valid until transmission completes)
 * @return TRUE if transmission was started successfully, FALSE otherwise.
*/
uint8_t ConsolePrintDMA(const char *str)
{
    // Ensure both the UART handle and the data pointer are not NULL
	if (p_uart->Instance == NULL || str == NULL)
		return FALSE;

    // Ensure the UART is ready for a new transmission
    if (p_uart->gState != HAL_UART_STATE_READY)
        return FALSE;

    // Calculate the number of characters/bytes to send
    uint16_t len = (uint16_t)strlen(str);

    // Start the DMA transmission
    // Will return the status of the UART DMA transmit
    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(p_uart, (uint8_t *)str, len);

    // Check if the DMA started correctly
    if (status != HAL_OK) {
        // Return transmission failed or UART is already busy with another DMA transmission
        return FALSE; 
    }

    // Return Transmission successful
    return TRUE;
}

