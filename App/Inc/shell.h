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
    SHELL_ERROR_UNKNOWN_COMMAND,    // Command not recognized (e.g., "FLY DEMO")
    SHELL_ERROR_MISSING_TOPIC,      // Command exists, but no topic (e.g., "HELP")
    SHELL_ERROR_UNKNOWN_TOPIC,      // Topic not in library (e.g., "HELP PROJECT")
    SHELL_ERROR_UNKNOWN_QUALIFIER,  // Key-2 not recognized (e.g., "HELP DEMO /FAST")
    SHELL_ERROR_INVALID_PARAMETER   // Qualifier value is wrong (e.g., "DEMO /MODE=TURBO")
} ShellError_t;

/* Function prototypes -------------------------------------------------------*/
// Initializes the CLI shell interface, rendering the system information and ready prompt 
void ShellInit(void);

// Command parser function prototype for processing user input from the CLI shell
void ShellCommandParser(char *rx_buffer);

#endif /* __SHELL_H */