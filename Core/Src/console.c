/**
 ******************************************************************************
 * @file           : console.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "console.h"

/* variables -----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
 * @fn void consolePrint(UART_HandleTypeDef *huart, const char *str)
 * @brief Prints a null-terminated string using the specified UART handle.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @param str The string to transmit.
 */
void consolePrint(UART_HandleTypeDef *huart, const char *str)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (huart == NULL || str == NULL)
		return;

	// Calculate the number of characters/bytes to send
	size_t len = strlen(str);

	// Call the HAL UART transmit function to send the string
	// HAL_MAX_DELAY ensures the function BLOCKS until all data is transmitted
	HAL_UART_Transmit(huart, (uint8_t *)str, (uint16_t)len, HAL_MAX_DELAY);
}

/**
 * @fn void consolePrintNewLine(UART_HandleTypeDef *huart, const char *str)
 * @brief Prints a null-terminated string followed by a newline using the
 * specified UART handle.
 * @param huart A pointer to the UART_HandleTypeDef structure
 * @param str The string to transmit
 * @return HAL_StatusTypeDef status of the final HAL_UART_Transmit call for the newline.
 */
HAL_StatusTypeDef consolePrintNewLine(UART_HandleTypeDef *huart, const char *str)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (huart == NULL || str == NULL)
		// Return an error status on invalid input
		return HAL_ERROR;

	// Use the existing consolePrint helper function to send the string
	consolePrint(huart, str);

	// Call the HAL UART transmit function and send newline characters "\r\n" excluding the null terminator
	// Will return the status of the HAL UART transmit
	return HAL_UART_Transmit(huart, (uint8_t *)NEWLINE_STR, sizeof(NEWLINE_STR) - 1, HAL_MAX_DELAY);
}

