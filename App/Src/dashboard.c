/**
 ******************************************************************************
 * @file           : dashboard.c
 * @brief          : Source file for the dashboard module of the STM32F4 ASCII
 * graphics demo application, implementing the functions for rendering the dashboard
 * UI, updating the FPS display, and managing menu selection with visual feedback.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "dashboard.h"
#include "scene_manager.h"
#include "serial_hw.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Private Defines -----------------------------------------------------------*/
// Dashboard Status Bar Text
#define SYSTEM_TEXT    "[SYSTEM: STM32F407VG]" // Text to display for system information
#define MAIN_PAGE_TEXT "[MAIN MENU]"           // Text to display for the dashboard page
#define FPS_TEXT       "[FPS: ??]"             // Placeholder text for FPS display

// Dashboard Status Bar Text sizes
#define STATUS_BAR_BUFFER_SIZE                                                                                         \
	(TERMINAL_WIDTH + ANSI_TEXT_LEN +                                                                                  \
	 1)                      // Total size of the status bar buffer, including ANSI escape codes and null terminator
#define ANSI_TEXT_LEN      8 // Length of the ANSI escape codes used for setting and resetting styling
#define SYSTEM_TEXT_LEN    sizeof(SYSTEM_TEXT) - 1    // Length of the system text without null terminator
#define MAIN_PAGE_TEXT_LEN sizeof(MAIN_PAGE_TEXT) - 1 // Length of the main page text without null terminator
#define FPS_TEXT_LEN       sizeof(FPS_TEXT) - 1       // Length of the FPS text without null terminator

// Dashboard Status Bar Text positions
#define SYSTEM_TEXT_POSITION    1  // Starting column for system information text
#define MAIN_PAGE_TEXT_POSITION 34 // Middle of she screen
#define FPS_TEXT_POSITION       70 // Starting column for FPS text
#define FPS_VALUE_POSITION      77 // Position immediately after the FPS label

// Dashboard Main Menu Option Texts and positions
#define HELP_TEXT     " ?      HELP     -  Get help on using the application "
#define AUTO_TEXT     " A      AUTO     -  Automatically go through the scenes "
#define PLAYLIST_TEXT " P      Playlist -  Cycle through curated scenes playlist "
#define INFO_TEXT     " I      INFO     -  About the application and system information "
#define QUIT_TEXT     " Q      QUIT     -  Exit the application "

// Row positions for each Main Menu option
#define OPTIONS_COL_POSITION 10 // Starting column for the dashboard option texts
#define OPTIONS_ROW_OFFSET   3  // Number of rows to space between each dashboard option
#define HELP_ROW_POSITION                                                                                              \
	5 // Starting row for the first dashboard option (help), with subsequent options spaced by OPTIONS_ROW_OFFSET
#define AUTO_ROW_POSITION     (HELP_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define PLAYLIST_ROW_POSITION (AUTO_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define INFO_ROW_POSITION     (PLAYLIST_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define QUIT_ROW_POSITION     (INFO_ROW_POSITION + OPTIONS_ROW_OFFSET)

// Menu selection timing and blinking effect parameters
#define MENU_SELECTION_INTERVAL                                                                                        \
	230 // Number of frames to wait before switching the highlighted menu option in the dashboard
#define SELECTION_BLINK_RATE   12 // Number of frames for each blink phase
#define SELECTION_BLINK_PHASES 2  // Total number of phases in the blink cycle (e.g., 2 phases for on/off blinking)

// Footer position
#define FOOTER_COL 8  // Starting column for the footer text in the dashboard
#define FOOTER_ROW 23 // Row position for the footer text in the dashboard

/* Private Variables ---------------------------------------------------------*/
DashboardPages_t current_page = DASHBOARD_HELP; // Initialize the current dashboard page to the help page

/* Private Function Prototypes -----------------------------------------------*/
static void __ApplyMenuHighlight(uint32_t global_frame);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void __ApplyMenuHighlight(uint32_t global_frame)
 * @brief Applies a blinking highlight effect to the currently selected menu option
 * in the dashboard based on the global frame count to create a visual indication
 * of the active option.
 * @param global_frame The current global frame count used to calculate the blinking state.
 */
static void __ApplyMenuHighlight(uint32_t global_frame)
{
	// Calculate the current blink state based on the global frame count, blink rate, and number of phases
	uint8_t blink_state = (global_frame / SELECTION_BLINK_RATE) % SELECTION_BLINK_PHASES;

	if (blink_state == 0)
		// Set reverse colour mode for the active menu option to create a blinking highlight effect
		SerialPrint(ANSI_REVERSE_MODE);
	else if (blink_state == 1)
		// Reset to default colour mode
		SerialPrint(ANSI_RESET_REVERSE_MODE);
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void MainPageInit(void)
 * @brief Initializes the dashboard by rendering the status bar and setting up
 * any necessary state for the dashboard pages.
 */
void MainPageInit(void)
{
	// Clear the terminal and set up the cursor for the dashboard display
	TerminalClearAndHome();
	TerminalInvisibleCursor();

	// Render the dashboard top status bar with system information and page title
	DashboardHeader();

	// Render the main body of the dashboard with the available options
	DashboardMenuList();

	// Render the dashboard footer with mock navigation instructions and credits
	DashboardFooter();
}

/**
 * @fn void DashboardFPSRefresh(uint32_t fps, uint8_t fps_range)
 * @brief Updates the FPS display in the dashboard status bar with the current
 * frames per second count.
 * @param fps The number of frames rendered in the last second to display as FPS.
 * @param fps_range The maximum FPS value to display before showing "MAX" or a capped value.
 */
void DashboardFPSRefresh(uint32_t fps, uint8_t fps_range)
{
	// Filter out unrealistic fps values that exceed the specified range (e.g., due to a timing issues)
	if (fps > fps_range * 2)
		return;

	// Buffer to hold the formatted FPS value
	char fps_value_buffer[42];

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int fps_value_len = snprintf(
	    fps_value_buffer, sizeof(fps_value_buffer), ANSI_REVERSE_MODE "%-2lu" ANSI_RESET_STYLE, (unsigned long)fps);

	// Check if snprintf failed (len < 0) or if the formatted string exceeded the buffer size
	if (fps_value_len <= 0 || (size_t)fps_value_len >= sizeof(fps_value_buffer))
		return;

	// Print the formatted FPS value at the correct position in the status bar
	TerminalSerialPrintString(fps_value_buffer, FPS_VALUE_POSITION, 1);
}

/**
 * @fn void DashboardHeader(void)
 * @brief Renders a static dashboard status bar at the top of the terminal,
 * displaying system information, the current page, and a placeholder for FPS.
 */
void DashboardHeader(void)
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
	if (status_bar_len <= 0 || (size_t)status_bar_len >= sizeof(status_bar_buffer))
	{
		// If formatting failed, print a fallback status bar without styling to ensure the dashboard is still usable
		TerminalSerialPrintString(data_buffer, 1, 1);
		return;
	}

	// Output the entire status bar as a single string
	TerminalSerialPrintString(status_bar_buffer, 1, 1);
}

/**
 * @fn void DashboardMenuList(void)
 * @brief Renders the main body of the dashboard, displaying the available mock
 * options for the user to interact with.
 */
void DashboardMenuList(void)
{
	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print each dashboard option text at its respective position
	TerminalSerialPrintString(HELP_TEXT, OPTIONS_COL_POSITION, HELP_ROW_POSITION);
	TerminalSerialPrintString(AUTO_TEXT, OPTIONS_COL_POSITION, AUTO_ROW_POSITION);
	TerminalSerialPrintString(PLAYLIST_TEXT, OPTIONS_COL_POSITION, PLAYLIST_ROW_POSITION);
	TerminalSerialPrintString(INFO_TEXT, OPTIONS_COL_POSITION, INFO_ROW_POSITION);
	TerminalSerialPrintString(QUIT_TEXT, OPTIONS_COL_POSITION, QUIT_ROW_POSITION);
}

/**
 * @fn void DashboardFooter(void)
 * @brief Renders the dashboard footer at the bottom of the terminal, displaying
 * mock navigation instructions and credits.
 */
void DashboardFooter(void)
{
	// Footer content to display navigation instructions and credits
	static const char footer_content[] = "[ENTER] Select   [W/S] Navigate   [ESC] Back   (C) 2026 Taseen";

	// Set default colours
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Output the entire footer as a single string
	TerminalSerialPrintString(footer_content, FOOTER_COL, FOOTER_ROW);
}

/**
 * @fn void DashboardMenuSelection(uint32_t global_frame)
 * @brief Applies a blinking highlight effect to the currently selected menu option
 * in the dashboard based on the global frame count to create a visual indication
 * of the active option.
 * @param global_frame The current global frame count used to calculate the blinking state.
 */
void DashboardMenuSelection(uint32_t global_frame)
{
	// Calculate the time spent in the current scene based on the global frame count and the defined interval for scene
	uint16_t time_in_scene = global_frame % MENU_SELECTION_INTERVAL;

	switch (current_page)
	{
		case DASHBOARD_HELP:
			// Apply the blinking highlight effect to the currently selected menu option based on the global frame count
			__ApplyMenuHighlight(global_frame);

			// Print the menu option text with the blinking effect
			TerminalSerialPrintString(HELP_TEXT, OPTIONS_COL_POSITION, HELP_ROW_POSITION);

			// break out of the switch
			break;

		case DASHBOARD_AUTO:
			// Apply the blinking highlight effect to the currently selected menu option based on the global frame count
			__ApplyMenuHighlight(global_frame);

			// Print the menu option text with the blinking effect
			TerminalSerialPrintString(AUTO_TEXT, OPTIONS_COL_POSITION, AUTO_ROW_POSITION);

			// Switch the global system mode to the auto scene
			if (time_in_scene == (MENU_SELECTION_INTERVAL - 1))
			{
				SceneManagerInit();
				system_mode = SYSTEM_STATE_AUTO_SCENE;
				TerminalClearAndHome();
				break;
			}

			// break out of the switch
			break;

		case DASHBOARD_PLAYLIST:
			// Apply the blinking highlight effect to the currently selected menu option based on the global frame count
			__ApplyMenuHighlight(global_frame);

			// Print the menu option text with the blinking effect
			TerminalSerialPrintString(PLAYLIST_TEXT, OPTIONS_COL_POSITION, PLAYLIST_ROW_POSITION);

			// Switch the global system mode to the auto scene
			if (time_in_scene == (MENU_SELECTION_INTERVAL - 1))
			{
				SceneManagerInit();
				system_mode = SYSTEM_STATE_PLAYLIST_SCENE;
				TerminalClearAndHome();
				break;
			}

			// break out of the switch
			break;

		case DASHBOARD_INFO:
			// Apply the blinking highlight effect to the currently selected menu option based on the global frame count
			__ApplyMenuHighlight(global_frame);

			// Print the menu option text with the blinking effect
			TerminalSerialPrintString(INFO_TEXT, OPTIONS_COL_POSITION, INFO_ROW_POSITION);

			// break out of the switch
			break;

		case DASHBOARD_QUIT:
			// Apply the blinking highlight effect to the currently selected menu option based on the global frame count
			__ApplyMenuHighlight(global_frame);

			// Print the menu option text with the blinking effect
			TerminalSerialPrintString(QUIT_TEXT, OPTIONS_COL_POSITION, QUIT_ROW_POSITION);

			// break out of the switch
			break;

		default:
			// break out of the switch
			break;
	}
}