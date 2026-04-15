/**
 ******************************************************************************
 * @file           : terminal.c
 * @brief          : ANSI/VT100 Terminal Graphics Driver and Protocol Handler.
 * @details		   : Provides a high-level API for controlling ANSI-compatible
 * terminals (e.g., PuTTY, Tera Term). Core features include:
 * - Cursor and Terminal control using ANSI Escape Sequences.
 * - Primitive drawing (Lines, Rectangles, Triangles, Circles).
 * - Character and String rendering.
 * @note           : All coordinates (col, row) are 1-based to maintain
 * consistency with ANSI/VT100 standards.
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
#include <stdlib.h>
#include <string.h>

/* Private Defines -----------------------------------------------------------*/
#define COLOUR_SEGMENT_MAX_LEN 12 // Enough for "48;5;255\0"
#define COLOUR_BUFFER_SIZE     32 // Enough for "\x1b[38;5;255;48;5;255m\0"
#define CURSOR_BUFFER_SIZE     16 // Enough for"ESC[255;255H"
#define DIMENSIONS_BUFFER_SIZE 20 // enough for "ESC[8;255;255t"

/* Private Variables ---------------------------------------------------------*/
// Internal framebuffer arrays for double buffering
static uint8_t framebuffer_a[TERMINAL_BUFFER_SIZE];
static uint8_t framebuffer_b[TERMINAL_BUFFER_SIZE];

// Pointers to the front and back buffers for double buffering
static uint8_t *back_buffer = framebuffer_a;  // Used for drawing before flushing to the terminal
static uint8_t *front_buffer = framebuffer_b; // Represents the current state of the terminal display

/* Private Function Prototypes -----------------------------------------------*/
static void NormalizeCoordinates_(uint16_t *col, uint16_t *row);
static uint8_t IsValidPos_(uint16_t col, uint16_t row);
static int BuildColourSequence_(char *buffer, size_t buf_size, uint16_t colour, uint8_t is_fg);
static void SwapBuffers_(void);
static void DrawChar_(char c, uint16_t col, uint16_t row);
static void DrawLineHorizontal_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void DrawLineVertical_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void NormalizeCoordinates_(int16_t *col, int16_t *row)
 * @brief Internal function to ensure that the provided column and row
 * coordinates are at least 1 for ANSI terminal compatibility.
 * @param col Pointer to the column number (1-based index).
 * @param row Pointer to the row number (1-based index).
 */
static void NormalizeCoordinates_(uint16_t *col, uint16_t *row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	if (*row == 0)
		*row = 1;
	if (*col == 0)
		*col = 1;
}

/**
 * @fn static uint8_t IsValidPos_(uint16_t col, uint16_t row)
 * @brief Internal function to check if the provided column and row
 * coordinates are within the valid terminal bounds.
 * @param col The column number (1-based index).
 * @param row The row number (1-based index).
 * @return TRUE if the position is valid, FALSE otherwise.
 */
static uint8_t IsValidPos_(uint16_t col, uint16_t row)
{
	// Check the maximum screen bounds
	if (row < 1 || row > TERMINAL_HEIGHT || col < 1 || col > TERMINAL_WIDTH)
		return FALSE;

	// Return position is valid
	return TRUE;
}

/**
 * @fn static int BuildColourSequence_(char *buffer, size_t buf_size, uint16_t colour, uint8_t is_fg)
 * @brief Internal function to build an ANSI escape sequence for setting either foreground or background colour.
 * This function handles both standard ANSI colours and extended 256-colour mode by checking the colour value against
 * the EXTENDED_COLOURS_OFFSET. It formats the appropriate ANSI command into the provided buffer and returns the length
 * of the formatted string.
 * @param buffer The character buffer to write the ANSI escape sequence into.
 * @param buf_size The size of the provided buffer to prevent overflow.
 * @param colour The colour value, which can be a standard ANSI colour or an extended colour (offset by
 * EXTENDED_COLOURS_OFFSET).
 * @param is_fg TRUE if the colour is for the foreground (text), FALSE if it is for the background.
 * @return The length of the formatted ANSI escape sequence string, or a negative value if formatting failed or the
 * buffer was too small.
 */
static int BuildColourSequence_(char *buffer, size_t buf_size, uint16_t colour, uint8_t is_fg)
{
	if (colour >= EXTENDED_COLOURS_OFFSET)
	{
		// Adjust the colour value for extended colours
		colour -= EXTENDED_COLOURS_OFFSET;

		if (is_fg == TRUE)
		{
			// Format based on the extended ANSI forground text colour \x1b[38;5;82m
			return snprintf(buffer, buf_size, "38;5;%d", colour);
		} else
		{
			// Format based on the extended ANSI background colour \x1b[48;5;82m
			return snprintf(buffer, buf_size, "48;5;%d", colour);
		}

	} else
	{
		// Format based on the Standard ANSI background colour \x1b[32m
		return snprintf(buffer, buf_size, "%d", colour);
	}
}

/**
 * @fn static void SwapBuffers_(void)
 * @brief Internal function to swap the front and back buffers for double buffering.
 * This function simply swaps the pointers to the framebuffer arrays, allowing
 * the back buffer to become the new front buffer and vice versa. This is used
 * after drawing operations are complete and the terminal needs updating.
 */
static void SwapBuffers_(void)
{
	uint8_t *temp = front_buffer;
	front_buffer = back_buffer;
	back_buffer = temp;
}

/**
 * @fn static void DrawChar_(char c, uint16_t col, uint16_t row)
 * @brief Internal function for drawing a single character into the
 * terminal back buffer at the specified row and column. This is a static function and does NOT
 * perform bounds checking. This function updates the internal back buffer array but does NOT
 * send any data to the terminal.
 * @param c The character to draw.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
static void DrawChar_(char c, uint16_t col, uint16_t row)
{
	// Check if the starting position is within the screen boundaries
	if (!IsValidPos_(col, row))
		return;

	// Calculate the 1D array index for the 2D back buffer
	uint32_t index = ((row - 1) * TERMINAL_WIDTH) + (col - 1);

	// Store the character in the back buffer
	back_buffer[index] = c;
}

/**
 * @fn static void DrawLineHorizontal_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
 * @brief Internal function for drawing a horizontal line using Bresenham's algorithm.
 * This function draws a line from (x0, y0) to (x1, y1) in the terminal framebuffer
 * using the specified character. This is a static function and does NOT perform bounds checking.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the line.
 * @param x0 The starting column (1-based index).
 * @param y0 The starting row (1-based index).
 * @param x1 The ending column (1-based index).
 * @param y1 The ending row (1-based index).
 */
static void DrawLineHorizontal_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// Make sure to draw from left to right
	if (x0 > x1)
	{
		// Swap x0 and x1
		uint16_t temp_x = x0;
		x0 = x1;
		x1 = temp_x;

		// Swap y0 and y1
		uint16_t temp_y = y0;
		y0 = y1;
		y1 = temp_y;
	}

	// Calculate the horizontal and vertical distances
	int16_t dx = (int16_t)(x1 - x0);
	int16_t dy = (int16_t)(y1 - y0);

	// Determine the step direction for the y-axis
	int16_t y_direction = (dy < 0) ? -1 : 1;
	if (dy < 0)
		dy = (int16_t)-dy;

	// Initialize Bresenham's decision variables
	int16_t decision_parameter = (int16_t)((2 * dy) - dx);
	int16_t y = (int16_t)y0;

	// Iterate across the x-axis
	for (int16_t x = (int16_t)x0; x <= (int16_t)x1; x++)
	{
		// Draw the character at (x, y) in the framebuffer
		DrawChar_(c, x, y);

		// Check if decision variable is positive then move to the next y
		if (decision_parameter > 0)
		{
			y = (int16_t)(y + y_direction);
			// Update error for step in both X and Y
			decision_parameter = (int16_t)(decision_parameter + (2 * (dy - dx)));
		} else
		{
			// Update error for step in X only
			decision_parameter = (int16_t)(decision_parameter + (2 * dy));
		}
	}
}

/**
 * @fn static void DrawLineVertical_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
 * @brief Internal function for drawing a vertical line using Bresenham's algorithm.
 * This function draws a line from (x0, y0) to (x1, y1) in the terminal framebuffer
 * using the specified character. This is a static function and does NOT perform bounds checking.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the line.
 * @param x0 The starting column (1-based index).
 * @param y0 The starting row (1-based index).
 * @param x1 The ending column (1-based index).
 * @param y1 The ending row (1-based index).
 */
static void DrawLineVertical_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// Make sure to draw from top to bottom
	if (y0 > y1)
	{
		// Swap y0 and y1
		uint16_t temp_y = y0;
		y0 = y1;
		y1 = temp_y;

		// Swap x0 and x1
		uint16_t temp_x = x0;
		x0 = x1;
		x1 = temp_x;
	}

	// Calculate the horizontal and vertical distances
	int16_t dx = (int16_t)(x1 - x0);
	int16_t dy = (int16_t)(y1 - y0);

	// Determine the step direction for the x-axis
	int16_t x_direction = (dx < 0) ? -1 : 1;
	if (dx < 0)
		dx = (int16_t)-dx;

	// Initialize Bresenham's decision variables (swapped dy and dx)
	int16_t decision_parameter = (int16_t)((2 * dx) - dy);
	int16_t x = (int16_t)x0;

	// Iterate across the y-axis
	for (int16_t y = (int16_t)y0; y <= (int16_t)y1; y++)
	{
		// Draw the character at (x, y) in the framebuffer
		DrawChar_(c, x, y);

		// Check if decision variable is positive then move to the next x
		if (decision_parameter > 0)
		{
			x = (int16_t)(x + x_direction);
			// Update error for step in both Y and X
			decision_parameter = (int16_t)(decision_parameter + (2 * (dx - dy)));
		} else
		{
			// Update error for step in Y only
			decision_parameter = (int16_t)(decision_parameter + (2 * dx));
		}
	}
}

/**
 * @fn static void DrawLine_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
 * @brief Internal function for drawing a line between two points using Bresenham's algorithm.
 * This function determines whether to draw a horizontal or vertical line based on
 * the slope and updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the line.
 * @param x0 The starting column (1-based index).
 * @param y0 The starting row (1-based index).
 * @param x1 The ending column (1-based index).
 * @param y1 The ending row (1-based index).
 */
static void DrawLine_(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// Calculate absolute horizontal and vertical distances
	int16_t absolute_x = (int16_t)abs((int)x1 - (int)x0);
	int16_t absolute_y = (int16_t)abs((int)y1 - (int)y0);

	// Choose to draw either a horizontal or vertical line based on greater distance between points
	if (absolute_x > absolute_y)
		// The line is "flat" (more horizontal than vertical)
		DrawLineHorizontal_(c, x0, y0, x1, y1);
	else
		// Line is "steep" (more vertical than horizontal)
		DrawLineVertical_(c, x0, y0, x1, y1);
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void TerminalInit(uint8_t cursor, uint16_t col, uint16_t row)
 * @brief Initializes the terminal by setting cursor visibility, clearing the screen,
 * and clearing the internal framebuffer.
 * @param cursor TRUE to show the cursor, FALSE to hide it.
 * @param col The number of columns for the terminal (1-based index).
 * @param row The number of rows for the terminal (1-based index).
 */
void TerminalInit(uint8_t cursor, uint16_t col, uint16_t row)
{
	// Set the terminal dimensions based on provided parameters
	TerminalSetDimensions(col, row);

	// Set cursor visibility based on parameter
	if (cursor == TRUE)
		TerminalVisibleCursor();
	else
		TerminalInvisibleCursor();

	// Clear and home the terminal display
	TerminalClearAndHome();

	// Clear the internal framebuffer
	TerminalBufferClear();

	// Reset terminal styling to default
	TerminalResetStyle();
}

/**
 * @fn void TerminalSetDimensions(uint16_t col, uint16_t row)
 * @brief Sets the terminal dimensions by sending the appropriate ANSI escape sequence.
 * This function formats the ANSI command to resize the terminal window to the specified
 * number of columns and rows. It ensures that the provided dimensions are valid and
 * sends the command via the serial interface.
 * @param col The desired number of columns for the terminal (1-based index).
 * @param row The desired number of rows for the terminal (1-based index).
 */
void TerminalSetDimensions(uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Temporary buffer to hold the ANSI escape sequence (enough for ESC[8;255;255t)
	char buffer[DIMENSIONS_BUFFER_SIZE];

	// Format the xterm resize sequence: ESC [ 8 ; <rows> ; <cols> t
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "8;%u;%ut", (unsigned int)row, (unsigned int)col);

	// Check if snprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
		return;

	// Transmit the escape sequence using the precise length calculated by snprintf
	SerialPrintN(buffer, (uint16_t)len);
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
 * The ANSI command format is `ESC[<row>;<col>H`.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
void TerminalSetCursorPos(uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Check if the starting position is within the screen boundaries
	if (!IsValidPos_(col, row))
		return;

	// Temporary buffer to hold the ANSI escape sequence (enough for a command like ESC[255;255H)
	char buffer[CURSOR_BUFFER_SIZE];

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
 * @fn void TerminalSetAttribute(TerminalAttr_t attribute)
 * @brief Sets a specific text attribute (e.g., bold, underline) in the terminal using ANSI escape sequences.
 * This function takes a TerminalAttr_t enum value and sends the corresponding ANSI command to enable that attribute.
 * @param attribute The text attribute to set, specified as a value from the TerminalAttr_t enum.
 */
void TerminalSetAttribute(TerminalAttr_t attribute)
{
	switch (attribute)
	{
		case TERM_ATTR_RESET:
			SerialPrint(ANSI_RESET_STYLE);
			break;
		case TERM_ATTR_BOLD:
			SerialPrint(ANSI_BOLD);
			break;
		case TERM_ATTR_DIM:
			SerialPrint(ANSI_DIM);
			break;
		case TERM_ATTR_UNDERLINE:
			SerialPrint(ANSI_UNDERLINE);
			break;
		case TERM_ATTR_BLINK:
			SerialPrint(ANSI_BLINK);
			break;
		case TERM_ATTR_REVERSE:
			SerialPrint(ANSI_REVERSE_MODE);
			break;
		case TERM_ATTR_STRIKE:
			SerialPrint(ANSI_STRIKETHROUGH);
			break;
		case TERM_ATTR_RESET_BOLD:
			SerialPrint(ANSI_RESET_BOLD);
			break;

		case TERM_ATTR_RESET_UNDERLINE:
			SerialPrint(ANSI_RESET_UNDERLINE);
			break;
		case TERM_ATTR_RESET_BLINK:
			SerialPrint(ANSI_RESET_BLINK);
			break;
		case TERM_ATTR_RESET_REVERSE_MODE:
			SerialPrint(ANSI_RESET_REVERSE_MODE);
			break;
		case TERM_ATTR_RESET_STRIKE:
			SerialPrint(ANSI_RESET_STRIKETHROUGH);
			break;
		default:
			break;
	}
}

/**
 * @fn void TerminalResetStyle(void)
 * @brief Sends the ANSI escape sequence to reset all terminal styles to default.
 * This function uses the ANSI_RESET_STYLE (ESC[0m) to clear any applied
 * text formatting, colours, or other styles and return to the default terminal appearance.
 */
void TerminalResetStyle(void)
{
	// Transmit the ANSI command string for resetting all styles to default
	SerialPrint(ANSI_RESET_STYLE);
}

/**
 * @fn void TerminalSetColour(ForegroundColour_t text_colour, BackgroundColour_t background_colour)
 * @brief Sets the terminal text (foreground) and background colours using ANSI escape sequences.
 * This function sends the appropriate ANSI command to change both text and background colours.
 * @param text_colour The desired foreground colour from the ForegroundColour_t enum.
 * @param background_colour The desired background colour from the BackgroundColour_t enum.
 */
void TerminalSetColour(ForegroundColour_t text_colour, BackgroundColour_t background_colour)
{
	// Temporary buffers to hold the ANSI escape sequences for foreground and background colours
	char buffer[COLOUR_BUFFER_SIZE];
	char fg_seg[COLOUR_SEGMENT_MAX_LEN];
	char bg_seg[COLOUR_SEGMENT_MAX_LEN];

	// Build the ANSI escape sequences for foreground and background colours
	BuildColourSequence_(fg_seg, sizeof(fg_seg), (int)text_colour, TRUE);
	BuildColourSequence_(bg_seg, sizeof(bg_seg), (int)background_colour, FALSE);

	// Combine the foreground and background escape sequence into a single ANSI escape sequence
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "%s;%sm", fg_seg, bg_seg);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
		return;

	SerialPrintN(buffer, (uint16_t)len);
}

/**
 * @fn void TerminalSetTextColour(ForegroundColour_t text_colour)
 * @brief Sets the terminal text (foreground) colour using ANSI escape sequences.
 * This function sends the appropriate ANSI command to change only the text colour.
 * @param text_colour The desired foreground colour from the ForegroundColour_t enum.
 */
void TerminalSetTextColour(ForegroundColour_t text_colour)
{
	// Temporary buffers to hold the ANSI escape sequences for foreground
	char buffer[COLOUR_BUFFER_SIZE];
	char fg_seg[COLOUR_SEGMENT_MAX_LEN];

	// Build the ANSI escape sequences for foreground
	BuildColourSequence_(fg_seg, sizeof(fg_seg), (int)text_colour, TRUE);

	// Format the escape sequence to set the foreground text colour
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "%sm", fg_seg);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
		return;

	// Transmit the escape sequence
	SerialPrintN(buffer, (uint16_t)len);
}

/**
 * @fn void TerminalSetBackgroundColour(BackgroundColour_t background_colour)
 * @brief Sets the terminal background colour using ANSI escape sequences.
 * This function sends the appropriate ANSI command to change only the background colour.
 * @param background_colour The desired background colour from the BackgroundColour_t enum.
 */
void TerminalSetBackgroundColour(BackgroundColour_t background_colour)
{
	// Temporary buffers to hold the ANSI escape sequences for background colours
	char buffer[COLOUR_BUFFER_SIZE];
	char bg_seg[COLOUR_SEGMENT_MAX_LEN];

	// Build the ANSI escape sequences for background colours
	BuildColourSequence_(bg_seg, sizeof(bg_seg), (int)background_colour, FALSE);

	// Format the escape sequence to set the foreground text colour
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "%sm", bg_seg);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
		return;

	// Transmit the escape sequence
	SerialPrintN(buffer, (uint16_t)len);
}

/**
 * @fn void TerminalPrintN(const char *str, uint16_t len)
 * @brief Transmits exactly len bytes of str to the terminal via serial_hw.
 * This is the single gateway for sending pre-formatted data (escape sequences,
 * raw characters) to the hardware. All application modules must use this
 * function instead of calling serial_hw directly.
 * @param str Pointer to the data to transmit. Does not need to be null-terminated.
 * @param len Number of bytes to transmit.
 */
void TerminalPrintN(const char *str, uint16_t len)
{
	SerialPrintN(str, len);
}

/**
 * @fn void TerminalPrint(const char *str)
 * @brief Transmits a null-terminated string to the terminal via serial_hw.
 * Convenience wrapper around TerminalPrintN for callers that have a
 * null-terminated string and do not want to compute its length manually.
 * @param str Null-terminated string to transmit.
 */
void TerminalPrint(const char *str)
{
	SerialPrint(str);
}

/**
 * @fn void TerminalPrintString(const char *str, uint16_t col, uint16_t row)
 * @brief DDraws a string directly to the terminal at the specified row and column.
 * @param str The string to draw.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
void TerminalPrintString(const char *str, uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Check if the starting position is within the screen boundaries
	if (!IsValidPos_(col, row))
		return;

	// Temporary buffer to hold the ANSI escape sequence (enough for a command like ESC[255;255H) and a string for the
	// whole terminal width
	char buffer[TERMINAL_WIDTH + 16];

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "%u;%uH%s", (unsigned int)row, (unsigned int)col, str);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the terminal size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
		return;

	// Transmit the escape sequence via using the precise length calculated by sprintf
	SerialPrintN(buffer, (uint16_t)len);
}

/**
 * @fn uint8_t TerminalIsBufferReady(void)
 * @brief Checks if the terminal is ready to receive new data by verifying that the serial transmission is not busy.
 * This function calls SerialIsTransmitBusy() to determine if the previous transmission has completed and the
 * terminal can accept new data.
 * @return TRUE if the terminal is ready for new data, FALSE if it is still busy transmitting.
 */
uint8_t TerminalIsBufferReady(void)
{
	return SerialIsTransmitBusy();
}

/**
 * @fn void TerminalBufferFlush(void)
 * @brief Flushes the internal terminal back buffer to the terminal display. This function first checks if the terminal
 * is ready to receive new data by calling TerminalIsBufferReady(). If the terminal is busy, it waits until it becomes
 * ready. Once ready, it swaps the front and back buffers and sends the entire contents of the new front buffer to the
 * terminal using DMA for efficient transmission. After this function is called, the back buffer can be safely modified
 * for the next frame of drawing operations.
 */
void TerminalBufferFlush(void)
{
	// Wait until the terminal is ready to receive new data before flushing the buffer
	while (TerminalIsBufferReady() == FALSE)
	{
	}

	// Swap the front and back buffers
	SwapBuffers_();

	// Move the cursor home and send the entire front buffer via DMA
	TerminalCursorHome();
	SerialTransmitDMA((const char *)front_buffer, TERMINAL_BUFFER_SIZE);
}

/**
 * @fn void TerminalBufferClear(void)
 * @brief Clears the internal terminal back buffer by filling it with space characters.
 * This function does NOT send any data to the terminal; it only updates the internal back buffer.
 */
void TerminalBufferClear(void)
{
	// Fill the entire back buffer with space characters
	memset(back_buffer, ' ', TERMINAL_BUFFER_SIZE);
}

/**
 * @fn void TerminalBufferDrawChar(char c, uint16_t col, uint16_t row)
 * @brief Draws a single character into the terminal framebuffer at the specified row and column.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to draw.
 * @param col The target column number (1-based index).
 * @param row The target row number (1-based index).
 */
void TerminalBufferDrawChar(char c, uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Check if the starting position is within the screen boundaries
	if (!IsValidPos_(col, row))
		return;

	// Draw the character in the framebuffer using the internal function
	DrawChar_(c, col, row);
}

/**
 * @fn void TerminalBufferDrawString(const char *str, uint16_t col, uint16_t row)
 * @brief Draws a null-terminated string into the terminal framebuffer at the specified row and column.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param str The string to draw.
 * @param row The target row number (1-based index).
 * @param col The target column number (1-based index).
 */
void TerminalBufferDrawString(const char *str, uint16_t col, uint16_t row)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Check if the starting position is within the screen boundaries
	if (!IsValidPos_(col, row))
		return;

	// Calculate the length of the string
	uint16_t len = (uint16_t)strlen(str);

	// Ensure the string fits within the terminal width
	// Subtract with 1 because the starting 'col' is included in the string length
	if (col + len - 1 > TERMINAL_WIDTH)
		return;

	// Store the string in the framebuffer using the internal function
	for (uint16_t i = 0; i < len; i++)
	{
		DrawChar_(str[i], col + i, row);
	}
}

/**
 * @fn void TerminalBufferDrawRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h)
 * @brief Draws a rectangle outline into the terminal framebuffer using the specified character.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the rectangle.
 * @param col The target starting column number (1-based index).
 * @param row The target starting row number (1-based index).
 * @param w The width of the rectangle in characters.
 * @param h The height of the rectangle in characters.
 */
void TerminalBufferDrawRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Quick exit check if the starting position is within the screen boundaries
	// Also check is the rectangle is valid
	if (!IsValidPos_(col, row) || w == 0 || h == 0)
		return;

	// Calculate corner coordinates for x and y
	uint32_t right_col = col + (uint32_t)w - 1;
	uint32_t bottom_row = row + (uint32_t)h - 1;

	// Draw Top and Bottom edges
	for (uint16_t i = 0; i < w; i++)
	{
		DrawChar_(c, col + i, row);
		if (bottom_row <= TERMINAL_HEIGHT)
			DrawChar_(c, col + i, bottom_row);
	}

	// Draw Left and Right edges
	for (uint16_t j = 0; j < h; j++)
	{
		DrawChar_(c, col, row + j);
		if (right_col <= TERMINAL_WIDTH)
			DrawChar_(c, right_col, row + j);
	}
}

/**
 * @fn void TerminalBufferFillRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h)
 * @brief Fills a rectangle area in the terminal framebuffer using the specified character.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for filling the rectangle.
 * @param col The starting column (1-based index).
 * @param row The starting row (1-based index).
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 */
void TerminalBufferFillRect(char c, uint16_t col, uint16_t row, uint16_t w, uint16_t h)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Quick exit check if the starting position is within the screen boundaries
	// Also check is the rectangle is valid
	if (!IsValidPos_(col, row) || w == 0 || h == 0)
		return;

	// Calculate corner coordinates for x and y
	uint32_t right_col = col + (uint32_t)w - 1;
	uint32_t bottom_row = row + (uint32_t)h - 1;

	// Clamp width and height to terminal boundaries
	w = (right_col > TERMINAL_WIDTH) ? (TERMINAL_WIDTH - col + 1) : w;
	h = (bottom_row > TERMINAL_HEIGHT) ? (TERMINAL_HEIGHT - row + 1) : h;

	// Fill the rectangle area in the framebuffer
	for (uint16_t j = 0; j < h; j++)
	{
		for (uint16_t i = 0; i < w; i++)
		{
			DrawChar_(c, col + i, row + j);
		}
	}
}

/**
 * @fn void TerminalBufferDrawLine(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
 * @brief Draws a line between two points in the terminal framebuffer using Bresenham's algorithm.
 * This function determines whether to draw a horizontal or vertical line based on
 * the slope and updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the line.
 * @param x0 The starting column (1-based index).
 * @param y0 The starting row (1-based index).
 * @param x1 The ending column (1-based index).
 * @param y1 The ending row (1-based index).
 */
void TerminalBufferDrawLine(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&x0, &y0);
	NormalizeCoordinates_(&x1, &y1);

	// Check if the line is not off the screen
	if ((x0 > TERMINAL_WIDTH && x1 > TERMINAL_WIDTH) || (y0 > TERMINAL_HEIGHT && y1 > TERMINAL_HEIGHT))
		return;

	// Draw the line using the internal function
	DrawLine_(c, x0, y0, x1, y1);
}

/**
 * @fn void TerminalBufferDrawTriangle(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t
 * y2)
 * @brief Draws a triangle outline between three points in the terminal framebuffer using Bresenham's algorithm.
 * This function updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the triangle.
 * @param x0 The column of Vertex A (1-based index).
 * @param y0 The row of Vertex A (1-based index).
 * @param x1 The column of Vertex B (1-based index).
 * @param y1 The row of Vertex B (1-based index).
 * @param x2 The column of Vertex C (1-based index).
 * @param y2 The row of Vertex C (1-based index).
 */
void TerminalBufferDrawTriangle(char c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&x0, &y0);
	NormalizeCoordinates_(&x1, &y1);
	NormalizeCoordinates_(&x2, &y2);

	// Check if the triangle is not off the screen
	if ((x0 > TERMINAL_WIDTH && x1 > TERMINAL_WIDTH && x2 > TERMINAL_WIDTH) ||
	    (y0 > TERMINAL_HEIGHT && y1 > TERMINAL_HEIGHT && y2 > TERMINAL_HEIGHT))
		return;

	// Draw the line Vertex A to Vertex B
	DrawLine_(c, x0, y0, x1, y1);

	// Draw the line Vertex B to Vertex C
	DrawLine_(c, x1, y1, x2, y2);

	// Draw the line Vertex C back to Vertex A
	DrawLine_(c, x2, y2, x0, y0);
}

/**
 * @fn void TerminalBufferDrawCircle(char c, uint16_t col, uint16_t row, uint16_t r)
 * @brief Draws a line between two points in the terminal framebuffer using Bresenham's algorithm.
 * This function determines whether to draw a horizontal or vertical line based on
 * the slope and updates the internal framebuffer array but does NOT send any data to the terminal.
 * @param c The character to use for drawing the circle.
 * @param col The target center column number (1-based index).
 * @param row The target center row number (1-based index).
 * @param r The radius of the circle in characters.
 */
void TerminalBufferDrawCircle(char c, uint16_t col, uint16_t row, uint16_t r)
{
	// Make row and column always be 1 or greater for ANSI terminals
	NormalizeCoordinates_(&col, &row);

	// Aspect ratio adjustment for terminal character cells that are 2:1
	const uint16_t aspect_ratio = 2;

	// Quick exit check if the circle is valid
	if (r == 0)
		return;

	// Initialize starting point
	int16_t x = 0;
	int16_t y = (int16_t)r;

	// Initial midpoint probability
	int16_t midpoint_probability = (int16_t)(3 - (2 * (int16_t)r));

	while (y >= x)
	{
		// --- Quadrant 1 (Upper-Right) ---
		DrawChar_(c, (col + (x * aspect_ratio)), (row - y));
		DrawChar_(c, (col + (y * aspect_ratio)), (row - x));

		// --- Quadrant 4 (Lower-Right) ---
		DrawChar_(c, (col + (x * aspect_ratio)), (row + y));
		DrawChar_(c, (col + (y * aspect_ratio)), (row + x));

		// --- Quadrant 3 (Lower-Left) ---
		DrawChar_(c, (col - (x * aspect_ratio)), (row + y));
		DrawChar_(c, (col - (y * aspect_ratio)), (row + x));

		// --- Quadrant 2 (Upper-Left) ---
		DrawChar_(c, (col - (x * aspect_ratio)), (row - y));
		DrawChar_(c, (col - (y * aspect_ratio)), (row - x));

		// Update the decision variable and coordinates
		if (midpoint_probability < 0)
		{
			midpoint_probability = (int16_t)(midpoint_probability + (4 * x) + 6);
		} else
		{
			midpoint_probability = (int16_t)(midpoint_probability + (4 * (x - y)) + 10);
			y--;
		}

		// Increment the column
		x++;
	}
}