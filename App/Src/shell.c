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
#include "shell_strings.h"
#include "dashboard.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <string.h>

/* Private Defines -----------------------------------------------------------*/
// Terminal layout definitions for the CLI shell interface
#define SHELL_COL_POSITION			1		// Starting column for the shell texts
#define INPUT_COL_POSITION			10		// Starting column for the user input text (after the prompt)
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
#define UPPERCASE_A         'A'     // ASCII value for uppercase 'A'
#define UPPERCASE_Z         'Z'     // ASCII value for uppercase 'Z'
#define LOWERCASE_OFFSET    32      // Offset to convert uppercase letters to lowercase in ASCII

// Error message formatting required rows for the CLI shell
#define ERROR_MESSAGE_ROWS		2
#define HELP_KEY1_RUN_ROWS		10
#define HELP_KEY1_DEMO_ROWS		10
#define HELP_KEY2_SCENE_ROW		7
#define HELP_KEY2_MODE_ROW 		10

/* Private Variables ---------------------------------------------------------*/
static uint16_t input_row = 16;		// Row position for the user input prompt in the CLI shell

/* Private Function Prototypes -----------------------------------------------*/
static void __PrintInputPrompt(uint16_t row);

static void __EnsureTerminalSpace(uint8_t required_space);
static void __DisplayErrorMessage(ShellError_t error_type);

static void __ParseRunCommand(char *rx_buffer, uint8_t command_offset);

static void __ParseHelpCommand(char *rx_buffer, uint8_t command_offset);
static void __PrintHelpKey1Run(void);
static void __PrintHelpKey1Demo(void);
static void __PrintHelpKey2Mode(void);
static void __PrintHelpKey2Scene(void);

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
	} else {
		input_row++;
	}
}

/**
 * @fn static void __DisplayErrorMessage(char *input_buffer, ShellError_t error_type)
 * @brief Handles the logic for displaying error messages in the CLI shell when the user 
 * inputs an unrecognized command or invalid parameters. Follows VMS-style error message 
 * formatting conventions for consistency and clarity.
 * @param error_type An integer representing the type of error (e.g., 1 for unrecognized command, 2 for invalid
 * parameters).
 */
static void __DisplayErrorMessage(ShellError_t error_type)
{
	// Space for Error(1) + Gap(1)
	__EnsureTerminalSpace(ERROR_MESSAGE_ROWS);

	// Set red text colour for error messages
	TerminalSetColour(FG_RED, BG_DEFAULT);

	switch (error_type) {
		case SHELL_ERROR_UNKNOWN_COMMAND:
			TerminalSerialPrintString(shell_error[SHELL_ERROR_UNKNOWN_COMMAND], SHELL_COL_POSITION, input_row++);
			break;

		case SHELL_ERROR_MISSING_TOPIC:
			TerminalSerialPrintString(shell_error[SHELL_ERROR_MISSING_TOPIC], SHELL_COL_POSITION, input_row++);
			break;

		case SHELL_ERROR_UNKNOWN_TOPIC:
			TerminalSerialPrintString(shell_error[SHELL_ERROR_UNKNOWN_TOPIC], SHELL_COL_POSITION, input_row++);
			break;

		case SHELL_ERROR_UNKNOWN_QUALIFIER:
			TerminalSerialPrintString(shell_error[SHELL_ERROR_UNKNOWN_QUALIFIER], SHELL_COL_POSITION, input_row++);
			break;

		case SHELL_ERROR_INVALID_PARAMETER:
			TerminalSerialPrintString(shell_error[SHELL_ERROR_INVALID_PARAMETER], SHELL_COL_POSITION, input_row++);
			break;

		default:
			TerminalSerialPrintString(shell_error[6], SHELL_COL_POSITION, input_row++);
			break;
	}

	// Set default colours for helper text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Prompt the user for the next command
	__PrintInputPrompt(++input_row);
}

/**
 * @fn static void __ParseRunCommand(char *rx_buffer, uint8_t command_offset)
 * @brief Parses the arguments provided for the run command and executes the
 * appropriate action based on the arguments.
 * @param rx_buffer The buffer containing the user input string to parse for the run command.
 * @param command_offset The offset in the buffer where the command arguments start after the command text.
 */
static void __ParseRunCommand(char *rx_buffer, uint8_t command_offset)
{
	// Argument texts for parsing the command
	static const char args_delimiter[] = " ";

	// Extract the command from the buffer after the command and parse them
	char *topic = strtok(rx_buffer + command_offset, args_delimiter);

	// No topic provided (e.g. user typed "RUN" with nothing after)
	if (topic == NULL) {
		__DisplayErrorMessage(SHELL_ERROR_MISSING_TOPIC);
		return;
	}

	// Parse the provided topic and go to the appropriate action based on the provided topic and qualifier
	/* --- Case: RUN Demo --- */
	if (strcmp(topic, demo_paremeter_text) == 0) {
		char *qualifier = strtok(NULL, args_delimiter);

		/* --- Case: RUN Demo--- */
		// If there is no qualifier provided after "RUN DEMO"
		if (qualifier == NULL) {
			// Call the dashboard page launcher swithing to the auto mode page by default if no qualifier is provided to
			// run demo
			__NavigateToDashboard(DASHBOARD_PLAYLIST);
		}

		/* --- Case: Mode qualifier (e.g., "RUN DEMO /MODE=auto") --- */
		else if (strcmp(qualifier, auto_mode_qualifier_text) == 0) {
			// Call the dashboard page launcher switching to the auto mode page if the auto mode qualifier is provided
			// to run demo
			__NavigateToDashboard(DASHBOARD_AUTO);
		}
		/* --- Case: Mode qualifier (e.g., "RUN DEMO /Mode=playlist") --- */
		else if (strcmp(qualifier, playlist_mode_qualifier_text) == 0) {
			// Call the dashboard page launcher switching to the playlist page if the playlist mode qualifier is
			// provided to run demo
			__NavigateToDashboard(DASHBOARD_PLAYLIST);
		}
		/* --- Case: UNKNOWN qualifier --- */
		else {
			// Qualifier provided after the topic is not recognized, display an error message
			__DisplayErrorMessage(SHELL_ERROR_UNKNOWN_QUALIFIER);
		}
	}

	/* --- Case: UNKNOWN topic --- */
	else {
		// Topic provided does not match any known RUN topics, display an error message
		__DisplayErrorMessage(SHELL_ERROR_UNKNOWN_TOPIC);
	}
}

/**
 * @fn static void __ParseHelpCommand(char *rx_buffer, uint8_t command_offset)
 * @brief Parses the arguments provided for the help command and displays the
 * appropriate help information based on the arguments (e.g., "demo" to show the demo command flags).
 * @param rx_buffer The buffer containing the user input string to parse for the help command.
 * @param command_offset The offset in the buffer where the command arguments start after the command text.
 */
static void __ParseHelpCommand(char *rx_buffer, uint8_t command_offset)
{
	// Argument texts for parsing the command
	static const char args_delimiter[] = " ";
	

	// Extract the command from the buffer after the command and parse them
	char *topic = strtok(rx_buffer + command_offset, args_delimiter);

	// If there is no command provided (e.g. user just typed "HELP")
	if (topic == NULL) {
		__DisplayErrorMessage(SHELL_ERROR_MISSING_TOPIC);
		return;
	}

	// Parse the provided topic and go to the appropriate help information based on the provided topic and qualifier
	/* --- Case: Demo topic --- */
	if (strcmp(topic, demo_paremeter_text) == 0) {
		char *qualifier = strtok(NULL, args_delimiter);

		/* --- Case: Help Demo (e.g., "HELP DEMO")--- */
		// If there is no qualifier provided after "HELP DEMO", display the general help information for the demo
		if (qualifier == NULL) {
			// Call the help command function to display the available qualifiers and their descriptions
			__PrintHelpKey1Demo();
			return;
		}

		/* --- Case: Mode qualifier (e.g., "HELP DEMO /MODE") --- */
		if (strcmp(qualifier, mode_qualifier_text) == 0) {
			// Print the MODE qualifier help information for the demo command
			__PrintHelpKey2Mode();
			return;
		}
		/* --- Case: Scene qualifier (e.g., "HELP DEMO /SCENE") --- */
		else if (strcmp(qualifier, scene_qualifier_text) == 0) {
			// Print the scene qualifier help information for the demo command
			__PrintHelpKey2Scene();
			return;
		}
		/* --- Case: UNKNOWN qualifier --- */
		else {
			// Qualifier provided after the topic is not recognized, display an error message
			__DisplayErrorMessage(SHELL_ERROR_UNKNOWN_QUALIFIER);
		}
	}

	/* --- Case: Run topic --- */
	else if (strncmp(topic, run_command_text, RUN_COMMAND_TEXT_LEN) == 0) {
		__PrintHelpKey1Run();
	}

	/* --- Case: UNKNOWN topic --- */
	else {
		// Topic provided does not match any known help topics, display an error message
		__DisplayErrorMessage(SHELL_ERROR_UNKNOWN_TOPIC);
	}
}

/**
 * @fn static void __PrintHelpKey1Run(void)
 * @brief Handles the logic for displaying help information about the "run" topic.
 * When the user types "HELP RUN" in the dashboard shell, including usage instructions,
 * @param void This function does not take any parameters.
 */
static void __PrintHelpKey1Run(void)
{
	// Ensure space for Topic(1), Gap(1), Desc(2), Gap(1), Format(1), Gap(1), Header(1), List(1)
	__EnsureTerminalSpace(HELP_KEY1_RUN_ROWS);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print the help information for the run command from the shell strings module
	for (uint8_t i = 0; i < shell_help_run_len; i++) {
		TerminalSerialPrintString(shell_help_run[i], SHELL_COL_POSITION, input_row++);
	}

	// Prompt the user for the next command
	__PrintInputPrompt(++input_row);
}

/**
 * @fn static void __PrintHelpKey1Demo(void)
 * @brief Handles the logic for displaying help information about the "demo" topic.
 * When the user types "HELP DEMO" in the dashboard shell, including usage instructions,
 * description, and available qualifier for the demo command.
 * @param void This function does not take any parameters.
 */
static void __PrintHelpKey1Demo(void)
{
	// Ensure space for Topic(1), Gap(1), Desc(2), Gap(1), Format(1), Gap(1), Header(1), List(1)
	__EnsureTerminalSpace(HELP_KEY1_DEMO_ROWS);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print the help information for the demo command from the shell strings module
	for (uint8_t i = 0; i < shell_help_demo_len; i++) {
		TerminalSerialPrintString(shell_help_demo[i], SHELL_COL_POSITION, input_row++);
	}

	// Prompt the user for the next command
	__PrintInputPrompt(++input_row);
}

/**
 * @fn static void __PrintHelpKey2Scene(void)
 * @brief Handles the logic for displaying help information about the available
 * scenes for the demo command when the user types "HELP DEMO /SCENE" in the dashboard shell,
 * including usage instructions and descriptions for each scene option.
 * @param void This function does not take any parameters.
 */
static void __PrintHelpKey2Scene(void)
{
	// Ensure space for Path(1), qualifier(1), Gap(1), Desc(7)
	__EnsureTerminalSpace(HELP_KEY2_SCENE_ROW);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print the help information for the demo command's scene qualifier from the shell strings module
	for (uint8_t i = 0; i < shell_help_subkey_scene_len; i++) {
		TerminalSerialPrintString(shell_help_subkey_scene[i], SHELL_COL_POSITION, input_row++);
	}

	// Prompt the user for the next command
	__PrintInputPrompt(++input_row);
}

/**
 * @fn static void __PrintHelpKey2Mode(void)
 * @brief Handles the logic for displaying help information about the available
 * modes for the demo command when the user types "HELP DEMO /MODE" in the dashboard shell,
 * including usage instructions and descriptions for each mode option.
 * @param void This function does not take any parameters.
 */
static void __PrintHelpKey2Mode(void)
{
	// Ensure space for Path(1), qualifier(1), Gap(1), Desc(2), Gap(1), Desc(1), Options(2), Gap(1)
	__EnsureTerminalSpace(HELP_KEY2_MODE_ROW);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print the help information for the mode qualifier of the demo command from the shell strings module
	for (uint8_t i = 0; i < shell_help_subkey_mode_len; i++) {
		TerminalSerialPrintString(shell_help_subkey_mode[i], SHELL_COL_POSITION, input_row++);
	}

	// Prompt the user for the next command
	__PrintInputPrompt(++input_row);
}

/**
 * @fn static void __NavigateToDashboard(DashboardPages_t page)
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
	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Render the system header
	TerminalSerialPrintString(shell_boot[0], SHELL_COL_POSITION, NAME_ROW_POSITION);
	TerminalSerialPrintString(shell_boot[1], SHELL_COL_POSITION, COPYRIGHT_ROW_POSITION);
	HAL_Delay(150);

	// Render the hardware specs with delays to simulate a boot sequence
	TerminalSerialPrintString(shell_boot[2], SHELL_COL_POSITION, CPU_ROW_POSITION);
	HAL_Delay(300);
	TerminalSerialPrintString(shell_boot[3], SHELL_COL_POSITION, SRAM_ROW_POSITION);
	HAL_Delay(1000);
	TerminalSerialPrintString(shell_boot[4], SHELL_COL_POSITION, FLASH_ROW_POSITION);
	HAL_Delay(300);

	// Render the peripheral checks with delays to simulate a boot sequence
	TerminalSerialPrintString(shell_boot[5], SHELL_COL_POSITION, DMA_ROW_POSITION);
	HAL_Delay(450);
	TerminalSerialPrintString(shell_boot[6], SHELL_COL_POSITION, UART_ROW_POSITION);
	HAL_Delay(300);
	TerminalSerialPrintString(shell_boot[7], SHELL_COL_POSITION, TERMINAL_ROW_POSITION);
	HAL_Delay(1000);

	// Render the system ready message and hint message
	TerminalSerialPrintString(shell_boot[8], SHELL_COL_POSITION, READY_ROW_POSITION);
	TerminalSerialPrintString(shell_boot[9], SHELL_COL_POSITION, HINT_ROW_POSITION);
	HAL_Delay(200);

	// Render the interaction prompt
	__PrintInputPrompt(INPUT_ROW_POSITION);

	// Set the initial input row position for user commands
	input_row = INPUT_ROW_POSITION;
}

/**
 * @fn void ShellCommandParser(char *rx_buffer)
 * @brief Parses the user input from the CLI shell, extracting the command
 * and its arguments to determine which action to take based on the input.
 * @param rx_buffer The buffer containing the user input string to parse.
 */
void ShellCommandParser(char *rx_buffer)
{
	// Early exit if buffer is null or invalid (e.g., empty string)
	if (rx_buffer == NULL)
		return;

	// Convert the received buffer to lowercase for comparison
	// In ASCII, the difference between uppercase and lowercase letters is a offset of 32
	size_t buffer_len = strlen(rx_buffer);
	for (size_t i = 0; i < buffer_len; i++) {
		if (rx_buffer[i] >= UPPERCASE_A && rx_buffer[i] <= UPPERCASE_Z) {
			rx_buffer[i] += LOWERCASE_OFFSET;
		}
	}

	// Parse the command and call the appropriate handler function based on the command text
	/* --- Case: RUN command --- */
	if ((strncmp(rx_buffer, run_command_text, RUN_COMMAND_TEXT_LEN) == 0) && (rx_buffer[RUN_COMMAND_TEXT_LEN] == SPACE_CHAR ||
	    rx_buffer[RUN_COMMAND_TEXT_LEN] == '\0')) {
		// Call the run command parser function to handle the run command arguments and execute the appropriate action
		__ParseRunCommand(rx_buffer, RUN_COMMAND_TEXT_LEN);
	}
	/* --- Case: HELP command --- */
	else if ((strncmp(rx_buffer, help_command_text, HELP_COMMAND_TEXT_LEN) == 0) &&
	         (rx_buffer[HELP_COMMAND_TEXT_LEN] == SPACE_CHAR || rx_buffer[HELP_COMMAND_TEXT_LEN] == '\0')) {
		// Call the help command parser function to handle the help command arguments and display the appropriate
		// help information
		__ParseHelpCommand(rx_buffer, HELP_COMMAND_TEXT_LEN);
	}
	/* --- Case: UNKNOWN command --- */
	else {
		// Command not recognized (e.g., "FLY DEMO")
		__DisplayErrorMessage(SHELL_ERROR_UNKNOWN_COMMAND);
		return;
	}
}
