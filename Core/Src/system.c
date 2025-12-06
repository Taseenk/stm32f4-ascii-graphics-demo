/**
 ******************************************************************************
 * @file           : system.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "system.h"

/* Private Variables ---------------------------------------------------------*/
static Rx_Message_t RxState = {0};		// Holds the state for UART DMA Rx 

/* Functions -----------------------------------------------------------------*/
/**
 * @fn void consoleReceiveInit(UART_HandleTypeDef *huart)
 * @brief Initializes the console reception via UART in DMA mode in circular buffer.
 * Allowing data to be transferred directly into a buffer in the background without 
 * constant CPU intervention. It resets the internal state and starts the DMA transfer process.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 */
void consoleReceiveInit(UART_HandleTypeDef *huart)
{
	// Early exit if the UART handle is NULL
	if (huart == NULL)
		return;

	// Reset structure before starting
	RxState.RxOldPos = 0;

	// Start the reception of UART data in DMA mode
	HAL_UART_Receive_DMA(huart, RxState.RxBuffer, UART_RX_BUFFER_SIZE);

	// Log successful initialization of the DMA transfer
	consolePrint(huart, "DMA has been started\r\n\n");
}

/**
 * @fn void consoleProcessData(UART_HandleTypeDef *huart)
 * @brief Processes received data from the UART circular buffer.
 * Iterates through the buffer to find delimiters (in this case, '\r').
 * When a delimiter is found, it performs associated data processing and
 * updates the buffer's read position.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 */
void consoleProcessData(UART_HandleTypeDef *huart)
{
	// Determine the current write position in the circular buffer based on the remaining DMA transfer count
	// HAL_DMA_GET_COUNTER tells how much space is left off the UART_RX_BUFFER_SIZE
	// Substracting UART_RX_BUFFER_SIZE with HAL_DMA_GET_COUNTER will give the already used space
	uint8_t CurrentPos = UART_RX_BUFFER_SIZE - (uint8_t)__HAL_DMA_GET_COUNTER(huart->hdmarx);

	// Calculate the length of newly received data accounting for buffer wrap-around
	size_t RxReceivedLength = ((CurrentPos - RxState.RxOldPos) + UART_RX_BUFFER_SIZE) % UART_RX_BUFFER_SIZE;

	// Early exit if no new data has been received
	if (RxReceivedLength == 0)
		return;

	// Variables used to calculate and track positions within the circular buffer
	uint8_t offset, bufferIndex;

	// Iterate through the newly received data
	for (offset = 0; offset < RxReceivedLength; offset++) {
		// Calculate the absolute index for the circular RxBuffer
		bufferIndex = (RxState.RxOldPos + offset) % UART_RX_BUFFER_SIZE;

		// Check if the end-of-message ('\r') delimiter is found
		if (RxState.RxBuffer[bufferIndex] == CARRIAGE_RETURN) {
			// Placeholder for message parsing, command execution, etc.
			consolePrint(huart, "Delimeter was found, Received: '");

			// --- DEBUG PRINT LOGIC ---
			uint8_t start_pos = RxState.RxOldPos;
			size_t message_len = offset;
			if (start_pos + message_len < UART_RX_BUFFER_SIZE) {
				// Case 1: Message is contiguous (no wrap-around)
				// Use consolePrintN to print the message directly from the RxBuffer
				consolePrintN(huart, (const char *)&RxState.RxBuffer[start_pos], message_len);
			} else {
				// Case 2: Message wraps around the buffer
				uint8_t first_part_len = UART_RX_BUFFER_SIZE - start_pos;
				uint8_t second_part_len = (uint8_t)message_len - first_part_len;

				// Print the first part (from start_pos to end of buffer)
				consolePrintN(huart, (const char *)&RxState.RxBuffer[start_pos], first_part_len);
				// Print the second part (from start of buffer)
				consolePrintN(huart, (const char *)&RxState.RxBuffer[0], second_part_len);
			}
			consolePrint(huart, "'\r\n\n");
			// --- END DEBUG PRINT LOGIC ---

			// Update and save the new start reading position
			// Wrap the circular buffer back to the start using modulo (remainder)
			RxState.RxOldPos = (bufferIndex + 1) % UART_RX_BUFFER_SIZE;
		}
	}
}
