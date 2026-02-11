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

/* Private Variables ---------------------------------------------------------*/

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
	char fps_value_buffer[10];

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int fps_value_len =
	    snprintf(fps_value_buffer, sizeof(fps_value_buffer), "%-2lu", (unsigned long)fps_counter);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (fps_value_len <= 0 || (size_t)fps_value_len >= sizeof(fps_value_buffer))
		return;

	// Set contrasting colours for the status bar
	TerminalSetColour(FG_BLACK, BG_WHITE);

	// Output the entire status bar as a single string
	TerminalSerialPrintString(fps_value_buffer, 77, 1);

	// Reset terminal colours for the rest of the screen
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);
}

/**
 * @fn void DashboardStatusBar(void)
 * @brief Renders a static dashboard status bar at the top of the terminal, 
 * displaying system information, the current page, and a placeholder for FPS.
 * @param void This function does not take any parameters.
 */
void DashboardStatusBar(void)
{
	// Initialize the status bar buffer with spaces and null-terminate it
	char status_bar_buffer[TERMINAL_WIDTH + 1];
	memset(status_bar_buffer, ' ', TERMINAL_WIDTH);
	status_bar_buffer[TERMINAL_WIDTH] = NULL_TERMINATOR;

	// Calculate the starting column for the dashboard page text to center it
	const uint16_t dashboard_page_col = (TERMINAL_WIDTH / 2) - (MAIN_PAGE_TEXT_LEN);

	// Copy the individual text into the status bar buffer at their respective positions
	memcpy(&status_bar_buffer[SYSTEM_TEXT_POSITION], SYSTEM_TEXT, SYSTEM_TEXT_LEN);
	memcpy(&status_bar_buffer[dashboard_page_col], MAIN_PAGE_TEXT, MAIN_PAGE_TEXT_LEN);
	memcpy(&status_bar_buffer[FPS_TEXT_POSITION], FPS_TEXT, FPS_TEXT_LEN);

	// Set contrasting colours for the status bar
	TerminalSetColour(FG_BLACK, BG_WHITE);

	// Output the entire status bar as a single string
	TerminalSerialPrintString(status_bar_buffer, 1, 1);

	// Reset terminal colours for the rest of the screen
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);
}

/**
 * @fn void DashboardMainBody(void)
 * @brief Renders the main body of the dashboard, displaying the available options
 * for the user to interact with.
 * @param void This function does not take any parameters.
 */
void DashboardMainBody(void)
{
	TerminalSerialPrintString(HELP_TEXT, OPTIONS_COL_POSITION, HELP_ROW_POSITION);
	TerminalSerialPrintString(SELECT_TEXT, OPTIONS_COL_POSITION, SELECT_ROW_POSITION);
	TerminalSerialPrintString(AUTO_TEXT, OPTIONS_COL_POSITION, AUTO_ROW_POSITION);
	TerminalSerialPrintString(INFO_TEXT, OPTIONS_COL_POSITION, INFO_ROW_POSITION);
	TerminalSerialPrintString(QUIT_TEXT, OPTIONS_COL_POSITION, QUIT_ROW_POSITION);
}