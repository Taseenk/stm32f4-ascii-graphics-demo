/**
 ******************************************************************************
 * @file           : shell.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __SHELL_H
#define __SHELL_H

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
#define UART_TEXT           "UART2 Terminal... Connected at 921600bps"
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
#define ARG_PLAYLIST_TEXT       "--playlist"    // Argument text for selecting the playlist scene
#define ARG_AUTO_TEXT           "--auto"        // Argument text for automatically cycling through scenes

#define ARG_SHORT_HELP_TEXT         "-h"        // Short argument text for help
#define ARG_SHORT_PLAYLIST_TEXT     "-p"        // Short argument text for playlist
#define ARG_SHORT_AUTO_TEXT         "-a"        // Short argument text for auto

#define UPPERCASE_A         'A'     // ASCII value for uppercase 'A'
#define UPPERCASE_Z         'Z'     // ASCII value for uppercase 'Z'
#define LOWERCASE_OFFSET    32      // Offset to convert uppercase letters to lowercase in ASCII

/* typedefs ------------------------------------------------------------------*/
// Shell error types for handling different error scenarios in the command parser
typedef enum {
    SHELL_ERROR_NONE = 0,           // No error occurred
    SHELL_ERROR_BAD_COMMAND,        // Command not recognized (e.g., "unknown.exe")
    SHELL_ERROR_INVALID_PARAM,      // Flag not recognized (e.g., "--wrong")
} ShellError_t;

/* Function prototypes -------------------------------------------------------*/
void ShellInit(void);
void ShellCommandParser(char *rx_buffer);

#endif /* __SHELL_H */