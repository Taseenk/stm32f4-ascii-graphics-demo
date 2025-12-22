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


/* Private Variables ---------------------------------------------------------*/
typedef struct{
	volatile uint8_t buffer[UART_BUFFER_SIZE];  // Circular buffer to store incoming UART data via DMA
	uint8_t read_index;                         // Read/Start position in the RxBuffer
}UART_RxDmaBuffer_t;

extern UART_HandleTypeDef huart2;               // UART handle defined in usart
static UART_HandleTypeDef *p_uart = &huart2;    // Private pointer to the UART handle used for terminal I/O
static UART_RxDmaBuffer_t s_uart_rx = {0};      // Holds the state for UART DMA Rx

/* Functions -----------------------------------------------------------------*/
/**
 * @fn uint8_t TerminalPrint(const char *str)
 * @brief Prints a null-terminated string using the private UART handle
 * @param str The string to transmit
 * @return TRUE if transmission was successful, FALSE otherwise.
*/
uint8_t TerminalPrint(const char *str)
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
 * @fn  uint8_t TerminalPrintN(const char *str, uint16_t len)
 * @brief Prints a string of a specified length using the UART handle, avoiding strlen.
 * @param str The string (or buffer) to transmit. Does not need to be null-terminated.
 * @param len The exact number of bytes to transmit.
 * @return TRUE if transmission was successful, FALSE otherwise.
 */
uint8_t TerminalPrintN(const char *str, uint16_t len)
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
 * @fn uint8_t TerminalPrintNewLine(const char *str)
 * @brief Prints a null-terminated string followed by a newline using the private UART handle
 * @param str The string to transmit
 * @return TRUE if transmission was successful, FALSE otherwise.
*/
uint8_t TerminalPrintNewLine(const char *str)
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
 * @fn void TerminalCursorHome(void)
 * @brief Sends the ANSI escape sequence to move the terminal cursor to the home position (1,1).
 * This function uses the ANSI_CURSOR_HOME (ESC[H) to quickly reposition
 * the cursor to the top-left corner of the terminal screen.
*/
void TerminalCursorHome(void)
{
    // Transmit the ANSI command string for moving the cursor to (1,1)
	TerminalPrint( ANSI_CURSOR_HOME);
}

/**
 * @fn void TerminalClearScreen(void)
 * @brief Sends the ANSI escape sequence to clear the entire terminal screen.
 * This function uses the CLEAR_SCREEN (ESC[2J) to erase all content
 * displayed in the terminal window.
 */
void TerminalClearScreen(void)
{
    // Transmit the ANSI command string for clearing the entire screen
	TerminalPrint( ANSI_CLEAR_SCREEN);
}

/**
 * @fn void TerminalClearAndHome(void)
 * @brief Sends the combined ANSI escape sequence to clear the screen AND move the cursor to home (1,1).
 * This function uses the ANSI_CLS_HOME define (ESC[2JESC[H).
 */
void TerminalClearAndHome(void)
{
	// Transmit the combined ANSI command string for clearing the screen and setting the cursor to home (1,1)
	TerminalPrint( ANSI_CLS_HOME);
}

/**
 * @fn void TerminalSetCursorPos(uint8_t row, uint8_t col)
 * @brief Sends the ANSI escape sequence to explicitly set the terminal cursor position.
 * The ANSI command format is ESC[<row>;<col>H.
 * @param row The target row number (1-based index).
 * @param col The target column number (1-based index).
 */
void TerminalSetCursorPos(uint8_t row, uint8_t col)
{
	// Temporary buffer to hold the ANSI escape sequence (enough for a command like ESC[255;255H)
	char buffer[16];

	// Make row and column always be 1 or greater for ANSI terminals
	if (row == 0)
		row = 1;
	if (col == 0)
		col = 1;

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "%u;%uH", (unsigned int)row, (unsigned int)col);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
        return;

	// Transmit the escape sequence via using the precise length calculated by sprintf
    TerminalPrintN(buffer, (uint16_t)len);
}

/**
 * @fn uint8_t TerminalPrintDMA(const char *str)
 * @brief Prints a null-terminated string using DMA (Non-blocking)
 * @param str The string to transmit (Must remain valid until transmission completes)
 * @return TRUE if transmission was started successfully, FALSE otherwise.
*/
uint8_t TerminalPrintDMA(const char *str)
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

/**
 * @fn void TerminalReceiveInit(void)
 * @brief Initializes the console reception via UART in DMA mode in circular buffer.
 * Allowing data to be transferred directly into a buffer in the background without
 * constant CPU intervention. It resets the internal state and starts the DMA transfer process.
 */
void TerminalReceiveInit(void)
{
    // Ensure the UART handle are not NULL
	if (p_uart->Instance == NULL)
		return;

    // Reset structures before starting
	s_uart_rx.read_index = 0;

    // Stop any ongoing DMA reception before starting a new one
    HAL_UART_DMAStop(p_uart);

    // Start the reception of UART data in DMA mode
	HAL_UART_Receive_DMA(p_uart, (uint8_t *)s_uart_rx.buffer, (uint16_t )UART_BUFFER_SIZE);

    // Log successful initialization of the DMA transfer
	TerminalPrint("DEBUG: DMA has been started\r\n\n");
}
