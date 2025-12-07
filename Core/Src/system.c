/**
 ******************************************************************************
 * @file           : system.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "system.h"

/* Private Variables ---------------------------------------------------------*/
static UART_RxDmaBuffer_t s_uart_rx = {0}; // Holds the state for UART DMA Rx

/* Functions -----------------------------------------------------------------*/
/**
 * @fn void ConsoleReceiveInit(UART_HandleTypeDef *huart)
 * @brief Initializes the console reception via UART in DMA mode in circular buffer.
 * Allowing data to be transferred directly into a buffer in the background without
 * constant CPU intervention. It resets the internal state and starts the DMA transfer process.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 */
void ConsoleReceiveInit(UART_HandleTypeDef *huart)
{
	// Early exit if the UART handle is NULL
	if (huart == NULL)
		return;

	// Reset structures before starting
	s_uart_rx.read_index = 0;

	// Start the reception of UART data in DMA mode
	HAL_UART_Receive_DMA(huart, s_uart_rx.buffer, UART_RX_BUFFER_SIZE);

	// Log successful initialization of the DMA transfer
	ConsolePrint(huart, "DEBUG: DMA has been started\r\n\n");
}

/**
 * @fn void ConsoleProcessData(UART_HandleTypeDef *huart)
 * @brief Processes received data from the UART circular buffer.
 * Iterates through the buffer to find delimiters (in this case, '\r').
 * When a delimiter is found, it performs associated data processing and
 * updates the buffer's read position.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 */
void ConsoleProcessData(UART_HandleTypeDef *huart)
{
	// Determine the current write position in the circular buffer based on the remaining DMA count
	// HAL_DMA_GET_COUNTER tells how much space is left off the UART_RX_BUFFER_SIZE
	// Substracting UART_RX_BUFFER_SIZE with HAL_DMA_GET_COUNTER will give the already used space
	size_t current_pos = UART_RX_BUFFER_SIZE - (uint8_t)__HAL_DMA_GET_COUNTER(huart->hdmarx);

	// Calculate the length of newly received data accounting for buffer wrap-around
	size_t rx_received_length = ((current_pos - s_uart_rx.read_index) + UART_RX_BUFFER_SIZE) % UART_RX_BUFFER_SIZE;

	// Early exit if no new data has been received
	if (rx_received_length == 0)
		return;

	// Structure to hold the incoming message
	Rx_Message_t rx_message;

	// Variables used to calculate and track positions within the circular buffer
	size_t message_length, buffer_index;

	// Iterate through the newly received data
	for (message_length = 0; message_length < rx_received_length; message_length++) {
		// Calculate the absolute index for the circular buffer
		buffer_index = (s_uart_rx.read_index + message_length) % UART_RX_BUFFER_SIZE;

		// Check if the message is too long for the buffer to hold
		if (message_length > sizeof(rx_message.message)) {
			// Log a warning message
			ConsolePrint(huart, "WARNING: Received message was too long");

			// Update and save the new start reading position
			s_uart_rx.read_index = (buffer_index + 1) % UART_RX_BUFFER_SIZE;

			// Early exit problems with message length
			return;
		}

		// Check if the end-of-message ('\r') delimiter is found
		if (s_uart_rx.buffer[buffer_index] == CARRIAGE_RETURN) {
			// Log delimiter successfully found
			ConsolePrint(huart, "DEBUG delimiter was found, Received: ");

			// Null-terminate the message in the buffer
			rx_message.message[message_length] = NULL_TERMINATOR;

			// Store the length of the received message with null terminator
			rx_message.length = message_length + 1;

			// Print the received string
			ConsolePrintNewLine(huart, rx_message.message);

			// Update and save the new start reading position
			// Wrap the circular buffer back to the start using modulo (remainder)
			s_uart_rx.read_index = (buffer_index + 1) % UART_RX_BUFFER_SIZE;

			// Exit loop safely
			return;
		}

		// If it no delimiter found, copy the character into the new buffer
		rx_message.message[message_length] = (char)s_uart_rx.buffer[buffer_index];
	}
}
