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