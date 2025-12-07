/**
 ******************************************************************************
 * @file           : console.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "console.h"

/* Functions -----------------------------------------------------------------*/

/**
 * @fn HAL_StatusTypeDef ConsolePrint(UART_HandleTypeDef *huart, const char *str)
 * @brief Prints a null-terminated string using the specified UART handle.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @param str The string to transmit
 * @return HAL_StatusTypeDef status of the final HAL_UART_Transmit call for the newline.
 */
HAL_StatusTypeDef ConsolePrint(UART_HandleTypeDef *huart, const char *str)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (huart == NULL || str == NULL)
		return HAL_ERROR;

	// Calculate the number of characters/bytes to send
	size_t len = strlen(str);

	// Call the HAL UART transmit function to send the string
	// HAL_MAX_DELAY ensures the function BLOCKS until all data is transmitted
	// Will return the status of the HAL UART transmit
	return HAL_UART_Transmit(huart, (uint8_t *)str, (uint16_t)len, HAL_MAX_DELAY);
}

/**
 * @fn  HAL_StatusTypeDef ConsolePrintN(UART_HandleTypeDef *huart, const char *str, size_t len)
 * @brief Prints a string of a specified length using the UART handle, avoiding strlen.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @param str The string (or buffer) to transmit. Does not need to be null-terminated.
 * @param len The exact number of bytes to transmit.
 * @return HAL_StatusTypeDef status of the final HAL_UART_Transmit call.
 */
HAL_StatusTypeDef ConsolePrintN(UART_HandleTypeDef *huart, const char *str, size_t len)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (huart == NULL || str == NULL)
		return HAL_ERROR;

	// Call the HAL UART transmit function to send the specified number of bytes.
	// HAL_MAX_DELAY ensures the function BLOCKS until all data is transmitted.
	return HAL_UART_Transmit(huart, (uint8_t *)str, (uint16_t)len, HAL_MAX_DELAY);
}

/**
 * @fn HAL_StatusTypeDef ConsolePrintNewLine(UART_HandleTypeDef *huart, const char *str)
 * @brief Prints a null-terminated string followed by a newline using the
 * specified UART handle.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @param str The string to transmit
 * @return HAL_StatusTypeDef status of the final HAL_UART_Transmit call for the newline.
 */
HAL_StatusTypeDef ConsolePrintNewLine(UART_HandleTypeDef *huart, const char *str)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (huart == NULL || str == NULL)
		// Return an error status on invalid input
		return HAL_ERROR;

	// Use the existing ConsolePrint helper function to send the string
	HAL_StatusTypeDef status = ConsolePrint(huart, str);

	// If the string failed to send, return the error
	if (status != HAL_OK)
		return status;

	// Call the HAL UART transmit function and send newline characters "\r\n" excluding the null terminator
	// Will return the status of the HAL UART transmit
	return HAL_UART_Transmit(huart, (uint8_t *)NEWLINE_STR, sizeof(NEWLINE_STR) - 1, HAL_MAX_DELAY);
}

/**
 * @fn HAL_StatusTypeDef ConsoleCursorHome(UART_HandleTypeDef *huart)
 * @brief Sends the ANSI escape sequence to move the terminal cursor to the home position (1,1).
 * This function uses the ANSI_CURSOR_HOME_CMD (ESC[H) to quickly reposition
 * the cursor to the top-left corner of the terminal screen.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @return HAL_StatusTypeDef HAL_OK if the command was successfully transmitted,
 * or HAL_ERROR/HAL_TIMEOUT otherwise.
 */
HAL_StatusTypeDef ConsoleCursorHome(UART_HandleTypeDef *huart)
{
	// Early exit if the UART handle is NULL
	if (huart == NULL)
		return HAL_ERROR;

	// Transmit the ANSI command string for moving the cursor to (1,1)
	return ConsolePrint(huart, ANSI_CURSOR_HOME_CMD);
}

/**
 * @fn HAL_StatusTypeDef ConsoleClearScreen(UART_HandleTypeDef *huart)
 * @brief Sends the ANSI escape sequence to clear the entire terminal screen.
 * This function uses the CLEAR_SCREEN_CMD (ESC[2J) to erase all content
 * displayed in the terminal window.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @return HAL_StatusTypeDef HAL_OK if the command was successfully transmitted,
 * or HAL_ERROR/HAL_TIMEOUT otherwise.
 */
HAL_StatusTypeDef ConsoleClearScreen(UART_HandleTypeDef *huart)
{
	// Early exit if the UART handle is NULL
	if (huart == NULL)
		return HAL_ERROR;

	// Transmit the ANSI command string for clearing the entire screen
	return ConsolePrint(huart, CLEAR_SCREEN_CMD);
}

/**
 * @fn HAL_StatusTypeDef ConsoleClearAndHome(UART_HandleTypeDef *huart)
 * @brief Sends the combined ANSI escape sequence to clear the screen AND move the cursor to home (1,1).
 * This function uses the ANSI_CLS_HOME_CMD define (ESC[2JESC[H).
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @return HAL_StatusTypeDef HAL_OK if the command was successfully transmitted,
 * or HAL_ERROR/HAL_TIMEOUT otherwise.
 */
HAL_StatusTypeDef ConsoleClearAndHome(UART_HandleTypeDef *huart)
{
	// Early exit if the UART handle is NULL
	if (huart == NULL)
		return HAL_ERROR;

	// Transmit the combined ANSI command string for clearing the screen and setting the cursor to home (1,1)
	return ConsolePrint(huart, ANSI_CLS_HOME_CMD);
}

/**
 * @fn HAL_StatusTypeDef ConsoleSetCursorPos(UART_HandleTypeDef *huart, uint8_t row, uint8_t col)
 * @brief Sends the ANSI escape sequence to explicitly set the terminal cursor position.
 * The ANSI command format is ESC[<row>;<col>H.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @param row The target row number (1-based index).
 * @param col The target column number (1-based index).
 * @return HAL_StatusTypeDef HAL_OK if the command was successfully transmitted,
 * or HAL_ERROR/HAL_TIMEOUT otherwise.
 */
HAL_StatusTypeDef ConsoleSetCursorPos(UART_HandleTypeDef *huart, uint8_t row, uint8_t col)
{
	// Early exit if the UART handle is NULL
	if (huart == NULL)
		return HAL_ERROR;

	// Buffer to hold the ANSI escape sequence (enough for a command like ESC[255;255H)
	char buffer[16];

	// Make row and column always be 1 or greater for ANSI terminals
	if (row == 0)
		row = 1;
	if (col == 0)
		col = 1;

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int len = sprintf(buffer, ANSI_ESC "%d;%dH", row, col);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
        return HAL_ERROR;

	// Transmit the escape sequence via using the precise length calculated by sprintf
    return ConsolePrintN(huart, buffer, (size_t)len);
}
