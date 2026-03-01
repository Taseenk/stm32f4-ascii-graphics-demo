/**
 ******************************************************************************
 * @file           : shell.h
 * @brief          : Header file for the CLI shell interface, containing function 
 * prototypes and type definitions
 ******************************************************************************
 */

#ifndef __SHELL_H
#define __SHELL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* typedefs ------------------------------------------------------------------*/
// Shell error types for handling different error scenarios in the command parser
typedef enum {
    SHELL_ERROR_NONE = 0,           // No error occurred
    SHELL_ERROR_BAD_COMMAND,        // Command not recognized (e.g., "unknown.exe")
    SHELL_ERROR_INVALID_FLAG,       // Flag not recognized (e.g., "--wrong")
    SHELL_ERROR_INVALID_ARG,        // Argument provided for a flag is invalid (e.g., "--mode wrong")
    SHELL_ERROR_MISSING_ARG,        // Required argument for a flag is missing (e.g., "--mode" without an argument)
    SHELL_TOTAL_ERROR_TYPES         // Total number of error types (used for bounds checking)
} ShellError_t;

/* Function prototypes -------------------------------------------------------*/
// Initializes the CLI shell interface, rendering the system information and ready prompt 
void ShellInit(void);

// Command parser function prototype for processing user input from the CLI shell
void ShellCommandParser(char *rx_buffer);

#endif /* __SHELL_H */