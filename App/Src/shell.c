/**
 ******************************************************************************
 * @file           : shell.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "shell.h"
#include "dashboard.h"
#include "serial_hw.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <string.h>

/* Private Variables ---------------------------------------------------------*/
static uint16_t input_row = 16;			// Row position for the user input prompt in the CLI shell

/* Private Function Prototypes -----------------------------------------------*/
static void __HelpCommand(void);
static void __RowOverflow(uint8_t required_space);
static void __InputCommand(uint16_t row);
static void __CommandError(char *input_buffer, ShellError_t error_type);
static void __DashboardPageLauncher(DashboardPages_t page);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn void __HelpCommand(void)
 * @brief Handles the logic for displaying help information about the available
 * command flags when the user types "demo.exe --help" in the dashboard shell.
 * @param void This function does not take any parameters.
 */
static void __HelpCommand(void)
{
	// List of available flags and their descriptions to print when the user types "demo.exe --help"
	const char *flags[] = {
		"  -h, --help     :\tShow help", 
		"  -p, --playlist :\tPlaylist Mode", 
		"  -a, --auto     :\tAuto Mode"
	};

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
 * @fn void __InputCommand(uint16_t row)
 * @brief Displays the input prompt at the specified row and sets the cursor
 * position for user input in the CLI shell.
 * @param row The row position where the input prompt should be displayed.
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
 * @brief Handles the logic for displaying error messages in the CLI shell
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
 * @fn void __DashboardPageLauncher(DashboardPages_t page)
 * @brief Handles the logic for launching different dashboard pages based on the
 * provided page parameter. It sets the system mode to the dashboard and initializes
 * the main page with the specified dashboard page.
 * @param page An integer representing the specific dashboard page to launch (e.g., playlist, auto mode).
 */
static void __DashboardPageLauncher(DashboardPages_t page)
{
	system_mode = SYSTEM_STATE_DASHBOARD;
	current_page = page;
	MainPageInit();
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void ShellInit(void)
 * @brief Initializes the CLI shell by rendering the initial
 * boot sequence with system information and a prompt for user input.
 * @param void This function does not take any parameters.
 */
void ShellInit(void)
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
 * @fn void ShellCommandParser(char *rx_buffer)
 * @brief Parses the user input from the CLI shell, extracting the command
 * and its arguments to determine which action to take based on the input.
 * @param rx_buffer The buffer containing the user input string to parse.
 */
void ShellCommandParser(char *rx_buffer)
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
		// Call the dashboard page launcher function with the playlist page parameter to go to the playlist scene
		__DashboardPageLauncher(DASHBOARD_PLAYLIST);
		return;
	}

	// Parse the arguments and go to the appropriate scene based on the provided flag
	while (arg != NULL) {
		if (strcmp(arg, ARG_HELP_TEXT) == 0 || strcmp(arg, ARG_SHORT_HELP_TEXT) == 0) {
			// Call the help command function to display the available flags and their descriptions
			__HelpCommand();
			return;
		} else if (strcmp(arg, ARG_PLAYLIST_TEXT) == 0 || strcmp(arg, ARG_SHORT_PLAYLIST_TEXT) == 0) {
			// Call the dashboard page launcher function with the playlist page parameter to go to the playlist scene
			__DashboardPageLauncher(DASHBOARD_PLAYLIST);
			return;
		} else if (strcmp(arg, ARG_AUTO_TEXT) == 0 || strcmp(arg, ARG_SHORT_AUTO_TEXT) == 0) {
			// Call the dashboard page launcher function with the auto mode page parameter to go to the auto mode scene
			__DashboardPageLauncher(DASHBOARD_AUTO);
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
