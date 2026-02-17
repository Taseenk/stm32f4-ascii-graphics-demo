/**
 ******************************************************************************
 * @file           : dashboard.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __DASHBOARD_H
#define __DASHBOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
// Shell Initialization Texts
#define NAME_TEXT           "STM32F407VG BIOS v1.0.4"
#define COPYRIGHT_TEXT      "(C) 2026 Taseen ASCII Graphics Demo"

#define CPU_TEXT            "CPU: ARM Cortex-M4 @ 168MHz (PLL_LOCKED)"
#define SRAM_TEXT           "SRAM: 128KB OK"
#define FLASH_TEXT          "FLASH: 1024KB OK"

#define DMA_TEXT            "DMA Controller... Initialized"
#define UART_TEXT           "UART2 Terminal... Connected at 115200bps"
#define TERMINAL_TEXT       "Display Mode... 80x24 ANSI Color"

#define READY_TEXT          "System is ready..."

#define HINT_TEXT           "Type 'demo.exe --help'"
#define INPUT_TEXT          "C:/>"

// Row and column positions for each shell Text
#define SHELL_COL_POSITION          1       // Starting column for the shell texts
#define INPUT_COL_POSITION          6       // Starting column for the user input text (after the prompt)
#define NAME_ROW_POSITION           1       // Starting row for the system name text
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
#define COMMAND_TEXT            "demo.exe"      // Expected command text to trigger the demo command parsing logic
#define COMMAND_TEXT_LEN        8               // Length of the command text without null terminator

#define ARGUMENT_DELIMITER      " "             // Delimiter used to separate command arguments in the input buffer
#define ARG_HELP_TEXT           "--help"        // Argument text for displaying help information about the command
#define ARG_AUTO_TEXT           "--auto"        // Argument text for automatically cycling through scenes
#define ARG_SELECT_TEXT         "--select"      // Argument text for selecting a specific scene to load

#define ARG_SHORT_HELP_TEXT     "-h"            // Short argument text for help
#define ARG_SHORT_AUTO_TEXT     "-a"            // Short argument text for auto
#define ARG_SHORT_SELECT_TEXT   "-s"            // Short argument text for select

#define UPPERCASE_A         'A'     // ASCII value for uppercase 'A'
#define UPPERCASE_Z         'Z'     // ASCII value for uppercase 'Z'
#define LOWERCASE_OFFSET    32      // Offset to convert uppercase letters to lowercase in ASCII

// Dashboard Status Bar Text
#define SYSTEM_TEXT         "[SYSTEM: STM32F407VG]"     // Text to display for system information
#define MAIN_PAGE_TEXT      "[MAIN MENU]"               // Text to display for the dashboard page
#define FPS_TEXT            "[FPS: ??]"                 // Placeholder text for FPS display

// Dashboard Status Bar Text sizes
#define STATUS_BAR_BUFFER_SIZE  (TERMINAL_WIDTH + ANSI_TEXT_LEN + 1)    // Total size of the status bar buffer, including ANSI escape codes and null terminator
#define ANSI_TEXT_LEN           8                                       // Length of the ANSI escape codes used for setting and resetting styling
#define SYSTEM_TEXT_LEN         sizeof(SYSTEM_TEXT) - 1                 // Length of the system text without null terminator
#define MAIN_PAGE_TEXT_LEN      sizeof(MAIN_PAGE_TEXT) - 1              // Length of the main page text without null terminator
#define FPS_TEXT_LEN            sizeof(FPS_TEXT) - 1                    // Length of the FPS text without null terminator

// Dashboard Status Bar Text positions
#define SYSTEM_TEXT_POSITION        1       // Starting column for system information text
#define MAIN_PAGE_TEXT_POSITION     34      // Middle of the screen
#define FPS_TEXT_POSITION           70      // Starting column for FPS text
#define FPS_VALUE_POSITION          77      // Position immediately after the FPS label

// Dashboard Main Menu Option Texts and positions
#define HELP_TEXT       " ?       HELP    -  Get help on using the application "
#define SELECT_TEXT     " S       SELECT  -  Select a scene to load "
#define AUTO_TEXT       " A       AUTO    -  Cycle through scenes "
#define INFO_TEXT       " I       INFO    -  About the application and system information "
#define QUIT_TEXT       " Q       QUIT    -  Exit the application "

#define OPTIONS_COL_POSITION        10      // Starting column for the dashboard option texts

// Row positions for each Main Menu option
#define OPTIONS_ROW_OFFSET          3       // Number of rows to space between each dashboard option
#define HELP_ROW_POSITION           5
#define SELECT_ROW_POSITION         (HELP_ROW_POSITION + OPTIONS_ROW_OFFSET)        
#define AUTO_ROW_POSITION           (SELECT_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define INFO_ROW_POSITION           (AUTO_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define QUIT_ROW_POSITION           (INFO_ROW_POSITION + OPTIONS_ROW_OFFSET)

// Dashboard Footer Texts
#define FOOTER_TEXT     "[ENTER] Select   [W/S] Navigate   [ESC] Back   (C) 2026 Taseen"

#define FOOTER_ROW_POSITION         23      // Row position for the dashboard footer
#define FOOTER_COL_POSITION         8       // Starting column for the dashboard footer

/* typedefs ------------------------------------------------------------------*/
// Shell error types for handling different error scenarios in the command parser
typedef enum {
    SHELL_ERROR_NONE = 0,           // No error occurred
    SHELL_ERROR_BAD_COMMAND,        // Command not recognized (e.g., "unknown.exe")
    SHELL_ERROR_INVALID_PARAM,      // Flag not recognized (e.g., "--wrong")
} ShellError_t;

/* Function prototypes -------------------------------------------------------*/
void DashboardShellInit(void);
void MainPageInit(void);

void DashboardShellCommandParser(char *rx_buffer);

// Functions to render different parts of the dashboard
void DashboardHeader(void);
void DashboardMenuList(void);
void DashboardFooter(void);

void DashboardFPSRefresh(uint32_t fps_counter);

#endif /* __DASHBOARD_H */