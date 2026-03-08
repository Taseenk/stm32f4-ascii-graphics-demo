/**
 ******************************************************************************
 * @file           : shell.c
 * @brief          : Implements the CLI shell interface for the ASCII graphics demo,
 * allowing users to interact with the system through a terminal interface and 
 * execute commands to navigate the dashboard and access different features.
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
#include <stdint.h>
#include <string.h>

/* Private Defines -----------------------------------------------------------*/
// Terminal layout definitions for the CLI shell interface
#define SHELL_COL_POSITION			1		// Starting column for the shell texts
#define INPUT_COL_POSITION			6		// Starting column for the user input text (after the prompt)
#define NAME_ROW_POSITION           1		// Starting row for the system name text
#define COPYRIGHT_ROW_POSITION      (NAME_ROW_POSITION + 1)

#define CPU_ROW_POSITION            (COPYRIGHT_ROW_POSITION + 2)
#define SRAM_ROW_POSITION           (CPU_ROW_POSITION + 1)
#define FLASH_ROW_POSITION          (SRAM_ROW_POSITION + 1)

#define DMA_ROW_POSITION            (FLASH_ROW_POSITION + 2)
#define UART_ROW_POSITION           (DMA_ROW_POSITION + 1)
#define TERMINAL_ROW_POSITION       (UART_ROW_POSITION + 1)

#define READY_ROW_POSITION          (TERMINAL_ROW_POSITION + 2)

#define HINT_ROW_POSITION           (READY_ROW_POSITION + 2)
#define INPUT_ROW_POSITION          (HINT_ROW_POSITION + 2)

// Shell Command Parsing
#define COMMAND_TEXT_LEN	8		// Length of the command text without null terminator

#define UPPERCASE_A         'A'     // ASCII value for uppercase 'A'
#define UPPERCASE_Z         'Z'     // ASCII value for uppercase 'Z'
#define LOWERCASE_OFFSET    32      // Offset to convert uppercase letters to lowercase in ASCII

/* Private Variables ---------------------------------------------------------*/
static uint16_t input_row = 16; // Row position for the user input prompt in the CLI shell

/* Private Function Prototypes -----------------------------------------------*/
static void __PrintInputPrompt(uint16_t row);

static void __EnsureTerminalSpace(uint8_t required_space);

static void __HelpCommand(void);
static void __DisplayErrorMessage(char *input_buffer, ShellError_t error_type);

static void __NavigateToDashboard(DashboardPages_t page);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void __PrintInputPrompt(uint16_t row)
 * @brief Displays the input prompt at the specified row and sets the cursor
 * position for user input in the CLI shell.
 * @param row The row position where the input prompt should be displayed.
 */
static void __PrintInputPrompt(uint16_t row)
{
	// The prompt string to display before the user input
	static const char prompt[] = "STM32F4>";

	// Print the input prompt at the current input row
	TerminalSerialPrintString(prompt, SHELL_COL_POSITION, row);

	// Move the cursor to the input position and enable it for user input
	TerminalSetCursorPos(INPUT_COL_POSITION, row);
	TerminalVisibleCursor();
}

/**
 * @fn static void __EnsureTerminalSpace(uint8_t required_space)
 * @brief Checks if there is enough space left in the terminal to print a new
 * message without overflowing the bottom of the screen. If there is not enough
 * space, it clears the terminal and resets the input row to prevent overflow.
 * @param required_space The number of rows required to print the new message and any additional helper text.
 */
static void __EnsureTerminalSpace(uint8_t required_space)
{
	// Check if the new messages will overflow the terminal height
	if ((input_row + required_space) >= TERMINAL_HEIGHT) {
		// Clear the terminal and reset the input row
		TerminalClearAndHome();
		input_row = 1;
	}
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
	static const char *flags[] = {"Available flags for demo.exe:", "  -h, --help     :\tShow help",
	                              "  -p, --playlist :\tPlaylist Mode", "  -a, --auto     :\tAuto Mode"};

	// Calculate the number of flags in the array for iteration
	const uint8_t flags_count = sizeof(flags) / sizeof(flags[0]);

	// Check if there is enough space to print the error message and helper text
	// if not clear the screen and reset the input row
	__EnsureTerminalSpace(6);

	// Set cyan text colour for the help command output
	TerminalSetColour(FG_CYAN, BG_DEFAULT);
	TerminalSerialPrintString(flags[0], SHELL_COL_POSITION, input_row);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print each flag and its description at its respective position
	for (int i = 1; i < flags_count; i++) {
		input_row++;
		TerminalSerialPrintString(flags[i], SHELL_COL_POSITION, input_row);
	}

	// Increment the input row and prompt the user for the next command
	input_row++;
	__PrintInputPrompt(++input_row);
}

/**
 * @fn void __DisplayErrorMessage(char *input_buffer, ShellError_t error_type)
 * @brief Handles the logic for displaying error messages in the CLI shell
 * when the user inputs an unrecognized command or invalid parameters.
 * @param input_buffer The buffer containing the user input string that caused the error.
 * @param error_type An integer representing the type of error (e.g., 1 for unrecognized command, 2 for invalid
 * parameters).
 */
static void __DisplayErrorMessage(char *input_buffer, ShellError_t error_type)
{
	static const char error_unknown[] = "An unknown error occurred.";
	static const char error_prefix[] = "'";
	static const char error_bad_cmd[] = "' is not recognized as a command.";
	static const char error_param[] = "Invalid parameter: ";
	static const char hint_message[] = "Type 'demo.exe --help or -h' for options.";

	// Check if there is enough space to print the error message and helper text
	// if not clear the screen and reset the input row
	__EnsureTerminalSpace(4);

	// Set red text colour for error messages
	TerminalSetColour(FG_RED, BG_DEFAULT);

	// Print the appropriate error message based on the error type
	TerminalSetCursorPos(SHELL_COL_POSITION, input_row);
	switch (error_type) {
		case SHELL_ERROR_BAD_COMMAND:
			SerialPrint(error_prefix);
            SerialPrint(input_buffer);
            SerialPrintLn(error_bad_cmd);
			// break out of the switch
			break;

		case SHELL_ERROR_INVALID_PARAM:
			SerialPrint(error_param);
			SerialPrintLn(input_buffer);
			// break out of the switch
			break;

		default:
			SerialPrintLn(error_unknown);
			// break out of the switch
			break;
	}

	// Set default colours for helper text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	input_row += 2;
	TerminalSetCursorPos(SHELL_COL_POSITION, input_row);
	SerialPrintLn(hint_message);

	// Increment the input row and prompt the user for the next command
	input_row += 2;
	__PrintInputPrompt(input_row);
}

/**
 * @fn void __NavigateToDashboard(DashboardPages_t page)
 * @brief Handles the logic for launching different dashboard pages based on the
 * provided page parameter. It sets the system mode to the dashboard and initializes
 * the main page with the specified dashboard page.
 * @param page An integer representing the specific dashboard page to launch (e.g., playlist, auto mode).
 */
static void __NavigateToDashboard(DashboardPages_t page)
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
	// Shell Initialization Texts
	static const char name_text[] = "STM32F407VG BIOS v1.0.4";
	static const char copyright_text[] = "(C) 2026 Taseen ASCII Graphics Demo";
	static const char cpu_text[] = "CPU: ARM Cortex-M4 @ 168MHz (PLL_LOCKED)";
	static const char sram_text[] = "SRAM: 128KB OK";
	static const char flash_text[] = "FLASH: 1024KB OK";
	static const char dma_text[] = "DMA Controller... Initialized";
	static const char uart_text[] = "UART2 Terminal... Connected at 921600bps";
	static const char terminal_text[] = "Display Mode... 80x24 ANSI Color";
	static const char ready_text[] = "System is ready...";
	static const char hint_message[] = "Type 'HELP DEMO' for command usage information.";

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Render the system header
	TerminalSerialPrintString(name_text, SHELL_COL_POSITION, NAME_ROW_POSITION);
	TerminalSerialPrintString(copyright_text, SHELL_COL_POSITION, COPYRIGHT_ROW_POSITION);
	HAL_Delay(150);

	// Render the hardware specs with delays to simulate a boot sequence
	TerminalSerialPrintString(cpu_text, SHELL_COL_POSITION, CPU_ROW_POSITION);
	HAL_Delay(300);
	TerminalSerialPrintString(sram_text, SHELL_COL_POSITION, SRAM_ROW_POSITION);
	HAL_Delay(1000);
	TerminalSerialPrintString(flash_text, SHELL_COL_POSITION, FLASH_ROW_POSITION);
	HAL_Delay(300);

	// Render the peripheral checks with delays to simulate a boot sequence
	TerminalSerialPrintString(dma_text, SHELL_COL_POSITION, DMA_ROW_POSITION);
	HAL_Delay(450);
	TerminalSerialPrintString(uart_text, SHELL_COL_POSITION, UART_ROW_POSITION);
	HAL_Delay(300);
	TerminalSerialPrintString(terminal_text, SHELL_COL_POSITION, TERMINAL_ROW_POSITION);
	HAL_Delay(1000);

	// Render the system ready message
	TerminalSerialPrintString(ready_text, SHELL_COL_POSITION, READY_ROW_POSITION);

	// Render the interaction prompt
	TerminalSerialPrintString(hint_message, SHELL_COL_POSITION, HINT_ROW_POSITION);
	HAL_Delay(200);
	__PrintInputPrompt(INPUT_ROW_POSITION);
}

/**
 * @fn void ShellCommandParser(char *rx_buffer)
 * @brief Parses the user input from the CLI shell, extracting the command
 * and its arguments to determine which action to take based on the input.
 * @param rx_buffer The buffer containing the user input string to parse.
 */
void ShellCommandParser(char *rx_buffer)
{
	static const char command_text[] = "demo.exe"; // Expected command text to trigger the demo command parsing logic

	// Argument texts for parsing the command flags
	static const char argument_delimiter[] = " ";
	static const char arg_help_text[] = "--help";
	static const char arg_auto_text[] = "--auto";
	static const char arg_playlist_text[] = "--playlist";
	static const char arg_short_help_text[] = "-h";
	static const char arg_short_auto_text[] = "-a";
	static const char arg_short_playlist_text[] = "-p";

	// Early exit if buffer is null or invalid (e.g., empty string)
	if (rx_buffer == NULL)
		return;

	// Calculate the length of the received buffer
	size_t buffer_len = strlen(rx_buffer);

	// Convert the received buffer to lowercase for comparison
	for (size_t i = 0; i < buffer_len; i++) {
		if (rx_buffer[i] >= UPPERCASE_A && rx_buffer[i] <= UPPERCASE_Z) {
			rx_buffer[i] += LOWERCASE_OFFSET;
		}
	}

	// Check if the received command starts with the expected command text
	if (strncmp(rx_buffer, command_text, COMMAND_TEXT_LEN) != 0) {
		// Command does not match, display an error message
		__DisplayErrorMessage(rx_buffer, SHELL_ERROR_BAD_COMMAND);
		return;
	}

	// Extract the arguments from the buffer after the command and parse them
	char *arg = strtok(rx_buffer + COMMAND_TEXT_LEN, argument_delimiter);

	// If there are no arguments provided, go to the default scene
	if (arg == NULL) {
		// Call the dashboard page launcher function with the playlist page parameter to go to the playlist scene
		__NavigateToDashboard(DASHBOARD_PLAYLIST);
		return;
	}

	// Parse the arguments and go to the appropriate scene based on the provided flag
	while (arg != NULL) {
		if (strcmp(arg, arg_help_text) == 0 || strcmp(arg, arg_short_help_text) == 0) {
			// Call the help command function to display the available flags and their descriptions
			__HelpCommand();
			return;
		} else if (strcmp(arg, arg_playlist_text) == 0 || strcmp(arg, arg_short_playlist_text) == 0) {
			// Call the dashboard page launcher function with the playlist page parameter to go to the playlist scene
			__NavigateToDashboard(DASHBOARD_PLAYLIST);
			return;
		} else if (strcmp(arg, arg_auto_text) == 0 || strcmp(arg, arg_short_auto_text) == 0) {
			// Call the dashboard page launcher function with the auto mode page parameter to go to the auto mode scene
			__NavigateToDashboard(DASHBOARD_AUTO);
			return;
		} else {
			// Argument does not match, display an error message
			__DisplayErrorMessage(arg, SHELL_ERROR_INVALID_PARAM);
			return;
		}

		// Move to the next argument in the buffer
		arg = strtok(NULL, argument_delimiter);
	}
}
