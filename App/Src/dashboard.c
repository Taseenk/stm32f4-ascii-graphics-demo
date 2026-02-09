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
#include <string.h>

/* Private Variables ---------------------------------------------------------*/


/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void DashboardStatusBar(void)
 * @brief Renders the dashboard status bar at the top of the terminal with system
 * information and FPS display.
 */
void DashboardStatusBar(void)
{
    // Static text for the status bar
	const char *system_text = "[SYSTEM: STM32F407VG]";
    const char *dashboard_page = "[MAIN MENU]";
    const char *fps_text = "[FPS: 30]";

    // Calculate the starting column for the dashboard page text to center it
    const uint16_t dashboard_page_col = (TERMINAL_WIDTH / 2) - (strlen(dashboard_page) / 2);

    // Initialize the status bar buffer with spaces and null-terminate it
	char status_bar_buffer[TERMINAL_WIDTH + 1];
    memset(status_bar_buffer, ' ', TERMINAL_WIDTH);
	status_bar_buffer[TERMINAL_WIDTH] = NULL_TERMINATOR;

    // Copy the individual text into the status bar buffer at their respective positions
    memcpy(&status_bar_buffer[SYSTEM_TEXT_POSITION],  system_text,  21);
    memcpy(&status_bar_buffer[dashboard_page_col],  dashboard_page,  11);
    memcpy(&status_bar_buffer[FPS_TEXT_POSITION],  fps_text,  9);

    // Set contrasting colours for the status bar
    TerminalSetColour(FG_BLACK, BG_WHITE);
    
    // Output the entire status bar as a single string
    TerminalSerialPrintString(status_bar_buffer, 1,1);

    // Reset terminal colours for the rest of the screen
    TerminalSetColour(FG_DEFAULT, BG_DEFAULT);
}