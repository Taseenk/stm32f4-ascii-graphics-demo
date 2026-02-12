/**
 ******************************************************************************
 * @file           : dashboard.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "dashboard.h"
#include "serial_hw.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <stdio.h>
#include <string.h>

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void DashboardInit(void)
 * @brief Initializes the dashboard by rendering the status bar and setting up
 * any necessary state for the dashboard pages.
 * @param void This function does not take any parameters.
 */
void DashboardInit(void)
{
	// Render the dashboard top status bar with system information and page title
	DashboardStatusBar();

	// Render the main body of the dashboard with the available options
	DashboardMainBody();
}

/**
 * @fn void DashboardFPSUpdater(uint32_t fps_counter)
 * @brief Updates the FPS display in the dashboard status bar with the current
 * frames per second count.
 * @param fps_counter The number of frames rendered in the last second to display as FPS.
 */
void DashboardFPSUpdater(uint32_t fps_counter)
{
	// Buffer to hold the formatted FPS value
	char fps_value_buffer[42];

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int fps_value_len =
	    snprintf(fps_value_buffer, sizeof(fps_value_buffer), ANSI_REVERSE_MODE "%-2lu" ANSI_RESET_STYLE, (unsigned long)fps_counter);

	// Check if snprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (fps_value_len <= 0 || (size_t)fps_value_len >= sizeof(fps_value_buffer))
		return;

	// Print the formatted FPS value at the correct position in the status bar
	TerminalSerialPrintString(fps_value_buffer, FPS_VALUE_POSITION, 1);
}

/**
 * @fn void DashboardStatusBar(void)
 * @brief Renders a static dashboard status bar at the top of the terminal, 
 * displaying system information, the current page, and a placeholder for FPS.
 * @param void This function does not take any parameters.
 */
void DashboardStatusBar(void)
{
	// Buffer to hold the entire status bar buffer, including ANSI escape codes and null terminator
	char status_bar_buffer[STATUS_BAR_BUFFER_SIZE];
	
	// Initialize the data text buffer with spaces and null-terminate it
	char data_buffer[TERMINAL_WIDTH + 1];
	memset(data_buffer, SPACE_CHAR, TERMINAL_WIDTH);
	data_buffer[TERMINAL_WIDTH] = NULL_TERMINATOR;

	// Copy the individual text into the status bar buffer at their respective positions
	memcpy(&data_buffer[SYSTEM_TEXT_POSITION], SYSTEM_TEXT, SYSTEM_TEXT_LEN);
	memcpy(&data_buffer[MAIN_PAGE_TEXT_POSITION], MAIN_PAGE_TEXT, MAIN_PAGE_TEXT_LEN);
	memcpy(&data_buffer[FPS_TEXT_POSITION], FPS_TEXT, FPS_TEXT_LEN);

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int status_bar_len =
	    snprintf(status_bar_buffer, sizeof(status_bar_buffer), ANSI_REVERSE_MODE "%s" ANSI_RESET_STYLE, data_buffer);

	// Check if snprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (status_bar_len <= 0 || (size_t)status_bar_len >= sizeof(status_bar_buffer)) {
		// If formatting failed, print a fallback status bar without styling to ensure the dashboard is still usable
		TerminalSerialPrintString(data_buffer, 1, 1);
		return;
	}

	// Output the entire status bar as a single string
	TerminalSerialPrintString(status_bar_buffer, 1, 1);
}
 
/**
 * @fn void DashboardMainBody(void)
 * @brief Renders the main body of the dashboard, displaying the available mock
 * options for the user to interact with.
 * @param void This function does not take any parameters.
 */
void DashboardMainBody(void)
{
	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print each dashboard option text at its respective position
	TerminalSerialPrintString(HELP_TEXT, OPTIONS_COL_POSITION, HELP_ROW_POSITION);
	TerminalSerialPrintString(SELECT_TEXT, OPTIONS_COL_POSITION, SELECT_ROW_POSITION);
	TerminalSerialPrintString(AUTO_TEXT, OPTIONS_COL_POSITION, AUTO_ROW_POSITION);
	TerminalSerialPrintString(INFO_TEXT, OPTIONS_COL_POSITION, INFO_ROW_POSITION);
	TerminalSerialPrintString(QUIT_TEXT, OPTIONS_COL_POSITION, QUIT_ROW_POSITION);
}