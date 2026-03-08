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
static const char demo_topic_text[] = "demo";

#define RUN_COMMAND_TEXT_LEN		3	// Length of the "run" command text for parsing user input
#define HELP_COMMAND_TEXT_LEN		4	// Length of the "help" command text for parsing user input

#define UPPERCASE_A         'A'     // ASCII value for uppercase 'A'
#define UPPERCASE_Z         'Z'     // ASCII value for uppercase 'Z'
#define LOWERCASE_OFFSET    32      // Offset to convert uppercase letters to lowercase in ASCII

/* Private Variables ---------------------------------------------------------*/
static uint16_t input_row = 16;		// Row position for the user input prompt in the CLI shell

/* Private Function Prototypes -----------------------------------------------*/
static void __PrintInputPrompt(uint16_t row);

static void __EnsureTerminalSpace(uint8_t required_space);
static void __DisplayErrorMessage(ShellError_t error_type);

static void __ParseRunCommand(char *rx_buffer, uint8_t command_offset);

static void __ParseHelpCommand(char *rx_buffer, uint8_t command_offset);
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
    __EnsureTerminalSpace(2);

	// Set red text colour for error messages
	TerminalSetColour(FG_RED, BG_DEFAULT);

	switch (error_type) {
		case SHELL_ERROR_UNKNOWN_COMMAND:
			TerminalSerialPrintString("%SYSTEM-E-UNRECOGNIZED, command not found", SHELL_COL_POSITION, input_row++);

			// break out of the switch
			break;

		case SHELL_ERROR_MISSING_TOPIC:
			TerminalSerialPrintString("%HELP-E-NOTOPIC, please specify a help topic (e.g., HELP DEMO)", SHELL_COL_POSITION, input_row++);
			
			// break out of the switch
			break;

		case SHELL_ERROR_UNKNOWN_TOPIC:
			TerminalSerialPrintString("%HELP-E-UNKNOWNTOPIC, no documentation available for that topic", SHELL_COL_POSITION, input_row++);
			
			// break out of the switch
			break;

		case SHELL_ERROR_UNKNOWN_QUALIFIER:
			TerminalSerialPrintString("%SHELL-E-INVQUAL, unrecognized qualifier in command string", SHELL_COL_POSITION, input_row++);
			
			// break out of the switch
			break;

		case SHELL_ERROR_INVALID_PARAMETER:
			TerminalSerialPrintString("%SHELL-E-INVPARAM, invalid parameter value provided", SHELL_COL_POSITION, input_row++);
			
			// break out of the switch
			break;

		default:
			TerminalSerialPrintString("%SYSTEM-F-ANOMALY, an unexpected shell error occurred", SHELL_COL_POSITION, input_row++);
			
			// break out of the switch
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

	// If there is no command provided (e.g. user just typed "HELP")
	if (topic == NULL) {
		__DisplayErrorMessage(SHELL_ERROR_MISSING_TOPIC);
		return;
	}

	while (topic != NULL) {
		/* --- Case: RUN Demo --- */
		if (strcmp(topic, demo_topic_text) == 0) {
			return;
		}

		/* --- Case: UNKNOWN topic --- */
		else {
			// Topic provided does not match any known help topics, display an error message
			__DisplayErrorMessage( SHELL_ERROR_UNKNOWN_TOPIC);
		}

		// Move to the next argument in the buffer
		topic = strtok(NULL, args_delimiter);
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
	while (topic != NULL) {
		/* --- Case: Demo topic --- */
		if (strcmp(topic, demo_topic_text) == 0) {
			char *qualifier = strtok(NULL, args_delimiter);

			/* --- Case: Help Demo (e.g., "HELP DEMO")--- */
			// If there is no qualifier provided after "HELP DEMO", display the general help information for the demo
			if (qualifier == NULL) {
				// Call the help command function to display the available qualifiers and their descriptions
				__PrintHelpKey1Demo();
				return;
			}

			/* --- Case: Mode qualifier (e.g., "HELP DEMO /MODE") --- */
			if (strcmp(qualifier, "/mode") == 0) {
				// Print the MODE qualifier help information for the demo command
				__PrintHelpKey2Mode();
				return;
			} 
			/* --- Case: Scene qualifier (e.g., "HELP DEMO /SCENE") --- */
			else if (strcmp(qualifier, "/scene") == 0) {
				// Print the scene qualifier help information for the demo command
				__PrintHelpKey2Scene();
				return;
			} 
			/* --- Case: UNKNOWN qualifier --- */
			else {
				// Qualifier provided after the topic is not recognized, display an error message
				__DisplayErrorMessage( SHELL_ERROR_UNKNOWN_QUALIFIER);
			}
		}

		/* --- Case: UNKNOWN topic --- */
		else {
			// Topic provided does not match any known help topics, display an error message
			__DisplayErrorMessage( SHELL_ERROR_UNKNOWN_TOPIC);
		}

		// Move to the next argument in the buffer
		topic = strtok(NULL, args_delimiter);
	}
}

/**
 * @fn static void __PrintHelpKey1Demo(void)
 * @brief Handles the logic for displaying help information about the "demo" topic
 * when the user types "HELP DEMO" in the dashboard shell, including usage instructions,
 * description, and available qualifier for the demo command.
 * @param void This function does not take any parameters.
 */
static void __PrintHelpKey1Demo(void)
{
	static const char key1_topic[] = "DEMO";
	static const char help_line1[] = "  Invokes the DEMO program showcasing various scenes";
	static const char help_line2[] = "  and graphics capabilities of the system.";
	static const char format_text[] = "  Format:  DEMO [/MODE=type or /SCENE=name]";

	static const char additional_info_header[] = "Additional information available:";
	static const char qualifiers_list[] = "  /MODE      /SCENE";

	// Ensure space for Topic(1), Gap(1), Desc(2), Gap(1), Format(1), Gap(1), Header(1), List(1)
	__EnsureTerminalSpace(9);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print the key topic header followed by a gap
	TerminalSerialPrintString(key1_topic, SHELL_COL_POSITION, input_row++);
	input_row++;

	// Print the help text and format instructions for the demo command followed by a gap
	TerminalSerialPrintString(help_line1, SHELL_COL_POSITION, input_row++);
	TerminalSerialPrintString(help_line2, SHELL_COL_POSITION, input_row++);
	input_row++;
	TerminalSerialPrintString(format_text, SHELL_COL_POSITION, input_row++);
	input_row++;

	// Print the additional information header followed by the list of qualifiers for the demo command
	TerminalSerialPrintString(additional_info_header, SHELL_COL_POSITION, input_row++);
	input_row++;
	TerminalSerialPrintString(qualifiers_list, SHELL_COL_POSITION, input_row++);

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
	// Text for the path header and qualifier header
	static const char path_header[] = "DEMO";
    static const char qualifier_header[] = "  /SCENE";

	// Description lines for the /SCENE qualifier of the demo topic
	static const char desc_line1[] = "    /SCENE=name";
    static const char desc_line2[] = "    Specifies the graphics scene to launch immediately, bypassing";
    static const char desc_line3[] = "    the interactive dashboard menu.";

	// Ensure space for Path(1), qualifier(1), Gap(1), Desc(7)
	__EnsureTerminalSpace(7);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	//
	TerminalSerialPrintString(path_header, SHELL_COL_POSITION, input_row++);
	TerminalSerialPrintString(qualifier_header, SHELL_COL_POSITION, input_row++);
    input_row++;

	//
	TerminalSerialPrintString(desc_line1, SHELL_COL_POSITION, input_row++);
    TerminalSerialPrintString(desc_line2, SHELL_COL_POSITION, input_row++);
	TerminalSerialPrintString(desc_line3, SHELL_COL_POSITION, input_row++);

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
	// Text for the path header and qualifier header
	static const char path_header[] = "DEMO";
	static const char qualifier_header[] = "  /MODE";

	// Description lines for the /MODE qualifier of the demo topic
	static const char desc_line1[] = "    /Mode=name";
	static const char desc_line2[] = "    Specifies the playback behavior for the DEMO program.";
	static const char desc_line3[] = "    Valid modes are:";

	// Explicit Parameter Definitions
    static const char opt_auto[]    = "      AUTO      Displays every scene sequentially at set intervals.";
    static const char opt_play[]    = "      PLAYLIST  Plays a curated list of specific scenes back-to-back.";

	// Ensure space for Path(1), qualifier(1), Gap(1), Desc(2), Gap(1), Desc(1), Options(2)
	__EnsureTerminalSpace(9);

	// Set default colours for the main body text
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

	// Print the path header, qualifier header
	TerminalSerialPrintString(path_header, SHELL_COL_POSITION, input_row++);
	TerminalSerialPrintString(qualifier_header, SHELL_COL_POSITION, input_row++);
	input_row++;

	// Print the description lines for the /MODE qualifier of the demo command
	TerminalSerialPrintString(desc_line1, SHELL_COL_POSITION, input_row++);
	TerminalSerialPrintString(desc_line2, SHELL_COL_POSITION, input_row++);
	input_row++;
	TerminalSerialPrintString(desc_line3, SHELL_COL_POSITION, input_row++);
	
	// Print the valid mode options for the /MODE qualifier of the demo command
	TerminalSerialPrintString(opt_auto, SHELL_COL_POSITION, input_row++);
    TerminalSerialPrintString(opt_play, SHELL_COL_POSITION, input_row++);
	input_row++;

	// Prompt the user for the next command
	__PrintInputPrompt(++input_row);
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
	
	// Text for the run and help commands to compare against the user input
	static const char run_command_text[] = "run";
	static const char help_command_text[] = "help";

	// Parse the command and call the appropriate handler function based on the command text
	/* --- Case: RUN command --- */
	if (strncmp(rx_buffer, run_command_text, RUN_COMMAND_TEXT_LEN) == 0) {
		// Call the run command parser function to handle the run command arguments and execute the appropriate action
		__ParseRunCommand(rx_buffer, RUN_COMMAND_TEXT_LEN);
	}
	/* --- Case: HELP command --- */
	else if (strncmp(rx_buffer, help_command_text, HELP_COMMAND_TEXT_LEN) == 0) {
		// Call the help command parser function to handle the help command arguments and display the appropriate help information
		__ParseHelpCommand(rx_buffer, HELP_COMMAND_TEXT_LEN);
	}
	/* --- Case: UNKNOWN command --- */
	else {
		// Command not recognized (e.g., "FLY DEMO")
		__DisplayErrorMessage(SHELL_ERROR_UNKNOWN_COMMAND);
		return;
	}
}
