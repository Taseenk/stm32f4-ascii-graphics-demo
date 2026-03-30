/**
 ******************************************************************************
 * @file           : serial_hw.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "serial_hw.h"
#include "shell.h"

// STM32 specific libraries
#include "main.h"

// Standard libraries
#include <stdio.h>
#include <string.h>

/* Private Variables ---------------------------------------------------------*/
typedef struct {
	volatile char buffer[UART_BUFFER_SIZE]; // Circular buffer to store incoming UART data via DMA
	uint8_t read_index;                     // Read/Start position in the RxBuffer
} UART_RxDmaBuffer_t;

static UART_RxDmaBuffer_t s_uart_rx = {0};   // Holds the state for UART DMA Rx
extern UART_HandleTypeDef huart2;            // UART handle defined in usart
static UART_HandleTypeDef *p_uart = &huart2; // Private pointer to the UART handle used for serial I/O
Rx_Message_t rx_message;                     // Global structure to hold the incoming message

/* Functions -----------------------------------------------------------------*/
/**
 * @fn  uint8_t SerialPrintN(const char *str, uint16_t len)
 * @brief Prints a string of a specified length using the UART handle, avoiding strlen.
 * @param str The string (or buffer) to transmit. Does not need to be null-terminated.
 * @param len The exact number of bytes to transmit.
 * @return TRUE if transmission was successful, FALSE otherwise.
 */
uint8_t SerialPrintN(const char *str, uint16_t len)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (p_uart->Instance == NULL || str == NULL)
		return FALSE;

	// Call the HAL UART transmit function to send the string
	// Timeout of 5ms to prevent blocking for too long
	// Will return the status of the HAL UART transmit
	HAL_StatusTypeDef status = HAL_UART_Transmit(p_uart, (uint8_t *)str, len, 5);

	// Check if the transmission was successful
	if (status != HAL_OK)
	{
		return FALSE;
	}

	// Return Transmission successful
	return TRUE;
}

/**
 * @fn uint8_t SerialPrint(const char *str)
 * @brief Prints a null-terminated string using the private UART handle
 * @param str The string to transmit
 * @return TRUE if transmission was successful, FALSE otherwise.
 */
uint8_t SerialPrint(const char *str)
{
	// Ensure the data pointer is not NULL
	if (str == NULL)
		return FALSE;

	// Calculate the number of characters/bytes to send
	uint16_t len = (uint16_t)strlen(str);

	// Use SerialPrintN to send the string and return its result
	return SerialPrintN(str, len);
}

/**
 * @fn uint8_t SerialPrintLn(const char *str)
 * @brief Prints a null-terminated string followed by a newline using the private UART handle
 * @param str The string to transmit
 * @return TRUE if transmission was successful, FALSE otherwise.
 */
uint8_t SerialPrintLn(const char *str)
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
	// Timeout of 5ms to prevent blocking for too long
	// Will return the status of the HAL UART transmit
	HAL_StatusTypeDef status = HAL_UART_Transmit(p_uart, (uint8_t *)buffer, len, 5);

	// Check if the transmission was successful
	if (status != HAL_OK)
	{
		return FALSE;
	}

	// Return Transmission successful
	return TRUE;
}

/**
 * @fn uint8_t SerialTransmitDMA(const char *str)
 * @brief Prints a null-terminated string using DMA (Non-blocking)
 * @param str The string to transmit (Must remain valid until transmission completes)
 * @param len The length of the string to transmit
 * @return TRUE if transmission was started successfully, FALSE otherwise.
 */
uint8_t SerialTransmitDMA(const char *str, uint16_t len)
{
	// Ensure both the UART handle and the data pointer are not NULL
	if (p_uart->Instance == NULL || str == NULL)
		return FALSE;

	// Ensure the UART is ready for a new transmission
	if (p_uart->gState != HAL_UART_STATE_READY)
		return FALSE;

	// Start the DMA transmission
	// Will return the status of the UART DMA transmit
	HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(p_uart, (uint8_t *)str, len);

	// Check if the DMA started correctly
	if (status != HAL_OK)
	{
		// Return transmission failed or UART is already busy with another DMA transmission
		return FALSE;
	}

	// Return Transmission successful
	return TRUE;
}

/**
 * @fn void SerialReceiveInit(void)
 * @brief Initializes the console reception via UART in DMA mode in circular buffer.
 * Allowing data to be transferred directly into a buffer in the background without
 * constant CPU intervention. It resets the internal state and starts the DMA transfer process.
 */
void SerialReceiveInit(void)
{
	// Ensure the UART handle are not NULL
	if (p_uart->Instance == NULL)
		return;

	// Reset structures before starting
	s_uart_rx.read_index = 0;

	// Stop any ongoing DMA reception before starting a new one
	HAL_UART_DMAStop(p_uart);

	// Start the reception of UART data in DMA mode
	HAL_UART_Receive_DMA(p_uart, (uint8_t *)s_uart_rx.buffer, (uint16_t)UART_BUFFER_SIZE);

	// Log successful initialization of the DMA transfer
	SerialPrint("DEBUG: DMA has been started\r\n\n");
}

/**
 * @fn void SerialProcessData(void)
 * @brief Processes received data from the UART circular buffer.
 * Iterates through the buffer to find delimiters (in this case, `'\r'`).
 * When a delimiter is found, it performs associated data processing and
 * updates the buffer's read position.
 */
void SerialProcessData(void)
{
	// Determine the current write position in the circular buffer based on the remaining DMA count
	// HAL_DMA_GET_COUNTER tells how much space is left off the UART_BUFFER_SIZE
	uint16_t dma_counter = __HAL_DMA_GET_COUNTER(p_uart->hdmarx);

	// Substracting UART_BUFFER_SIZE with HAL_DMA_GET_COUNTER will give the already used space
	size_t current_pos = UART_BUFFER_SIZE - dma_counter;

	// Calculate the length of newly received data accounting for buffer wrap-around
	size_t rx_received_length = ((current_pos - s_uart_rx.read_index) + UART_BUFFER_SIZE) % UART_BUFFER_SIZE;

	// Early exit if no new data has been received
	if (rx_received_length == 0)
		return;

	// Variables used to calculate and track positions within the circular buffer
	size_t buffer_index, message_length = 0;
	char current_char;

	// Iterate through the newly received data
	for (int i = 0; i < rx_received_length; i++)
	{
		// Calculate the absolute index for the circular buffer
		buffer_index = (s_uart_rx.read_index + i) % UART_BUFFER_SIZE;

		// The current character being processed from the buffer
		current_char = (char)s_uart_rx.buffer[buffer_index];

		// Check if the message is too long for the buffer to hold
		if (message_length >= (sizeof(rx_message.message) - 1))
		{
			// Log a warning message
			SerialPrint("WARNING: Received message was too long");

			// Update and save the new start reading position
			s_uart_rx.read_index = (buffer_index + 1) % UART_BUFFER_SIZE;

			// Early exit problems with message length
			return;
		}

		// If the first character(s) are spaces, skip them and update the read index to the first non-space character
		if (current_char == ' ' && message_length == 0)
		{
			// Continue to the next character in the buffer
			continue;
		}

		// Handle user spelling mistakes, if a backspace character ('\b') is found
		// Remove the last character from the message buffer (if there is one) and continue to the next character
		if (current_char == BACKSPACE)
		{
			// Handle backspace: if message_length > 0, remove the last character from the message buffer
			if (message_length > 0)
				message_length--;

			// Continue to the next character in the buffer
			continue;
		}

		// Check if the end-of-message ('\r') delimiter is found
		if (current_char == CARRIAGE_RETURN)
		{
			// Null-terminate the message in the buffer at the location of the \r
			rx_message.message[message_length] = NULL_TERMINATOR;

			// Store the length of the received message with null terminator
			rx_message.length = message_length + 1;

			// Update and save the new start reading position
			// Wrap the circular buffer back to the start using modulo
			s_uart_rx.read_index = (buffer_index + 1) % UART_BUFFER_SIZE;

			// Send the received message to the command parser for processing
			ShellCommandParser(rx_message.message);

			// Exit loop safely
			return;
		}

		// Increment the message length and store the character in the message buffer
		// If it no delimiter found, copy the character into the new buffer
		rx_message.message[message_length++] = (char)s_uart_rx.buffer[buffer_index];
	}
}