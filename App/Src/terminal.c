/**
 ******************************************************************************
 * @file           : terminal.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "terminal.h"
#include "serial_hw.h"

// STM32 libraries
#include "main.h"
#include "stm32f407xx.h"

// Standard libraries
#include <stdio.h>
#include <string.h>

/* Private Variables ---------------------------------------------------------*/
uint8_t framebuffer[TERMINAL_BUFFER_SIZE];

/* Functions -----------------------------------------------------------------*/
/**
 * @fn void TerminalCursorHome(void)
 * @brief Sends the ANSI escape sequence to move the terminal cursor to the home position (1,1).
 * This function uses the ANSI_CURSOR_HOME (ESC[H) to quickly reposition
 * the cursor to the top-left corner of the terminal screen.
 */
void TerminalCursorHome(void)
{
	// Transmit the ANSI command string for moving the cursor to (1,1)
	SerialPrint(ANSI_CURSOR_HOME);
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
	SerialPrint(ANSI_CLEAR_SCREEN);
}

/**
 * @fn void TerminalClearAndHome(void)
 * @brief Sends the combined ANSI escape sequence to clear the screen AND move the cursor to home (1,1).
 * This function uses the ANSI_CLS_HOME define (ESC[2JESC[H).
 */
void TerminalClearAndHome(void)
{
	// Transmit the combined ANSI command string for clearing the screen and setting the cursor to home (1,1)
	SerialPrint(ANSI_CLS_HOME);
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
	SerialPrintN(buffer, (uint16_t)len);
}

/**
 * @fn void TerminalClearBuffer(void)
 * @brief Clears the internal terminal framebuffer by filling it with space characters.
 * This function does NOT send any data to the terminal; it only updates the internal framebuffer.
 */
void TerminalClearBuffer(void)
{
	// Fill the entire framebuffer with space characters
	memset(framebuffer, SPACE_CHAR, TERMINAL_BUFFER_SIZE);
}

/**
 * @fn void TerminalDrawChar(char c, uint8_t row, uint8_t col)
 * @brief Draws a single character into the terminal framebuffer at the specified row and column.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to draw.
 * @param row The target row number (1-based index).
 * @param col The target column number (1-based index).
 */
void TerminalDrawChar(char c, uint8_t row, uint8_t col)
{
	// Make row and column always be 1 or greater for ANSI terminals
	if (row == 0)
		row = 1;
	if (col == 0)
		col = 1;

	// Check the maximum screen bounds
	if (row > TERMINAL_HEIGHT || col > TERMINAL_WIDTH)
		return;

	// Calculate the 1D array index for the 2D framebuffer
	uint16_t index = ((uint16_t)(row - 1) * TERMINAL_WIDTH) + (col - 1);

	// Store the character in the framebuffer
	framebuffer[index] = (uint8_t)c;
}

/**
 * @fn void TerminalDrawString(const char *str, uint8_t row, uint8_t col)
 * @brief Draws a null-terminated string into the terminal framebuffer at the specified row and column.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param str The string to draw.
 * @param row The target row number (1-based index).
 * @param col The target column number (1-based index).
 */
void TerminalDrawString(const char *str, uint8_t row, uint8_t col)
{
	// Make row and column always be 1 or greater for ANSI terminals
	if (row == 0)
		row = 1;
	if (col == 0)
		col = 1;

	// Check the maximum screen bounds
	if (row > TERMINAL_HEIGHT || col > TERMINAL_WIDTH)
		return;

	// Calculate the length of the string
	uint16_t len = (uint16_t)strlen(str);

	// Ensure the string fits within the terminal width
	if (col + len > TERMINAL_WIDTH)
		return;

	// Calculate the 1D array index for the 2D framebuffer
	uint16_t index = ((uint16_t)(row - 1) * TERMINAL_WIDTH) + (col - 1);

	// Store the string in the framebuffer
	for (int i = 0; i < len; i++) {
		framebuffer[index + i] = str[i];
	}
}

/**
 * @fn void TerminalFlush(void)
 * @brief Sends the entire terminal framebuffer to the terminal display.
 * This function moves the cursor to the home position before transmitting
 * the framebuffer content using DMA for non-blocking transmission.
 */
void TerminalFlush(void)
{
	// Move cursor to home before flushing the framebuffer
	TerminalCursorHome();

	// Send the entire framebuffer to the terminal
	SerialTransmitDMA((const char*)framebuffer, TERMINAL_BUFFER_SIZE);
}