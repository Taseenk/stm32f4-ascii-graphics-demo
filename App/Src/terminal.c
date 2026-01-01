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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Private Variables ---------------------------------------------------------*/
static uint8_t framebuffer[TERMINAL_BUFFER_SIZE];

/* Private Function Prototypes -----------------------------------------------*/
static void __NormalizeCoordinates(int16_t *col, int16_t *row);
static uint8_t __IsValidPos(int16_t col, int16_t row);
static void __DrawChar(char c, int16_t col, int16_t row);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void __NormalizeCoordinates(int16_t *col, int16_t *row)
 * @brief Internal function to ensure that the provided column and row
 * coordinates are at least 1 for ANSI terminal compatibility.
 * @param col Pointer to the column number (1-based index).
 * @param row Pointer to the row number (1-based index).
 */
static void __NormalizeCoordinates(int16_t *col, int16_t *row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	if (*row == 0)
		*row = 1;
	if (*col == 0)
		*col = 1;
}

/**
 * @fn static uint8_t __IsValidPos(int16_t col, int16_t row)
 * @brief Internal function to check if the provided column and row
 * coordinates are within the valid terminal bounds.
 * @param col The column number (1-based index).
 * @param row The row number (1-based index).
 * @return TRUE if the position is valid, FALSE otherwise.
 */
static uint8_t __IsValidPos(int16_t col, int16_t row)
{
	// Check the maximum screen bounds
	if (row > TERMINAL_HEIGHT || col > TERMINAL_WIDTH)
		return FALSE;

	// Return position is valid
	return TRUE;
}

/**
 * @fn static void __DrawChar(char c, uint16_t col, uint16_t row)
 * @brief Internal function for drawing a single character into the
 * terminal framebuffer at the specified row and column. This is a static function and does NOT
 * perform bounds checking. This function updates the internal framebuffer array but does NOT
 * send any data to the terminal.
 * @param c The character to draw.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
static void __DrawChar(char c, int16_t col, int16_t row)
{
	// Check for minimum bounds (1-based indexing)
	if (row < 1 || col < 1)
		return;

	// Check if the starting position is within the screen boundaries
	if (!__IsValidPos(col, row))
		return;

	// Calculate the 1D array index for the 2D framebuffer
	int16_t index = ((row - 1) * TERMINAL_WIDTH) + (col - 1);

	// Store the character in the framebuffer
	framebuffer[index] = c;
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void TerminalInit(uint8_t cursor)
 * @brief Initializes the terminal by setting cursor visibility, clearing the screen,
 * and clearing the internal framebuffer.
 * @param cursor TRUE to show the cursor, FALSE to hide it.
 */
void TerminalInit(uint8_t cursor)
{
	// Set cursor visibility based on parameter
	if (cursor == TRUE)
		TerminalVisibleCursor();
	else
		TerminalInvisibleCursor();

	// Clear and home the terminal display
	TerminalClearAndHome();

	// Clear the internal framebuffer
	TerminalClearBuffer();

	// Log successful initialization of the terminal
	SerialPrint("DEBUG: Terminal initialized successfully\r\n");
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
 * @fn void TerminalInvisibleCursor(void)
 * @brief Sends the ANSI escape sequence to hide the terminal cursor.
 * This function uses the ANSI_CURSOR_INVISIBLE (ESC[?25l) to make
 * the cursor invisible on the terminal display.
 */
void TerminalInvisibleCursor(void)
{
	// Transmit the ANSI command string for hiding the cursor
	SerialPrint(ANSI_CURSOR_INVISIBLE);
}

/**
 * @fn void TerminalVisibleCursor(void)
 * @brief Sends the ANSI escape sequence to show the terminal cursor.
 * This function uses the ANSI_CURSOR_VISIBLE (ESC[?25h) to make
 * the cursor visible on the terminal display.
 */
void TerminalVisibleCursor(void)
{
	// Transmit the ANSI command string for showing the cursor
	SerialPrint(ANSI_CURSOR_VISIBLE);
}

/**
 * @fn void TerminalSetCursorPos(uint16_t col, uint16_t row)
 * @brief Sends the ANSI escape sequence to explicitly set the terminal cursor position.
 * The ANSI command format is ESC[<row>;<col>H.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
void TerminalSetCursorPos(uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	__NormalizeCoordinates((int16_t *)&col, (int16_t *)&row);

	// Check if the starting position is within the screen boundaries
	if (!__IsValidPos(col, row))
		return;

	// Temporary buffer to hold the ANSI escape sequence (enough for a command like ESC[255;255H)
	char buffer[16];

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
	SerialTransmitDMA((const char *)framebuffer, TERMINAL_BUFFER_SIZE);
}

/**
 * @fn void TerminalDrawChar(char c, uint16_t col, uint16_t row)
 * @brief Draws a single character into the terminal framebuffer at the specified row and column.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to draw.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
void TerminalDrawChar(char c, uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	__NormalizeCoordinates((int16_t *)&col, (int16_t *)&row);

	// Check if the starting position is within the screen boundaries
	if (!__IsValidPos(col, row))
		return;

	// Draw the character in the framebuffer using the internal function
	__DrawChar(c, col, row);
}

/**
 * @fn void TerminalDrawString(const char *str, uint16_t col, uint16_t row)
 * @brief Draws a null-terminated string into the terminal framebuffer at the specified row and column.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param str The string to draw.
 * @param row The target row number (1-based index).
 * @param col The target column number (1-based index).
 */
void TerminalDrawString(const char *str, uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	__NormalizeCoordinates((int16_t *)&col, (int16_t *)&row);

	// Check if the starting position is within the screen boundaries
	if (!__IsValidPos(col, row))
		return;

	// Calculate the length of the string
	uint16_t len = (uint16_t)strlen(str);

	// Ensure the string fits within the terminal width
	// Subtract with 1 because the starting 'col' is included in the string length
	if (col + len - 1 > TERMINAL_WIDTH)
		return;

	// Store the string in the framebuffer using the internal function
	for (uint16_t i = 0; i < len; i++) {
		__DrawChar(str[i], col + i, row);
	}
}

/**
 * @fn void TerminalDrawRect(char c, int16_t col, int16_t row, uint16_t w, uint16_t h)
 * @brief Draws a rectangle outline into the terminal framebuffer using the specified character.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the rectangle.
 * @param col The target starting column number (1-based index).
 * @param row The target starting row number (1-based index).
 * @param w The width of the rectangle in characters.
 * @param h The height of the rectangle in characters.
 */
void TerminalDrawRect(char c, int16_t col, int16_t row, uint16_t w, uint16_t h)
{
	// Make row and column always be 1 or greater for ANSI terminals
	__NormalizeCoordinates((int16_t *)&col, (int16_t *)&row);

	// Quick exit check if the starting position is within the screen boundaries
	// Also check is the rectangle is valid
	if (!__IsValidPos(col, row) || w == 0 || h == 0)
		return;

	// Calculate corner coordinates for x and y
	int16_t right_col = col + (int16_t)w - 1;
	int16_t bottom_row = row + (int16_t)h - 1;

	// Draw Top and Bottom edges
	for (int16_t i = 0; i < (int16_t)w; i++) {
		__DrawChar(c, col + i, row);
		if (bottom_row <= TERMINAL_HEIGHT)
			__DrawChar(c, col + i, bottom_row);
	}

	// Draw Left and Right edges
	for (int16_t j = 0; j < (int16_t)h; j++) {
		__DrawChar(c, col, row + j);
		if (right_col <= TERMINAL_WIDTH)
			__DrawChar(c, right_col, row + j);
	}
}

/**
 * @fn void TerminalFillRect(char c, int16_t col, int16_t row, uint16_t w, uint16_t h)
 * @brief Fills a rectangle area in the terminal framebuffer using the specified character.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for filling the rectangle.
 * @param col The starting column (1-based index).
 * @param row The starting row (1-based index).
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 */
void TerminalFillRect(char c, int16_t col, int16_t row, uint16_t w, uint16_t h)
{
	// Make row and column always be 1 or greater for ANSI terminals
	__NormalizeCoordinates((int16_t *)&col, (int16_t *)&row);

	// Quick exit check if the starting position is within the screen boundaries
	// Also check is the rectangle is valid
	if (!__IsValidPos(col, row) || w == 0 || h == 0)
		return;

	// Calculate corner coordinates for x and y
	int16_t right_col = col + (int16_t)w - 1;
	int16_t bottom_row = row + (int16_t)h - 1;

	// Clamp width and height to terminal boundaries
	w = (right_col > TERMINAL_WIDTH) ? (TERMINAL_WIDTH - col + 1) : w;
	h = (bottom_row > TERMINAL_HEIGHT) ? (TERMINAL_HEIGHT - row + 1) : h;

	// Fill the rectangle area in the framebuffer
	for (int16_t j = 0; j < (int16_t)h; j++) {
		for (int16_t i = 0; i < (int16_t)w; i++) {
			__DrawChar(c, col + i, row + j);
		}
	}
}