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
static uint16_t input_row = 16;

/* Private Function Prototypes -----------------------------------------------*/
static void __RowOverflow(uint8_t required_space);
static void __InputCommand(uint16_t row);
static void __CommandError(char *input_buffer, ShellError_t error_type);
static void __HelpCommand(void);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn void __RowOverflow(uint8_t required_space)
 * @brief Checks if there is enough space left in the terminal to print a new
 * message without overflowing the bottom of the screen. If there is not enough
 * space, it clears the terminal and resets the input row to prevent overflow.
 * @param required_space The number of rows required to print the new message and any additional helper text.
 */
static void __RowOverflow(uint8_t required_space)
{
	// Check if the new messages will overflow the terminal height
	if ((input_row + required_space) >= TERMINAL_HEIGHT) {
		// Clear the terminal and reset the input row
		TerminalClearAndHome();
		input_row = 1;
	}
}

/**
 * @fn void __RowOverflow(uint8_t required_space)
 * @brief Checks if there is enough space left in the terminal to print a new
 * message without overflowing the bottom of the screen. If there is not enough
 * space, it clears the terminal and resets the input row to prevent overflow.
 * @param required_space The number of rows required to print the new message and any additional helper text.
 */
static void __InputCommand(uint16_t row)
{
	// Print the input prompt at the current input row
	TerminalSerialPrintString(INPUT_TEXT, SHELL_COL_POSITION, row);

	// Move the cursor to the input position and enable it for user input
	TerminalSetCursorPos(INPUT_COL_POSITION, row);
	TerminalVisibleCursor();
}

/**
 * @fn void __CommandError(char *input_buffer, ShellError_t error_type)
 * @brief Handles the logic for displaying error messages in the dashboard shell
 * when the user inputs an unrecognized command or invalid parameters.
 * @param input_buffer The buffer containing the user input string that caused the error.
 * @param error_type An integer representing the type of error (e.g., 1 for unrecognized command, 2 for invalid
 * parameters).
 */
static void __CommandError(char *input_buffer, ShellError_t error_type)
{
	// Check if there is enough space to print the error message and helper text
	// if not clear the screen and reset the input row
	__RowOverflow(4);

	// Set red text colour for error messages
	TerminalSetColour(FG_RED, BG_DEFAULT);

	// Print the appropriate error message based on the error type
	TerminalSetCursorPos(SHELL_COL_POSITION, input_row);
	switch (error_type) {
		case SHELL_ERROR_BAD_COMMAND:
			SerialPrint("'");
			SerialPrint(input_buffer);
			SerialPrintLn("' is not recognized as a command.");
			// break out of the switch
			break;

		case SHELL_ERROR_INVALID_PARAM:
			SerialPrint("Invalid parameter: ");
			SerialPrintLn(input_buffer);
			// break out of the switch
			break;

		default:
			SerialPrintLn("An unknown error occurred.");
			// break out of the switch
			break;
	}

	// Set default colours for helper text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	input_row += 2;
	TerminalSetCursorPos(SHELL_COL_POSITION, input_row);
	SerialPrintLn("Type 'demo.exe --help' for options.");

	// Increment the input row and prompt the user for the next command
	input_row += 2;
	__InputCommand(input_row);
}

/**
 * @fn void __HelpCommand(void)
 * @brief Handles the logic for displaying help information about the available
 * command flags when the user types "demo.exe --help" in the dashboard shell.
 * @param void This function does not take any parameters.
 */
static void __HelpCommand(void)
{
	// List of available flags and their descriptions to print when the user types "demo.exe --help"
	const char *flags[] = {"  --help   :\tShow help", "  --auto   :\tAuto Mode", "  --select :\tSelect Mode"};

	const uint8_t flags_count = sizeof(flags) / sizeof(flags[0]);

	// Check if there is enough space to print the error message and helper text
	// if not clear the screen and reset the input row
	__RowOverflow(6);

	// Set cyan text colour for the help command output
	TerminalSetColour(FG_CYAN, BG_DEFAULT);
	TerminalSerialPrintString("Available flags for demo.exe:", SHELL_COL_POSITION, input_row);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print each flag and its description at its respective position
	for (int i = 0; i < flags_count; i++) {
		input_row++;
		TerminalSerialPrintString(flags[i], SHELL_COL_POSITION, input_row);
	}

	// Increment the input row and prompt the user for the next command
	input_row++;
	__InputCommand(++input_row);
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void DashboardShellInit(void)
 * @brief Initializes the dashboard shell by rendering the initial
 * boot sequence with system information and a prompt for user input.
 * @param void This function does not take any parameters.
 */
void DashboardShellInit(void)
{
	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Render the system header
	TerminalSerialPrintString(NAME_TEXT, SHELL_COL_POSITION, NAME_ROW_POSITION);
	TerminalSerialPrintString(COPYRIGHT_TEXT, SHELL_COL_POSITION, COPYRIGHT_ROW_POSITION);
	HAL_Delay(150);

	// Render the hardware specs with delays to simulate a boot sequence
	TerminalSerialPrintString(CPU_TEXT, SHELL_COL_POSITION, CPU_ROW_POSITION);
	HAL_Delay(300);
	TerminalSerialPrintString(SRAM_TEXT, SHELL_COL_POSITION, SRAM_ROW_POSITION);
	HAL_Delay(1000);
	TerminalSerialPrintString(FLASH_TEXT, SHELL_COL_POSITION, FLASH_ROW_POSITION);
	HAL_Delay(300);

	// Render the peripheral checks with delays to simulate a boot sequence
	TerminalSerialPrintString(DMA_TEXT, SHELL_COL_POSITION, DMA_ROW_POSITION);
	HAL_Delay(450);
	TerminalSerialPrintString(UART_TEXT, SHELL_COL_POSITION, UART_ROW_POSITION);
	HAL_Delay(300);
	TerminalSerialPrintString(TERMINAL_TEXT, SHELL_COL_POSITION, TERMINAL_ROW_POSITION);
	HAL_Delay(1000);

	// Render the system ready message
	TerminalSerialPrintString(READY_TEXT, SHELL_COL_POSITION, READY_ROW_POSITION);

	// Render the interaction prompt
	TerminalSerialPrintString(HINT_TEXT, SHELL_COL_POSITION, HINT_ROW_POSITION);
	HAL_Delay(200);
	__InputCommand(INPUT_ROW_POSITION);
}

/**
 * @fn void MainPageInit(void)
 * @brief Initializes the dashboard by rendering the status bar and setting up
 * any necessary state for the dashboard pages.
 * @param void This function does not take any parameters.
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
 * @fn void DashboardShellCommandParser(char *rx_buffer)
 * @brief Parses the user input from the dashboard shell, extracting the command
 * and its arguments to determine which action to take based on the input.
 * @param rx_buffer The buffer containing the user input string to parse.
 */
void DashboardShellCommandParser(char *rx_buffer)
{
	// Calculate the length of the received buffer
	size_t buffer_len = strlen(rx_buffer);

	// Early exit if buffer is null or invalid (e.g., empty string)
	if (rx_buffer == NULL)
		return;

	// Convert the received buffer to lowercase for comparison
	for (size_t i = 0; i < buffer_len; i++) {
		if (rx_buffer[i] >= UPPERCASE_A && rx_buffer[i] <= UPPERCASE_Z) {
			rx_buffer[i] += LOWERCASE_OFFSET;
		}
	}

	// Check if the received command starts with the expected command text
	if (strncmp(rx_buffer, COMMAND_TEXT, COMMAND_TEXT_LEN) != 0) {
		// Command does not match, display an error message
		__CommandError(rx_buffer, SHELL_ERROR_BAD_COMMAND);
		return;
	}

	// Extract the arguments from the buffer after the command and parse them
	char *arg = strtok(rx_buffer + COMMAND_TEXT_LEN, ARGUMENT_DELIMITER);

	// If there are no arguments provided, go to the default scene
	if (arg == NULL) {
		// TODO: Implement default scene loading when no arguments are provided
		g_system_mode = SYSTEM_STATE_DASHBOARD;
		MainPageInit();
		return;
	}

	// Parse the arguments and go to the appropriate scene based on the provided flag
	while (arg != NULL) {
		if (strcmp(arg, ARG_HELP_TEXT) == 0 || strcmp(arg, ARG_SHORT_HELP_TEXT) == 0) {
			// Call the help command function to display the available flags and their descriptions
			__HelpCommand();
			return;
		} else if (strcmp(arg, ARG_AUTO_TEXT) == 0 || strcmp(arg, ARG_SHORT_AUTO_TEXT) == 0) {
			// TODO: Implement auto scene loading when the '--auto or '-a' flag gets used
			g_system_mode = SYSTEM_STATE_DASHBOARD;
			MainPageInit();
			return;
		} else if (strcmp(arg, ARG_SELECT_TEXT) == 0 || strcmp(arg, ARG_SHORT_SELECT_TEXT) == 0) {
			// TODO: Implement select scene loading when the '--select or '-s' flag gets used
			g_system_mode = SYSTEM_STATE_DASHBOARD;
			MainPageInit();
			return;
		} else {
			// Argument does not match, display an error message
			__CommandError(arg, SHELL_ERROR_INVALID_PARAM);
			return;
		}

		// Move to the next argument in the buffer
		arg = strtok(NULL, ARGUMENT_DELIMITER);
	}
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
	int fps_value_len = snprintf(fps_value_buffer, sizeof(fps_value_buffer), ANSI_REVERSE_MODE "%-2lu" ANSI_RESET_STYLE,
	                             (unsigned long)fps);

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
 * @param void This function does not take any parameters.
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
	if (status_bar_len <= 0 || (size_t)status_bar_len >= sizeof(status_bar_buffer)) {
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
 * @param void This function does not take any parameters.
 */
void DashboardMenuList(void)
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

/**
 * @fn void DashboardFooter(void)
 * @brief Renders the dashboard footer at the bottom of the terminal, displaying
 * mock navigation instructions and credits.
 * @param void This function does not take any parameters.
 */
void DashboardFooter(void)
{
	// Set default colours
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Output the entire footer as a single string
	TerminalSerialPrintString(FOOTER_TEXT, FOOTER_COL_POSITION, FOOTER_ROW_POSITION);
}